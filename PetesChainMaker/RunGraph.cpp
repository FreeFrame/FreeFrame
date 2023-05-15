#include "RunGraph.h"

#include <stdio.h>

#include "GraphUtil.h"

void RunGraph_InitNode(SPete_Node* pNode,void* pCookie);
void RunGraph_DeInitNode(SPete_Node* pNode,void* pCookie);
void RunGraph_RunNode(SPete_Node* pNode,void* pCookie);
void RunGraph_SetFFParamsInfo(SPete_Node* pNode,void* pCookie);

void RunGraph_RunPluginNode(SPete_Node* pNode,SPete_RunTimeEnvironment* pEnv);
void RunGraph_SetFFParams(SPete_EffectNode* pEffectNode);
float RunGraph_GetFFParamDefault(SPete_EffectNode* pEffectNode,int nIndex);
void RunGraph_SetFFParam(SPete_EffectNode* pEffectNode,int nIndex,float Value);
void RunGraph_RunParametersOnly(SPete_Node* pNode,void* pCookie);
void RunGraph_RunEffectsOnly(SPete_Node* pNode,void* pCookie);

void RunGraph_Init(SPete_GraphData* pGraph,int nWidth,int nHeight,SPete_RunTimeEnvironment* pEnv) {

	SPete_Node* pRootNode=pGraph->m_pRootNode;

	pEnv->m_pGraph=pGraph;

	pEnv->m_nVideoWidth=nWidth;
	pEnv->m_nVideoHeight=nHeight;

	const int nScreenByteCount=
		(pEnv->m_nVideoWidth*pEnv->m_nVideoHeight)*
		sizeof(U32);

	pEnv->m_pBlankInput=(U32*)malloc(nScreenByteCount);
	if (pEnv->m_pBlankInput==NULL) {
		MessageBox(NULL,"Out of memory!","",MB_OK);
		return;
	}

	Pete_ZeroMemory(pEnv->m_pBlankInput,nScreenByteCount);

	int nFFParamsCount=-1;
	GraphUtil_Iterate(pRootNode,GraphUtil_FindHighestFFParameter,&nFFParamsCount);

	nFFParamsCount+=1;

	pEnv->m_pFFParams=(float*)
		(malloc(nFFParamsCount*sizeof(float)));

	const int nParamsInfoByteCount=
		(nFFParamsCount*sizeof(SPete_PluginParameter));

	pEnv->m_pParamsInfo=(SPete_PluginParameter*)
		(malloc(nParamsInfoByteCount));

	pEnv->m_nFFParamsCount=nFFParamsCount;

	SPete_PluginParameter* pParamsInfo=pEnv->m_pParamsInfo;

	int nCount;
	for (nCount=0; nCount<nFFParamsCount; nCount+=1) {

		strcpy(pParamsInfo[nCount].m_pName,"Unused");
		pParamsInfo[nCount].m_Default=0.0f;

	}

	GraphUtil_Iterate(pRootNode,RunGraph_SetFFParamsInfo,pEnv->m_pParamsInfo);

	for (nCount=0; nCount<nFFParamsCount; nCount+=1) {

		pEnv->m_pFFParams[nCount]=
			pEnv->m_pParamsInfo[nCount].m_Default;

	}

	const int nMaxInputs=256;
	pEnv->m_ppInputs=(U32**)
		(malloc(nMaxInputs*sizeof(U32*)));
	if (pEnv->m_ppInputs==NULL) {
		MessageBox(NULL,"Out of memory!","",MB_OK);
		return;
	}

	Pete_ZeroMemory(pEnv->m_ppInputs,(nMaxInputs*sizeof(U32*)));

	GraphUtil_Iterate(pRootNode,RunGraph_InitNode,pEnv);

}


void RunGraph_DeInit(SPete_Node* pRootNode,SPete_RunTimeEnvironment* pEnv) {

	GraphUtil_Iterate(pRootNode,RunGraph_DeInitNode,pEnv);

	free(pEnv->m_pFFParams);

}

void RunGraph_Run(SPete_Node* pRootNode,SPete_RunTimeEnvironment* pEnv) {

//	GraphUtil_Iterate(pRootNode,RunGraph_RunNode,pEnv);

	GraphUtil_Iterate(pRootNode,RunGraph_RunParametersOnly,pEnv);
	GraphUtil_PrunedIterate(pRootNode,RunGraph_RunEffectsOnly,pEnv);

}

void RunGraph_InitNode(SPete_Node* pNode,void* pCookie) {

	SPete_RunTimeEnvironment* pEnv=(SPete_RunTimeEnvironment*)(pCookie);

	const int nScreenByteCount=
		(pEnv->m_nVideoWidth*pEnv->m_nVideoHeight)*
		sizeof(U32);

	switch (pNode->m_eType) {

		case eType_Effect_Output: {

		}break;

		case eType_Effect_Input: {

		}break;

		case eType_Effect_Plugin: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

			VideoInfoStruct VideoInfo;
			VideoInfo.frameWidth=pEnv->m_nVideoWidth;
			VideoInfo.frameHeight=pEnv->m_nVideoHeight;
			VideoInfo.bitDepth=FF_CAP_32BITVIDEO;

			const bool bLoadSucceded=
				GraphUtil_DoLoadPlugin(pEnv->m_pGraph,pPluginInfo);

			if (!bLoadSucceded) {
				char pMessageString[1024];
				sprintf(pMessageString,"FreeChain: Couldn't load plugin '%s'",pPluginInfo->m_pDLLPath);
				MessageBox(NULL,pMessageString,"",MB_OK);
				return;
			}

			FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

			if (pPluginInfo->m_nReferenceCount==1) {

				(*pMainFunc)(FF_INITIALISE,(void*)(&VideoInfo),0);

			}

			pEffectNode->m_InstanceCookie=(U32)
				(*pMainFunc)(FF_INSTANTIATE,(void*)(&VideoInfo),0);

			pEffectNode->m_pInternalOutputBuffer=(U32*)
				(malloc(nScreenByteCount));

			pEffectNode->m_pExternalOutputBuffer=(U32*)
				(malloc(nScreenByteCount));

			pEffectNode->m_pOutput=NULL;

		}break;

		case eType_Effect_Switcher: {

		}break;

		default: {
			// do nothing
		}break;

	}

}

void RunGraph_DeInitNode(SPete_Node* pNode,void* pCookie) {

	SPete_RunTimeEnvironment* pEnv=(SPete_RunTimeEnvironment*)(pCookie);

	switch (pNode->m_eType) {

		case eType_Effect_Plugin: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

			FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

			(*pMainFunc)(FF_DEINSTANTIATE,NULL,pEffectNode->m_InstanceCookie);

			if (pPluginInfo->m_nReferenceCount==1) {

				(*pMainFunc)(FF_DEINITIALISE,NULL,0);

			}

			GraphUtil_DoUnLoadPlugin(pPluginInfo);

			free(pEffectNode->m_pInternalOutputBuffer);
			pEffectNode->m_pInternalOutputBuffer=NULL;

			free(pEffectNode->m_pExternalOutputBuffer);
			pEffectNode->m_pExternalOutputBuffer=NULL;

			pEffectNode->m_pOutput=NULL;

		}break;

		default: {
			// do nothing
		}break;

	}


}

void RunGraph_RunNode(SPete_Node* pNode,void* pCookie) {

	SPete_RunTimeEnvironment* pEnv=(SPete_RunTimeEnvironment*)(pCookie);

	const int nScreenByteCount=
		(pEnv->m_nVideoWidth*pEnv->m_nVideoHeight)*
		sizeof(U32);

	switch (pNode->m_eType) {

		case eType_Effect_Output: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			U32* pInput=pEffectNode->m_ppInputs[0]->m_pOutput;
			U32* pOutput=pEnv->m_pOutput;
			memcpy(pOutput,pInput,nScreenByteCount);

		}break;

		case eType_Effect_Input: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			int nInputIndex=pEffectNode->m_nInputIndex;
			const int nInputTotal=pEnv->m_nInputsCount;

			if (nInputIndex>=(nInputTotal)) {
				nInputIndex=(nInputTotal-1);
			}

			pEffectNode->m_pOutput=pEnv->m_ppInputs[nInputIndex];

		}break;

		case eType_Effect_Plugin: {

			RunGraph_RunPluginNode(pNode,pEnv);

		}break;

		case eType_Effect_Switcher: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			const int nInputCount=pEffectNode->m_nSwitcherInputCount;

			const float ParamValue=pEffectNode->m_ppParameters[0]->m_OutputValue;

			const int nCurrentInput=(int)((ParamValue*(nInputCount-1))+0.5f);

			pEffectNode->m_pOutput=pEffectNode->m_ppInputs[nCurrentInput]->m_pOutput;

		}break;

		case eType_Parameter_External: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			const int nFFIndex=pParamNode->m_nExternalIndex;

			pParamNode->m_OutputValue=pEnv->m_pFFParams[nFFIndex];

		}break;

		case eType_Parameter_Constant: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		case eType_Parameter_Keyboard: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		case eType_Parameter_Midi: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		default: {
			// do nothing
		}break;

	}


}

void RunGraph_SetFFParamsInfo(SPete_Node* pNode,void* pCookie) {

	SPete_PluginParameter* pParamsInfo=(SPete_PluginParameter*)
		(pCookie);

	switch (pNode->m_eType) {

		case eType_Parameter_External: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			const int nFFIndex=pParamNode->m_nExternalIndex;

			strcpy(pParamsInfo[nFFIndex].m_pName,pParamNode->m_pExternalLabel);
			pParamsInfo[nFFIndex].m_Default=pParamNode->m_ExternalDefault;
			pParamsInfo[nFFIndex].m_ExternalType=pParamNode->m_ExternalType;

		}break;

		default: {
			// do nothing
		}break;

	}

}

void RunGraph_RunPluginNode(SPete_Node* pNode,SPete_RunTimeEnvironment* pEnv) {

	SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

	bool bIsBypassed;

	SPete_ParameterNode* pBypassParam=pEffectNode->m_ppParameters[0];
	if (pBypassParam==NULL) {
		bIsBypassed=false;
	} else {
		bIsBypassed=(pBypassParam->m_OutputValue!=0.0f);
	}

	const int nInputCount=GraphUtil_GetInputCount(pNode);

	if (!bIsBypassed) {

		FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

		ProcessFrameCopyStruct PFCData;

		const int nMaxInputFrames=64;
		U32* ppInputFrames[nMaxInputFrames];

		int nCount;
		for (nCount=0; nCount<nInputCount; nCount+=1) {

			SPete_EffectNode* pInputNode=pEffectNode->m_ppInputs[nCount];

			U32* pInputData;
			if (pInputNode==NULL) {

				pInputData=pEnv->m_pBlankInput;

			} else {

				pInputData=pInputNode->m_pOutput;
				if (pInputData==NULL) {
					pInputData=pEnv->m_pBlankInput;
				}

			}

			ppInputFrames[nCount]=pInputData;

		}

		PFCData.numInputFrames=nInputCount;
		PFCData.InputFrames=(void**)ppInputFrames;
		PFCData.OutputFrame=pEffectNode->m_pInternalOutputBuffer;
		if (PFCData.OutputFrame==NULL) {
			PFCData.OutputFrame=pEnv->m_pBlankInput;
		}

		RunGraph_SetFFParams(pEffectNode);

		(*pMainFunc)(FF_PROCESSFRAMECOPY,(void*)(&PFCData),pEffectNode->m_InstanceCookie);

		U32* pSwapTemp=pEffectNode->m_pInternalOutputBuffer;
		pEffectNode->m_pInternalOutputBuffer=pEffectNode->m_pExternalOutputBuffer;
		pEffectNode->m_pExternalOutputBuffer=pSwapTemp;

		pEffectNode->m_pOutput=pEffectNode->m_pExternalOutputBuffer;

	} else {

		const int nScreenByteCount=(sizeof(U32)*pEnv->m_nVideoWidth*pEnv->m_nVideoHeight);

		if (nInputCount>0) {

			SPete_EffectNode* pInputNode=pEffectNode->m_ppInputs[0];

			if (pInputNode==NULL) {
				pEffectNode->m_pOutput=pEnv->m_pBlankInput;
			} else {
				pEffectNode->m_pOutput=pInputNode->m_pOutput;
			}

		} else {

			pEffectNode->m_pOutput=pEnv->m_pBlankInput;

		}

	}

}

void RunGraph_SetFFParams(SPete_EffectNode* pEffectNode) {

	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

	const int nParamCount=GraphUtil_GetParameterCount(pEffectNode);
	const int nFFParamCount=(nParamCount-1);

	FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

	int nCount;
	for (nCount=0; nCount<nFFParamCount; nCount+=1) {

		const int nParamIndex=(nCount+1);

		float ParamValue;

		SPete_ParameterNode* pInputParam=pEffectNode->m_ppParameters[nParamIndex];
		if (pInputParam==NULL) {
			ParamValue=RunGraph_GetFFParamDefault(pEffectNode,nCount);
		} else {
			ParamValue=pInputParam->m_OutputValue;
		}

		RunGraph_SetFFParam(pEffectNode,nCount,ParamValue);

	}

}

float RunGraph_GetFFParamDefault(SPete_EffectNode* pEffectNode,int nIndex) {

	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

	FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

	static void* pCastIndex;
	pCastIndex=reinterpret_cast<void*>(nIndex);

	static U32 ResultAsU32;
	ResultAsU32=(U32)(*pMainFunc)(FF_GETPARAMETERDEFAULT,pCastIndex,0);
	const float DefaultValue=*((float*)&ResultAsU32);

	return DefaultValue;

}

void RunGraph_SetFFParam(SPete_EffectNode* pEffectNode,int nIndex,float Value) {

	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

	FF_Main_FuncPtr pMainFunc=pPluginInfo->m_pMainFunc;

	SetParameterStruct ArgStruct;
	ArgStruct.index=nIndex;
	ArgStruct.value=Value;

	(*pMainFunc)(FF_SETPARAMETER,&ArgStruct,pEffectNode->m_InstanceCookie);

}

void RunGraph_RunParametersOnly(SPete_Node* pNode,void* pCookie) {

	SPete_RunTimeEnvironment* pEnv=(SPete_RunTimeEnvironment*)(pCookie);

	const int nScreenByteCount=
		(pEnv->m_nVideoWidth*pEnv->m_nVideoHeight)*
		sizeof(U32);

	switch (pNode->m_eType) {

		case eType_Parameter_External: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			const int nFFIndex=pParamNode->m_nExternalIndex;

			pParamNode->m_OutputValue=pEnv->m_pFFParams[nFFIndex];

		}break;

		case eType_Parameter_Constant: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		case eType_Parameter_Keyboard: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		case eType_Parameter_Midi: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			pParamNode->m_OutputValue=0.0f;

		}break;

		default: {
			// do nothing
		}break;

	}


}

void RunGraph_RunEffectsOnly(SPete_Node* pNode,void* pCookie) {

	SPete_RunTimeEnvironment* pEnv=(SPete_RunTimeEnvironment*)(pCookie);

	const int nScreenByteCount=
		(pEnv->m_nVideoWidth*pEnv->m_nVideoHeight)*
		sizeof(U32);

	switch (pNode->m_eType) {

		case eType_Effect_Output: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			U32* pInput=pEffectNode->m_ppInputs[0]->m_pOutput;
			U32* pOutput=pEnv->m_pOutput;
			memcpy(pOutput,pInput,nScreenByteCount);

		}break;

		case eType_Effect_Input: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			int nInputIndex=pEffectNode->m_nInputIndex;
			const int nInputTotal=pEnv->m_nInputsCount;

			if (nInputIndex>=(nInputTotal)) {
				nInputIndex=(nInputTotal-1);
			}

			pEffectNode->m_pOutput=pEnv->m_ppInputs[nInputIndex];

		}break;

		case eType_Effect_Plugin: {

			RunGraph_RunPluginNode(pNode,pEnv);

		}break;

		case eType_Effect_Switcher: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			const int nInputCount=pEffectNode->m_nSwitcherInputCount;

			SPete_ParameterNode* pSelectorNode=pEffectNode->m_ppParameters[0];

			float ParamValue;
			if (pSelectorNode==NULL) {
				ParamValue=0.0f;
			} else {
				ParamValue=pSelectorNode->m_OutputValue;
			}

			const int nCurrentInput=(int)((ParamValue*(nInputCount-1))+0.5f);

			SPete_EffectNode* pInputNode=pEffectNode->m_ppInputs[nCurrentInput];
			if (pInputNode==NULL) {
				pEffectNode->m_pOutput=pEnv->m_pBlankInput;
			} else {
				pEffectNode->m_pOutput=pInputNode->m_pOutput;
			}

		}break;

		default: {
			// do nothing
		}break;

	}

}
