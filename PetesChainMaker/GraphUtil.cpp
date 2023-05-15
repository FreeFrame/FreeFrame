#include "GraphUtil.h"

#include <stdio.h>

#define PETE_PLUGINS_MAX (128)

void GraphUtil_InitPluginInfo(SPete_GraphData* pGraph);
bool GraphUtil_LoadPlugin(SPete_GraphData* pGraph,SPete_PluginInfo* pResult,char* pFileName);
void GraphUtil_GetPluginName(FF_Main_FuncPtr pFreeFrameMain,char* poutName);
bool GraphUtil_MakeListEntry(SPete_GraphData* pGraph,SPete_Node* pNode);
void GraphUtil_FreeNode(SPete_Node* pNode);
void GraphUtil_FreePluginNode(SPete_Node* pNode);
void GraphUtil_FreeInputNode(SPete_Node* pNode);
void GraphUtil_FreeOutputNode(SPete_Node* pNode);
void GraphUtil_FreeSwitcherNode(SPete_Node* pNode);
void GraphUtil_FreeExternalParamNode(SPete_Node* pNode);
void GraphUtil_FreeConstantParamNode(SPete_Node* pNode);
void GraphUtil_FreeKeyboardParamNode(SPete_Node* pNode);
void GraphUtil_FreeMidiParamNode(SPete_Node* pNode);
int GraphUtil_CalcFlattenedDataSize(SPete_GraphData* pGraph);
int GraphUtil_CalcFlattenedDLLSize(SPete_PluginInfo* pPluginInfo);
int GraphUtil_CalcFlattenedNodeSize(SPete_Node* pNode);
void GraphUtil_FlattenDLL(SPete_PluginInfo* pPluginInfo,char* pCurrentData);
int GraphUtil_FlattenNode(SPete_Node* pNode,SPete_GraphData* pGraph,char* pCurrentData);
U32 GraphUtil_GetNodeOffset(SPete_Node* pNode,SPete_NodeListEntry* pNodeListHead);
int GraphUtil_UnFlattenNode(SPete_Node* pNode,SPete_GraphData* pGraph,char* pCurrentData);
void GraphUtil_DoIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie,U32 CurrentVisitTime);
void GraphUtil_DoPrunedIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie,U32 CurrentVisitTime);
bool GraphUtil_DoLoadFlattenedDLL(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo);

void GraphUtil_Init(SPete_GraphData* pGraph) {

	Pete_ZeroMemory(pGraph,sizeof(*pGraph));
	pGraph->m_nStructSize=sizeof(*pGraph);

	GraphUtil_InitPluginInfo(pGraph);

	int nEffectPosX=400;
	int nEffectPosY=50;
	const int nEffectPosIncY=100;

#if 0

	SPete_Node* pCurrentNode=GraphUtil_MakeInputNode(pGraph,0,nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pInputEffect0=(SPete_EffectNode*)
		(pCurrentNode);


	pCurrentNode=GraphUtil_MakePluginNode(pGraph,&pGraph->m_pPluginInfo[0],nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pPluginEffect0=(SPete_EffectNode*)
		(pCurrentNode);


	pCurrentNode=GraphUtil_MakePluginNode(pGraph,&pGraph->m_pPluginInfo[1],nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pPluginEffect1=(SPete_EffectNode*)
		(pCurrentNode);


	pCurrentNode=GraphUtil_MakePluginNode(pGraph,&pGraph->m_pPluginInfo[2],nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pPluginEffect2=(SPete_EffectNode*)
		(pCurrentNode);


	pCurrentNode=GraphUtil_MakePluginNode(pGraph,&pGraph->m_pPluginInfo[3],nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pPluginEffect3=(SPete_EffectNode*)
		(pCurrentNode);


	pCurrentNode=GraphUtil_MakeOutputNode(pGraph,nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	pGraph->m_pRootNode=pCurrentNode;

	SPete_EffectNode* pOutputEffect=(SPete_EffectNode*)
		(pCurrentNode);

	int nParamPosX=100;
	int nParamPosY=50;
	const int nParamPosIncY=35;

	GraphUtil_AttachInput(pPluginEffect0,0,pInputEffect0);

	const int nEffect0ParamCount=
		GraphUtil_GetParameterCount(pPluginEffect0);

	int nCurrentExternalIndex=0;

	int nCount;
	for (nCount=0; nCount<nEffect0ParamCount; nCount+=1) {

		SPete_PluginParameter ParamInfo;
		GraphUtil_GetEffectParamInfo(pPluginEffect0,nCount,&ParamInfo);

		pCurrentNode=
			GraphUtil_MakeExternalParamNode(pGraph,nCurrentExternalIndex,ParamInfo.m_pName,ParamInfo.m_Default,nParamPosX,nParamPosY);
		nParamPosY+=nParamPosIncY;
		nCurrentExternalIndex+=1;

		SPete_ParameterNode* pCurrentParam=(SPete_ParameterNode*)
			(pCurrentNode);

		GraphUtil_AttachParameter(pPluginEffect0,nCount,pCurrentParam);

	}

	GraphUtil_AttachInput(pPluginEffect1,0,pPluginEffect0);

	const int nEffect1ParamCount=
		GraphUtil_GetParameterCount(pPluginEffect1);

	for (nCount=0; nCount<nEffect1ParamCount; nCount+=1) {

		SPete_PluginParameter ParamInfo;
		GraphUtil_GetEffectParamInfo(pPluginEffect1,nCount,&ParamInfo);

		pCurrentNode=
			GraphUtil_MakeExternalParamNode(pGraph,nCurrentExternalIndex,ParamInfo.m_pName,ParamInfo.m_Default,nParamPosX,nParamPosY);
		nParamPosY+=nParamPosIncY;
		nCurrentExternalIndex+=1;

		SPete_ParameterNode* pCurrentParam=(SPete_ParameterNode*)
			(pCurrentNode);

		GraphUtil_AttachParameter(pPluginEffect1,nCount,pCurrentParam);

	}


	GraphUtil_AttachInput(pPluginEffect2,0,pPluginEffect1);

	const int nEffect2ParamCount=
		GraphUtil_GetParameterCount(pPluginEffect2);

	for (nCount=0; nCount<nEffect2ParamCount; nCount+=1) {

		SPete_PluginParameter ParamInfo;
		GraphUtil_GetEffectParamInfo(pPluginEffect2,nCount,&ParamInfo);

		pCurrentNode=
			GraphUtil_MakeExternalParamNode(pGraph,nCurrentExternalIndex,ParamInfo.m_pName,ParamInfo.m_Default,nParamPosX,nParamPosY);
		nParamPosY+=nParamPosIncY;
		nCurrentExternalIndex+=1;

		SPete_ParameterNode* pCurrentParam=(SPete_ParameterNode*)
			(pCurrentNode);

		GraphUtil_AttachParameter(pPluginEffect2,nCount,pCurrentParam);

	}

	GraphUtil_AttachInput(pPluginEffect3,0,pPluginEffect2);

	const int nEffect3ParamCount=
		GraphUtil_GetParameterCount(pPluginEffect3);

	for (nCount=0; nCount<nEffect3ParamCount; nCount+=1) {

		SPete_PluginParameter ParamInfo;
		GraphUtil_GetEffectParamInfo(pPluginEffect3,nCount,&ParamInfo);

		pCurrentNode=
			GraphUtil_MakeExternalParamNode(pGraph,nCurrentExternalIndex,ParamInfo.m_pName,ParamInfo.m_Default,nParamPosX,nParamPosY);
		nParamPosY+=nParamPosIncY;
		nCurrentExternalIndex+=1;

		SPete_ParameterNode* pCurrentParam=(SPete_ParameterNode*)
			(pCurrentNode);

		GraphUtil_AttachParameter(pPluginEffect3,nCount,pCurrentParam);

	}


	pOutputEffect->m_ppInputs[0]=pPluginEffect3;

#else

	SPete_Node* pCurrentNode=GraphUtil_MakeInputNode(pGraph,0,nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	SPete_EffectNode* pInputEffect0=(SPete_EffectNode*)
		(pCurrentNode);

	pCurrentNode=GraphUtil_MakeOutputNode(pGraph,nEffectPosX,nEffectPosY);
	nEffectPosY+=nEffectPosIncY;

	pGraph->m_pRootNode=pCurrentNode;

	SPete_EffectNode* pOutputEffect=(SPete_EffectNode*)
		(pCurrentNode);

	int nParamPosX=100;
	int nParamPosY=50;
	const int nParamPosIncY=35;

	GraphUtil_AttachInput(pOutputEffect,0,pInputEffect0);

#endif

	pGraph->m_bIsValid=true;

	strcpy(pGraph->m_pName,"FreeChain Plugin");

	pGraph->m_pUniqueID[0]='F';
	pGraph->m_pUniqueID[1]='C';
	pGraph->m_pUniqueID[2]='H';
	pGraph->m_pUniqueID[3]='N';
	pGraph->m_pUniqueID[4]='\0';

	strcpy(pGraph->m_pDescription,"No description written");

	pGraph->m_bStorePluginCopies=true;

}

void GraphUtil_InitFromFlat(SPete_GraphData* pGraph,char* pFlatData,int nFlatDataSize) {

	Pete_ZeroMemory(pGraph,sizeof(*pGraph));

	char* pDataStart=pFlatData;
	char* pDataEnd=(pFlatData+nFlatDataSize);

	char* pCurrentData=pDataStart;

	SPete_GraphData* pFlatGraph=(SPete_GraphData*)(pCurrentData);

	*pGraph=*pFlatGraph;
	pGraph->m_bIsValid=false;

	pGraph->m_nFlattenedDataSize=nFlatDataSize;
	pGraph->m_nPluginInfoCount=pFlatGraph->m_nPluginInfoCount;
	pGraph->m_pRootNode=(SPete_Node*)
		(pDataStart+((U32)(pFlatGraph->m_pRootNode)));
	pGraph->m_pFlattenedData=pFlatData;
	pGraph->m_pNodeListHead=NULL;
	pGraph->m_pPluginInfo=(SPete_PluginInfo*)
		(pDataStart+((U32)(pFlatGraph->m_pPluginInfo)));
	pGraph->m_PluginInfoAllocType=eAlloc_Flattened;

	pCurrentData+=pGraph->m_nStructSize;

	const int nPluginInfoCount=pGraph->m_nPluginInfoCount;

	SPete_PluginInfo* pPluginInfo=pGraph->m_pPluginInfo;

	pCurrentData+=(sizeof(pPluginInfo[0]))*nPluginInfoCount;

	int nCount;
	for (nCount=0; nCount<nPluginInfoCount; nCount+=1) {

		SPete_PluginInfo* pCurrentInfo=
			&pPluginInfo[nCount];

		pCurrentData+=pCurrentInfo->m_FlattenedDLLImageSize;

	}

	while (pCurrentData<pDataEnd) {

		SPete_Node* pNode=(SPete_Node*)(pCurrentData);

		const int nNodeSize=
			GraphUtil_UnFlattenNode(pNode,pGraph,pCurrentData);

		pCurrentData+=nNodeSize;

	}

	pGraph->m_bIsValid=true;

}

void GraphUtil_DeInit(SPete_GraphData* pGraph) {

	pGraph->m_bIsValid=false;

	if (pGraph->m_PluginInfoAllocType==eAlloc_Heap) {
		if (pGraph->m_pPluginInfo!=NULL) {
			free(pGraph->m_pPluginInfo);
		}
		pGraph->m_pPluginInfo=NULL;
	}

	SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;

	while (pCurrentEntry!=NULL) {

		SPete_NodeListEntry* pNext=pCurrentEntry->m_pNext;

		GraphUtil_FreeNode(pCurrentEntry->m_pNode);

		pCurrentEntry->m_pNode=NULL;
		pCurrentEntry->m_pNext=NULL;
		free(pCurrentEntry);

		pCurrentEntry=pNext;

	}

	pGraph->m_pNodeListHead=NULL;

	if (pGraph->m_pFlattenedData!=NULL) {
		free(pGraph->m_pFlattenedData);
		pGraph->m_pFlattenedData=NULL;
	}

	Pete_ZeroMemory(pGraph,sizeof(*pGraph));

}


bool GraphUtil_DoLoadPlugin(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo) {

	bool bLoadSucceeded=false;

	if ((pPluginInfo->m_pMainFunc!=NULL)||
		(pPluginInfo->m_nReferenceCount>0)){
		pPluginInfo->m_nReferenceCount+=1;
		return true;
	}

	pPluginInfo->m_nReferenceCount=1;

	pPluginInfo->m_hModuleHandle=LoadLibrary(pPluginInfo->m_pDLLPath);

	if (pPluginInfo->m_hModuleHandle!=NULL) {

		FF_Main_FuncPtr pFreeFrameMain=
			(FF_Main_FuncPtr)
			GetProcAddress(
			pPluginInfo->m_hModuleHandle,
			"plugMain");

		if (pFreeFrameMain!=NULL) {
			pPluginInfo->m_pMainFunc=pFreeFrameMain;
			bLoadSucceeded=true;
		} else {
			FreeLibrary(pPluginInfo->m_hModuleHandle);
		}

	} else {

		const bool bFlatDLLAvailable=
			((pGraph->m_nFlattenedDataSize>0)&&
			(pPluginInfo->m_FlattenedDLLImageSize>0)&&
			(pPluginInfo->m_FlattenedDLLImageOffset>0));

		if (bFlatDLLAvailable) {

			bLoadSucceeded=
				GraphUtil_DoLoadFlattenedDLL(pGraph,pPluginInfo);

		} else {

			bLoadSucceeded=false;

		}

	}

	return bLoadSucceeded;

}

void GraphUtil_DoUnLoadPlugin(SPete_PluginInfo* pPluginInfo) {

	pPluginInfo->m_nReferenceCount-=1;

	if (pPluginInfo->m_nReferenceCount<=0) {
		pPluginInfo->m_nReferenceCount=0;

		if (pPluginInfo->m_pMainFunc!=NULL) {

			pPluginInfo->m_pMainFunc=NULL;
			FreeLibrary(pPluginInfo->m_hModuleHandle);

		}

	}

}

void GraphUtil_FlattenGraph(SPete_GraphData* pGraph) {

	const int nFlattenedDataSize=
		GraphUtil_CalcFlattenedDataSize(pGraph);

	pGraph->m_pFlattenedData=(char*)
		malloc(nFlattenedDataSize);

	if (pGraph->m_pFlattenedData==NULL) {
		MessageBox(NULL,"Couldn't allocate flattened data buffer","",MB_OK);
		return;
	}

	pGraph->m_nFlattenedDataSize=nFlattenedDataSize;

	char* pDataStart=pGraph->m_pFlattenedData;

	char* pCurrentData=pDataStart;

	SPete_GraphData* pFlatGraph=(SPete_GraphData*)(pCurrentData);

	*pFlatGraph=*pGraph;

	pFlatGraph->m_nFlattenedDataSize=pGraph->m_nFlattenedDataSize;
	pFlatGraph->m_nPluginInfoCount=pGraph->m_nPluginInfoCount;
	pFlatGraph->m_pRootNode=(SPete_Node*)
		GraphUtil_GetNodeOffset(pGraph->m_pRootNode,pGraph->m_pNodeListHead);
	pFlatGraph->m_pFlattenedData=(char*)(pGraph->m_pFlattenedData-pDataStart);
	pFlatGraph->m_pNodeListHead=NULL;
	pFlatGraph->m_pPluginInfo=(SPete_PluginInfo*)
		((pCurrentData-pDataStart)+sizeof(SPete_GraphData));

	pCurrentData+=sizeof(SPete_GraphData);

	const int nPluginInfoCount=pGraph->m_nPluginInfoCount;

	SPete_PluginInfo* pPluginInfo=pGraph->m_pPluginInfo;
	SPete_PluginInfo* pFlatPluginInfo=
		(SPete_PluginInfo*)
		(pCurrentData);

	int nCount;
	for (nCount=0; nCount<nPluginInfoCount; nCount+=1) {

		SPete_PluginInfo* pCurrentInfo=
			&pPluginInfo[nCount];

		SPete_PluginInfo* pFlatInfo=
			(SPete_PluginInfo*)(pCurrentData);

		pFlatInfo->m_FlattenedDLLImageOffset=0;

		pFlatInfo->m_FlattenedDLLImageSize=
			pCurrentInfo->m_FlattenedDLLImageSize;

		pFlatInfo->m_hModuleHandle=NULL;
		pFlatInfo->m_nInputCount=pCurrentInfo->m_nInputCount;
		pFlatInfo->m_nParameterCount=pCurrentInfo->m_nParameterCount;
		pFlatInfo->m_nReferenceCount=0;
		strcpy(pFlatInfo->m_pDLLPath,pCurrentInfo->m_pDLLPath);
		pFlatInfo->m_pMainFunc=NULL;
		strcpy(pFlatInfo->m_pName,pCurrentInfo->m_pName);
		pFlatInfo->m_FlattenedInfoOffset=pCurrentInfo->m_FlattenedInfoOffset;

		const int nParamTotal=pCurrentInfo->m_nParameterCount;

		int nParamIndex;
		for (nParamIndex=0; nParamIndex<nParamTotal; nParamIndex+=1) {

			pFlatInfo->m_pParameterInfo[nParamIndex]=
				pCurrentInfo->m_pParameterInfo[nParamIndex];

		}

		pCurrentData+=sizeof(SPete_PluginInfo);

	}

	for (nCount=0; nCount<nPluginInfoCount; nCount+=1) {

		SPete_PluginInfo* pCurrentInfo=
			&pFlatPluginInfo[nCount];

		pCurrentInfo->m_FlattenedDLLImageOffset=
			(pCurrentData-pDataStart);

		GraphUtil_FlattenDLL(pCurrentInfo,pCurrentData);

		pCurrentData+=pCurrentInfo->m_FlattenedDLLImageSize;

	}

	SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;

	while (pCurrentEntry!=NULL) {

		SPete_Node* pNode=pCurrentEntry->m_pNode;

		const int nNodeSize=
			GraphUtil_FlattenNode(pNode,pGraph,pCurrentData);

		pCurrentData+=nNodeSize;

		pCurrentEntry=pCurrentEntry->m_pNext;

	}

}

void GraphUtil_AttachInput(SPete_EffectNode* pEffectNode,int nIndex,SPete_EffectNode* pInputNode) {

	const int nInputCount=GraphUtil_GetInputCount(pEffectNode);
	assert(nIndex<nInputCount);

	pEffectNode->m_ppInputs[nIndex]=pInputNode;

}

void GraphUtil_AttachParameter(SPete_Node* pNode,int nIndex,SPete_ParameterNode* pInputNode) {

	const int nParamCount=GraphUtil_GetParameterCount(pNode);
	assert(nIndex<nParamCount);

	pNode->m_ppParameters[nIndex]=pInputNode;

}

SPete_Node* GraphUtil_MakeOutputNode(SPete_GraphData* pGraph,int nPosX,int nPosY) {

	SPete_EffectNode* pResult=(SPete_EffectNode*)
		malloc(sizeof(SPete_EffectNode));

	if (pResult==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	pResult->m_eType=eType_Effect_Output;
	pResult->m_Size.nX=50;
	pResult->m_Size.nY=40;
	pResult->m_Pos.nX=nPosX;
	pResult->m_Pos.nY=nPosY;
	pResult->m_AllocationType=eAlloc_Heap;

	const int nInputCount=1;
	const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

	pResult->m_ppInputs=(SPete_EffectNode**)
		malloc(nInputByteCount);
	if (pResult->m_ppInputs==NULL) {
		return NULL;
	}
	Pete_ZeroMemory(pResult->m_ppInputs,nInputByteCount);

	const bool bMakeListEntryResult=
		GraphUtil_MakeListEntry(pGraph,pResult);

	if (bMakeListEntryResult==false) {
		return NULL;
	}

	return pResult;

}

SPete_Node* GraphUtil_MakePluginNode(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo,int nPosX,int nPosY) {

	SPete_EffectNode* pResult=(SPete_EffectNode*)
		malloc(sizeof(SPete_EffectNode));

	if (pResult==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	pResult->m_eType=eType_Effect_Plugin;
	pResult->m_pPluginInfo=pPluginInfo;
	pResult->m_Size.nX=80;
	pResult->m_Size.nY=40;
	pResult->m_Pos.nX=nPosX;
	pResult->m_Pos.nY=nPosY;
	pResult->m_AllocationType=eAlloc_Heap;

	const int nInputCount=GraphUtil_GetInputCount(pResult);
	const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

	pResult->m_ppInputs=(SPete_EffectNode**)
		malloc(nInputByteCount);
	if (pResult->m_ppInputs==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult->m_ppInputs,nInputByteCount);

	const int nParameterCount=GraphUtil_GetParameterCount(pResult);
	const int nParameterByteCount=
		(nParameterCount*sizeof(SPete_ParameterNode*));

	pResult->m_ppParameters=(SPete_ParameterNode**)
		malloc(nParameterByteCount);
	if (pResult->m_ppParameters==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult->m_ppParameters,nParameterByteCount);

	const bool bMakeListEntryResult=
		GraphUtil_MakeListEntry(pGraph,pResult);

	if (bMakeListEntryResult==false) {
		return NULL;
	}

	return pResult;

}

SPete_Node* GraphUtil_MakeInputNode(SPete_GraphData* pGraph,int nInputIndex,int nPosX,int nPosY) {

	SPete_EffectNode* pResult=(SPete_EffectNode*)
		malloc(sizeof(SPete_EffectNode));

	if (pResult==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	pResult->m_eType=eType_Effect_Input;
	pResult->m_Size.nX=50;
	pResult->m_Size.nY=40;
	pResult->m_Pos.nX=nPosX;
	pResult->m_Pos.nY=nPosY;
	pResult->m_AllocationType=eAlloc_Heap;

	pResult->m_nInputIndex=nInputIndex;

	const bool bMakeListEntryResult=
		GraphUtil_MakeListEntry(pGraph,pResult);

	if (bMakeListEntryResult==false) {
		return NULL;
	}

	return pResult;

}

SPete_Node* GraphUtil_MakeSwitcherNode(SPete_GraphData* pGraph,int nSwitcherInputCount,int nPosX,int nPosY) {

	SPete_EffectNode* pResult=(SPete_EffectNode*)
		malloc(sizeof(SPete_EffectNode));

	if (pResult==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	pResult->m_eType=eType_Effect_Switcher;
	pResult->m_Size.nX=50;
	pResult->m_Size.nY=40;
	pResult->m_Pos.nX=nPosX;
	pResult->m_Pos.nY=nPosY;
	pResult->m_AllocationType=eAlloc_Heap;

	pResult->m_nSwitcherInputCount=nSwitcherInputCount;

	const int nInputCount=PETE_MAX_SWITCHER_INPUTS;
	const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

	pResult->m_ppInputs=(SPete_EffectNode**)
		malloc(nInputByteCount);
	if (pResult->m_ppInputs==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult->m_ppInputs,nInputByteCount);

	const int nParameterCount=GraphUtil_GetParameterCount(pResult);
	const int nParameterByteCount=
		(nParameterCount*sizeof(SPete_ParameterNode*));

	pResult->m_ppParameters=(SPete_ParameterNode**)
		malloc(nParameterByteCount);
	if (pResult->m_ppParameters==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult->m_ppParameters,nParameterByteCount);

	const bool bMakeListEntryResult=
		GraphUtil_MakeListEntry(pGraph,pResult);

	if (bMakeListEntryResult==false) {
		return NULL;
	}

	return pResult;

}

SPete_Node* GraphUtil_MakeExternalParamNode(SPete_GraphData* pGraph,int nIndex,char* pName,float Default,U32 Type,int nPosX,int nPosY) {

	SPete_ParameterNode* pResult=(SPete_ParameterNode*)
		malloc(sizeof(SPete_ParameterNode));

	if (pResult==NULL) {
		return NULL;
	}

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	pResult->m_eType=eType_Parameter_External;
	pResult->m_Size.nX=50;
	pResult->m_Size.nY=16;
	pResult->m_Pos.nX=nPosX;
	pResult->m_Pos.nY=nPosY;
	pResult->m_AllocationType=eAlloc_Heap;

	pResult->m_OutputValue=0.0f;

	strncpy(pResult->m_pExternalLabel,pName,15);
	pResult->m_pExternalLabel[15]='\0';

	pResult->m_ExternalDefault=Default;

	pResult->m_nExternalIndex=nIndex;

	pResult->m_ExternalType=Type;

	const bool bMakeListEntryResult=
		GraphUtil_MakeListEntry(pGraph,pResult);

	if (bMakeListEntryResult==false) {
		return NULL;
	}

	return pResult;

}

int GraphUtil_GetInputCount(SPete_Node* pNode) {

	int nResult;

	switch (pNode->m_eType) {

		case eType_Effect_Output: {
			nResult=1;
		}break;

		case eType_Effect_Input: {
			nResult=0;
		}break;

		case eType_Effect_Plugin: {
			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);
			SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;
			nResult=pPluginInfo->m_nInputCount;
		}break;

		case eType_Effect_Switcher: {
			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);
			nResult=pEffectNode->m_nSwitcherInputCount;
		}break;

		default: {
			nResult=0;
		}break;

	}

	return nResult;

}

int GraphUtil_GetParameterCount(SPete_Node* pNode) {

	int nResult;

	switch (pNode->m_eType) {

		case eType_Effect_Output: {
			nResult=0;
		}break;

		case eType_Effect_Input: {
			nResult=0;
		}break;

		case eType_Effect_Plugin: {
			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);
			SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;
			nResult=(pPluginInfo->m_nParameterCount+1);
		}break;

		case eType_Effect_Switcher: {
			nResult=1;
		}break;

		default: {
			nResult=0;
		}break;

	}

	return nResult;

}

void GraphUtil_GetEffectParamInfo(SPete_Node* pNode,int nIndex,SPete_PluginParameter* poutParamInfo) {

	if (pNode->m_eType!=eType_Effect_Plugin) {
		return;
	}

	SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);
	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

	if (nIndex==0) {
		strcpy(poutParamInfo->m_pName,"Bypass Effect");
		poutParamInfo->m_Default=0.0f;
	} else {

		nIndex-=1;
		SPete_PluginParameter* pPluginParam=
			(&pPluginInfo->m_pParameterInfo[nIndex]);

		*poutParamInfo=*pPluginParam;

	}

}

SPete_NodeListEntry* GraphUtil_FindListEntry(SPete_GraphData* pGraph,SPete_Node* pNode) {

	SPete_NodeListEntry* pCurrent=pGraph->m_pNodeListHead;

	while (pCurrent!=NULL) {

		if (pCurrent->m_pNode==pNode) {

			return pCurrent;

		}

		pCurrent=pCurrent->m_pNext;

	}

	return NULL;

}

SPete_PluginInfo* GraphUtil_AddPluginInfo(SPete_GraphData* pGraph,char* pFileName) {

	const int nPluginCount=pGraph->m_nPluginInfoCount;

	SPete_PluginInfo* pPluginInfo=pGraph->m_pPluginInfo;

	int nCount;
	for (nCount=0; nCount<nPluginCount; nCount+=1) {

		SPete_PluginInfo* pCurrentPlugin=&pPluginInfo[nCount];

		if (stricmp(pFileName,pCurrentPlugin->m_pDLLPath)==0) {

			return pCurrentPlugin;

		}


	}

	if (nPluginCount>=(PETE_PLUGINS_MAX-1)) {

		return NULL;

	}

	const int nNewPluginCount=nPluginCount+1;
	SPete_PluginInfo* pNewPluginInfo=(SPete_PluginInfo*)
		(malloc(sizeof(SPete_PluginInfo)*nNewPluginCount));

	memcpy(pNewPluginInfo,pPluginInfo,sizeof(SPete_PluginInfo)*nPluginCount);

	SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;
	while (pCurrentEntry!=NULL) {

		SPete_Node* pNode=pCurrentEntry->m_pNode;
		if (pNode->m_eType==eType_Effect_Plugin) {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			SPete_PluginInfo* pOldPluginPointer=pEffectNode->m_pPluginInfo;
			const int nPluginInfoOffset=(pOldPluginPointer-pPluginInfo);
			pEffectNode->m_pPluginInfo=(pNewPluginInfo+nPluginInfoOffset);

		}

		pCurrentEntry=pCurrentEntry->m_pNext;

	}

	pGraph->m_pPluginInfo=pNewPluginInfo;
	pPluginInfo=pNewPluginInfo;

	SPete_PluginInfo* pNewPlugin=&pPluginInfo[nPluginCount];

	bool bLoadPluginResult=GraphUtil_LoadPlugin(pGraph,pNewPlugin,pFileName);

	if (bLoadPluginResult==false) {

		return NULL;

	}

	pGraph->m_nPluginInfoCount=nNewPluginCount;

	return pNewPlugin;

}

void GraphUtil_Iterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie) {

	U32 CurrentTime=(pNode->m_LastVisitTime+1);

	GraphUtil_DoIterate(pNode,pfnAction,pCookie,CurrentTime);

}

void GraphUtil_FindHighestFFParameter(SPete_Node* pNode,void* pCookie) {

	int* pCurrentHighest=(int*)(pCookie);

	switch (pNode->m_eType) {

		case eType_Parameter_External: {

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)(pNode);

			const int nFFIndex=pParamNode->m_nExternalIndex;

			if (nFFIndex>*pCurrentHighest) {
				*pCurrentHighest=nFFIndex;
			}

		}break;

		default: {
			// do nothing
		}break;

	}

}

bool GraphUtil_HasParameterOutput(SPete_Node* pNode) {

	bool bResult;

	switch (pNode->m_eType) {

		case eType_Effect_Output:
		case eType_Effect_Input:
		case eType_Effect_Plugin:
		case eType_Effect_Switcher: {

			bResult=false;

		}break;

		case eType_Parameter_External:
		case eType_Parameter_Constant:
		case eType_Parameter_Keyboard:
		case eType_Parameter_Midi: {

			bResult=true;

		}break;

		default: {
			bResult=false;
		}break;

	}

	return bResult;

}

bool GraphUtil_HasImageOutput(SPete_Node* pNode) {

	bool bResult;

	switch (pNode->m_eType) {

		case eType_Effect_Output: {
			bResult=false;
		}break;

		case eType_Effect_Input:
		case eType_Effect_Plugin:
		case eType_Effect_Switcher: {

			bResult=true;

		}break;

		case eType_Parameter_External:
		case eType_Parameter_Constant:
		case eType_Parameter_Keyboard:
		case eType_Parameter_Midi: {

			bResult=false;

		}break;

		default: {
			bResult=false;
		}break;

	}

	return bResult;

}

void GraphUtil_PrunedIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie) {

	U32 CurrentTime=(pNode->m_LastVisitTime+1);

	GraphUtil_DoPrunedIterate(pNode,pfnAction,pCookie,CurrentTime);

}


/*---------------------------------------------------------------------------*/

void GraphUtil_InitPluginInfo(SPete_GraphData* pGraph) {

	pGraph->m_nPluginInfoCount=0;

	pGraph->m_pPluginInfo=(SPete_PluginInfo*)
		malloc(sizeof(SPete_PluginInfo)*PETE_PLUGINS_MAX);

	pGraph->m_PluginInfoAllocType=eAlloc_Heap;

	if (pGraph->m_pPluginInfo==NULL) {
		MessageBox(NULL,"Couldn't allocate plugin info buffer","",MB_OK);
		return;
	}

//	GraphUtil_LoadPlugin(&pGraph->m_pPluginInfo[0],"C:\\projects\\delphihost\\pluginsbackup\\peteburn.dll");
//
//	GraphUtil_LoadPlugin(&pGraph->m_pPluginInfo[1],"C:\\projects\\delphihost\\pluginsbackup\\petedodge.dll");
//
//	GraphUtil_LoadPlugin(&pGraph->m_pPluginInfo[2],"C:\\projects\\delphihost\\pluginsbackup\\peteexclusion.dll");
//
//	GraphUtil_LoadPlugin(&pGraph->m_pPluginInfo[3],"C:\\projects\\delphihost\\pluginsbackup\\petefisheye.dll");

}

bool GraphUtil_LoadPlugin(SPete_GraphData* pGraph,SPete_PluginInfo* pResult,char* pFileName) {

	Pete_ZeroMemory(pResult,sizeof(*pResult));

	strcpy(pResult->m_pDLLPath,pFileName);
	pResult->m_nReferenceCount=0;

	const bool bLoadSucceeded=GraphUtil_DoLoadPlugin(pGraph,pResult);

	if (!bLoadSucceeded) {
		char MessageString[1024];
		sprintf(MessageString,"Failed to load plugin DLL '%s'",pFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

	FF_Main_FuncPtr pMainFunc=pResult->m_pMainFunc;

	PlugInfoStruct* pPluginInfoData=(PlugInfoStruct*)((*pMainFunc)(FF_GETINFO,NULL,0));

	const int nPluginVersion=
		(pPluginInfoData->APIMajorVersion*1000)+
		(pPluginInfoData->APIMinorVersion*1);

	if (nPluginVersion<=500) {
		char MessageString[1024];
		sprintf(MessageString,"Plugin DLL '%s' is a 0.5 version plugin, not supported",pFileName);
		MessageBox(NULL,MessageString,"FreeChain Error",MB_OK);
		return false;
	}

	void* pInitializeResult=(*pMainFunc)(FF_INITIALISE,NULL,0);
	if (pInitializeResult==(void*)FF_FAIL) {
		char MessageString[1024];
		sprintf(MessageString,"Plugin DLL '%s' failed to initialize",pFileName);
		MessageBox(NULL,MessageString,"FreeChain Error",MB_OK);
		return false;
	}

	void* p32BitVideoResult=(*pMainFunc)(FF_GETPLUGINCAPS,(void*)FF_CAP_32BITVIDEO,0);
	if (((DWORD)p32BitVideoResult)==FF_FALSE) {
		char MessageString[1024];
		sprintf(MessageString,"Plugin DLL '%s' doesn't support 32 bit rendering, needed by FreeChain",pFileName);
		MessageBox(NULL,MessageString,"FreeChain Error",MB_OK);
		return false;
	}

	void* pParamCountResult=(*pMainFunc)(FF_GETNUMPARAMETERS,NULL,0);
	const int nParamCount=(int)(pParamCountResult);

	pResult->m_nParameterCount=nParamCount;

	void* pInputCountResult=(*pMainFunc)(FF_GETPLUGINCAPS,(void*)FF_CAP_MAXIMUMINPUTFRAMES,0);
	pResult->m_nInputCount=(int)(pInputCountResult);

	GraphUtil_GetPluginName(pMainFunc,pResult->m_pName);

	SPete_PluginParameter* pParams=
		(&pResult->m_pParameterInfo[0]);

	int nCount;
	for (nCount=0; nCount<nParamCount; nCount+=1) {

		SPete_PluginParameter* pCurrentParam=&pParams[nCount];

		char* pName=(char*)
			((*pMainFunc)(FF_GETPARAMETERNAME,(void*)(nCount),0));

		void* pGetDefaultResult=
			(*pMainFunc)(FF_GETPARAMETERDEFAULT,(void*)(nCount),0);

		float Default=*((float*)&pGetDefaultResult);

		strncpy(pCurrentParam->m_pName,pName,16);
		pCurrentParam->m_pName[15]='\0';

		pCurrentParam->m_Default=Default;

	}

	(*pMainFunc)(FF_DEINITIALISE,NULL,0);

	GraphUtil_DoUnLoadPlugin(pResult);

	return true;

}


void GraphUtil_GetPluginName(FF_Main_FuncPtr pFreeFrameMain,char* poutName) {

	strcpy(poutName,"No Name");

	if (pFreeFrameMain==NULL) {
		return;
	}

	PlugInfoStruct* pInfoStruct=(PlugInfoStruct*)pFreeFrameMain(FF_GETINFO,NULL,0);

	if (pInfoStruct==NULL) {
		return;
	}

	char* pPluginName=(char*)(&pInfoStruct->pluginName[0]);

	int nCharCount=0;
	while ((pPluginName[nCharCount]!='\0')&&(nCharCount<16)) {
		poutName[nCharCount]=pPluginName[nCharCount];
		nCharCount+=1;
	}

	poutName[nCharCount]='\0';

}

bool GraphUtil_MakeListEntry(SPete_GraphData* pGraph,SPete_Node* pNode) {

	SPete_NodeListEntry* pListEntry=(SPete_NodeListEntry*)
		(malloc(sizeof(*pListEntry)));

	if (pListEntry==NULL) {
		return false;
	}

	pListEntry->m_pNode=pNode;
	pListEntry->m_pNext=pGraph->m_pNodeListHead;

	pGraph->m_pNodeListHead=pListEntry;

	return true;

}

void GraphUtil_DeleteNode(SPete_GraphData* pGraph,SPete_Node* pNode) {

	SPete_NodeListEntry* pCurrent=pGraph->m_pNodeListHead;
	SPete_NodeListEntry* pPrevious=NULL;

	while (pCurrent!=NULL) {

		SPete_NodeListEntry* pNext=pCurrent->m_pNext;

		if (pCurrent->m_pNode==pNode) {

			if (pPrevious==NULL) {

				pGraph->m_pNodeListHead=pNext;

			} else {

				pPrevious->m_pNext=pNext;

			}

			GraphUtil_FreeNode(pCurrent->m_pNode);

			pCurrent->m_pNode=NULL;
			pCurrent->m_pNext=NULL;
			free(pCurrent);

		} else {

			SPete_Node* pOtherNode=pCurrent->m_pNode;

			const int nParamCount=GraphUtil_GetParameterCount(pOtherNode);

			int nCount;
			for (nCount=0; nCount<nParamCount; nCount+=1) {

				if (pOtherNode->m_ppParameters[nCount]==pNode) {
					pOtherNode->m_ppParameters[nCount]=NULL;
				}

			}

			const int nInputCount=GraphUtil_GetInputCount(pOtherNode);

			if (nInputCount>0) {

				SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pOtherNode);

				int nCount;
				for (nCount=0; nCount<nInputCount; nCount+=1) {

					if (pEffectNode->m_ppInputs[nCount]==pNode) {
						pEffectNode->m_ppInputs[nCount]=NULL;
					}

				}


			}


		}

		pPrevious=pCurrent;
		pCurrent=pNext;

	}


}

void GraphUtil_FreeNode(SPete_Node* pNode) {

	if (pNode->m_AllocationType!=eAlloc_Heap) {
		return;
	}

	switch(pNode->m_eType) {

		case eType_Effect_Output: {
			GraphUtil_FreeOutputNode(pNode);
		}break;

		case eType_Effect_Input: {
			GraphUtil_FreeInputNode(pNode);
		}break;

		case eType_Effect_Plugin: {
			GraphUtil_FreePluginNode(pNode);
		}break;

		case eType_Effect_Switcher: {
			GraphUtil_FreeSwitcherNode(pNode);
		}break;

		case eType_Parameter_External: {
			GraphUtil_FreeExternalParamNode(pNode);
		}break;

		case eType_Parameter_Constant: {
			GraphUtil_FreeConstantParamNode(pNode);
		}break;

		case eType_Parameter_Keyboard : {
			GraphUtil_FreeKeyboardParamNode(pNode);
		}break;

		case eType_Parameter_Midi: {
			GraphUtil_FreeMidiParamNode(pNode);
		}break;

		default: {
			// do nothing
		}break;

	}

}

void GraphUtil_FreePluginNode(SPete_Node* pNode) {

	SPete_EffectNode* pEffectNode=
		(SPete_EffectNode*)(pNode);

	free(pEffectNode->m_ppInputs);

	free(pEffectNode);

}

void GraphUtil_FreeInputNode(SPete_Node* pNode) {

	free(pNode);

}

void GraphUtil_FreeOutputNode(SPete_Node* pNode) {

	free(pNode);

}

void GraphUtil_FreeSwitcherNode(SPete_Node* pNode) {

	SPete_EffectNode* pEffectNode=
		(SPete_EffectNode*)(pNode);

	free(pEffectNode->m_ppInputs);

	free(pEffectNode);

}

void GraphUtil_FreeExternalParamNode(SPete_Node* pNode) {

	free(pNode);

}

void GraphUtil_FreeConstantParamNode(SPete_Node* pNode) {

	free(pNode);

}

void GraphUtil_FreeKeyboardParamNode(SPete_Node* pNode) {

	free(pNode);

}

void GraphUtil_FreeMidiParamNode(SPete_Node* pNode) {

	free(pNode);

}


int GraphUtil_CalcFlattenedDataSize(SPete_GraphData* pGraph) {

	int nCurrentOffset=0;

	nCurrentOffset+=sizeof(SPete_GraphData);

	const int nPluginInfoCount=pGraph->m_nPluginInfoCount;

	SPete_PluginInfo* pPluginInfo=pGraph->m_pPluginInfo;

	int nCount;
	for (nCount=0; nCount<nPluginInfoCount; nCount+=1) {

		SPete_PluginInfo* pCurrentPluginInfo=
			&pPluginInfo[nCount];

		pCurrentPluginInfo->m_FlattenedInfoOffset=nCurrentOffset;

		nCurrentOffset+=(sizeof(SPete_PluginInfo));

		const int nDLLSize=
			GraphUtil_CalcFlattenedDLLSize(&pPluginInfo[nCount]);

		pCurrentPluginInfo->m_FlattenedDLLImageSize=nDLLSize;

	}

	for (nCount=0; nCount<nPluginInfoCount; nCount+=1) {

		SPete_PluginInfo* pCurrentPluginInfo=
			&pPluginInfo[nCount];

		nCurrentOffset+=pCurrentPluginInfo->m_FlattenedDLLImageSize;

	}

	SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;

	while (pCurrentEntry!=NULL) {

		pCurrentEntry->m_FlattenedOffset=nCurrentOffset;

		nCurrentOffset+=
			GraphUtil_CalcFlattenedNodeSize(pCurrentEntry->m_pNode);

		pCurrentEntry=pCurrentEntry->m_pNext;

	}

	return nCurrentOffset;

}

int GraphUtil_CalcFlattenedDLLSize(SPete_PluginInfo* pPluginInfo) {

	char* pDLLPath=pPluginInfo->m_pDLLPath;

	FILE* pInputDLL=fopen(pDLLPath,"rb");
	if (pInputDLL==NULL) {

		char MessageString[1024];
		sprintf(MessageString,"DLL file '%s' could not be loaded",pDLLPath);
		MessageBox(NULL,MessageString,"",MB_OK);
		return 0;

	}

	fseek(pInputDLL,0,SEEK_END);
	const int nFileLength=ftell(pInputDLL);
	fseek(pInputDLL,0,SEEK_SET);

	fclose(pInputDLL);

	return nFileLength;

}

int GraphUtil_CalcFlattenedNodeSize(SPete_Node* pNode) {

	int nResult;

	switch(pNode->m_eType) {

		case eType_Effect_Input:
		case eType_Effect_Output:
		case eType_Effect_Plugin:
		case eType_Effect_Switcher: {

			int nInputCount;
			if (pNode->m_eType==eType_Effect_Switcher) {
				nInputCount=PETE_MAX_SWITCHER_INPUTS;
			} else {
				nInputCount=GraphUtil_GetInputCount(pNode);
			}

			const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			nResult=
				sizeof(SPete_EffectNode)+
				nInputByteCount+
				nParameterByteCount;

		}break;

		case eType_Parameter_External:
		case eType_Parameter_Constant:
		case eType_Parameter_Keyboard:
		case eType_Parameter_Midi: {

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			nResult=sizeof(SPete_ParameterNode)+nParameterByteCount;

		}break;

		default: {
			// do nothing
		}break;

	}

	return nResult;

}

void GraphUtil_FlattenDLL(SPete_PluginInfo* pPluginInfo,char* pCurrentData) {

	char* pDLLPath=pPluginInfo->m_pDLLPath;

	FILE* pInputDLL=fopen(pDLLPath,"rb");
	if (pInputDLL==NULL) {

		char MessageString[1024];
		sprintf(MessageString,"DLL file '%s' could not be loaded",pDLLPath);
		MessageBox(NULL,MessageString,"",MB_OK);
		return;

	}

	fseek(pInputDLL,0,SEEK_END);
	const int nFileLength=ftell(pInputDLL);
	fseek(pInputDLL,0,SEEK_SET);

	fread(pCurrentData,nFileLength,1,pInputDLL);

	fclose(pInputDLL);

}

int GraphUtil_FlattenNode(SPete_Node* pNode,SPete_GraphData* pGraph,char* pCurrentData) {

	int nSize;

	char* pDataStart=pGraph->m_pFlattenedData;
	SPete_NodeListEntry* pNodeListHead=pGraph->m_pNodeListHead;

	switch(pNode->m_eType) {

		case eType_Effect_Input:
		case eType_Effect_Output:
		case eType_Effect_Plugin:
		case eType_Effect_Switcher: {

			int nInputCount;
			if (pNode->m_eType==eType_Effect_Switcher) {
				nInputCount=PETE_MAX_SWITCHER_INPUTS;
			} else {
				nInputCount=GraphUtil_GetInputCount(pNode);
			}

			const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);
			SPete_EffectNode* pFlatEffectNode=(SPete_EffectNode*)(pCurrentData);

			pFlatEffectNode->m_AllocationType=eAlloc_Flattened;
			pFlatEffectNode->m_eType=pEffectNode->m_eType;
			pFlatEffectNode->m_InstanceCookie=0;
			pFlatEffectNode->m_LastVisitTime=0;
			pFlatEffectNode->m_nInputIndex=pEffectNode->m_nInputIndex;
			pFlatEffectNode->m_pInternalOutputBuffer=NULL;
			pFlatEffectNode->m_Pos=pEffectNode->m_Pos;
			pFlatEffectNode->m_pOutput=NULL;
			pFlatEffectNode->m_nSwitcherInputCount=pEffectNode->m_nSwitcherInputCount;
			pFlatEffectNode->m_pExternalOutputBuffer=NULL;

			if (nInputCount==0) {

				pFlatEffectNode->m_ppInputs=NULL;

			} else {

				SPete_EffectNode** ppInputs=
					pEffectNode->m_ppInputs;

				SPete_EffectNode** ppFlatInputs=(SPete_EffectNode**)
					(pCurrentData+sizeof(SPete_EffectNode));

				int nCount;
				for (nCount=0; nCount<nInputCount; nCount+=1) {

					if (ppInputs[nCount]!=NULL) {

						ppFlatInputs[nCount]=(SPete_EffectNode*)
							GraphUtil_GetNodeOffset(ppInputs[nCount],pNodeListHead);

					} else {

						ppFlatInputs[nCount]=NULL;

					}

				}

				pFlatEffectNode->m_ppInputs=(SPete_EffectNode**)
					(((char*)ppFlatInputs)-pDataStart);

			}

			if (nParameterCount==0) {

				pFlatEffectNode->m_ppParameters=NULL;

			} else {

				SPete_ParameterNode** ppParameters=
					pEffectNode->m_ppParameters;

				SPete_ParameterNode** ppFlatParameters=(SPete_ParameterNode**)
					(pCurrentData+sizeof(SPete_EffectNode)+nInputByteCount);

				int nCount;
				for (nCount=0; nCount<nParameterCount; nCount+=1) {

					if (ppParameters[nCount]!=NULL) {

						ppFlatParameters[nCount]=(SPete_ParameterNode*)
							GraphUtil_GetNodeOffset(ppParameters[nCount],pNodeListHead);

					} else {

						ppFlatParameters[nCount]=NULL;

					}

				}

				pFlatEffectNode->m_ppParameters=(SPete_ParameterNode**)
					(((char*)ppFlatParameters)-pDataStart);

			}

			if (pEffectNode->m_eType==eType_Effect_Plugin) {

				U32 PluginInfoOffset=
					pEffectNode->m_pPluginInfo->m_FlattenedInfoOffset;

				pFlatEffectNode->m_pPluginInfo=(SPete_PluginInfo*)
					(PluginInfoOffset);

			} else {

				pFlatEffectNode->m_pPluginInfo=NULL;

			}

			pFlatEffectNode->m_Size=pEffectNode->m_Size;

			nSize=sizeof(SPete_EffectNode)+
				nInputByteCount+
				nParameterByteCount;

		}break;

		case eType_Parameter_External:
		case eType_Parameter_Constant:
		case eType_Parameter_Keyboard:
		case eType_Parameter_Midi: {

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)pNode;
			SPete_ParameterNode* pFlatParamNode=(SPete_ParameterNode*)pCurrentData;

			pFlatParamNode->m_AllocationType=eAlloc_Flattened;
			pFlatParamNode->m_eType=pParamNode->m_eType;
			pFlatParamNode->m_LastVisitTime=pParamNode->m_LastVisitTime;
			pFlatParamNode->m_nParametersCount=pParamNode->m_nParametersCount;
			pFlatParamNode->m_OutputValue=pParamNode->m_OutputValue;
			pFlatParamNode->m_Pos=pParamNode->m_Pos;
			strcpy(pFlatParamNode->m_pExternalLabel,pParamNode->m_pExternalLabel);
			pFlatParamNode->m_ExternalDefault=pParamNode->m_ExternalDefault;
			pFlatParamNode->m_nExternalIndex=pParamNode->m_nExternalIndex;
			pFlatParamNode->m_ExternalType=pParamNode->m_ExternalType;

			if (nParameterCount==0) {

				pFlatParamNode->m_ppParameters=NULL;

			} else {

				SPete_ParameterNode** ppParameters=
					pParamNode->m_ppParameters;

				SPete_ParameterNode** ppFlatParameters=(SPete_ParameterNode**)
					(pCurrentData+sizeof(SPete_ParameterNode));

				int nCount;
				for (nCount=0; nCount<nParameterCount; nCount+=1) {

					if (ppParameters[nCount]!=NULL) {

						ppFlatParameters[nCount]=(SPete_ParameterNode*)
							GraphUtil_GetNodeOffset(ppParameters[nCount],pNodeListHead);
					} else {

						ppFlatParameters[nCount]=NULL;

					}

				}

				pFlatParamNode->m_ppParameters=(SPete_ParameterNode**)
					(((char*)ppFlatParameters)-pDataStart);

			}

			pFlatParamNode->m_Size=pParamNode->m_Size;

			nSize=sizeof(SPete_ParameterNode)+nParameterByteCount;

		}break;

		default: {
			// do nothing
		}break;

	}

	return nSize;

}

U32 GraphUtil_GetNodeOffset(SPete_Node* pNode,SPete_NodeListEntry* pNodeListHead) {

	SPete_NodeListEntry* pCurrentEntry=pNodeListHead;

	while (pCurrentEntry!=NULL) {

		if (pCurrentEntry->m_pNode==pNode) {
			return pCurrentEntry->m_FlattenedOffset;
		}

		pCurrentEntry=pCurrentEntry->m_pNext;

	}

	return 0;

}


int GraphUtil_UnFlattenNode(SPete_Node* pNode,SPete_GraphData* pGraph,char* pCurrentData) {

	int nSize;

	char* pDataStart=pGraph->m_pFlattenedData;
	SPete_NodeListEntry* pNodeListHead=pGraph->m_pNodeListHead;

	switch(pNode->m_eType) {

		case eType_Effect_Input:
		case eType_Effect_Output:
		case eType_Effect_Plugin:
		case eType_Effect_Switcher: {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			if (pEffectNode->m_eType==eType_Effect_Plugin) {

				pEffectNode->m_pPluginInfo=(SPete_PluginInfo*)
					(pDataStart+((U32)(pEffectNode->m_pPluginInfo)));

			}

			int nInputCount;
			if (pNode->m_eType==eType_Effect_Switcher) {
				nInputCount=PETE_MAX_SWITCHER_INPUTS;
			} else {
				nInputCount=GraphUtil_GetInputCount(pNode);
			}
			const int nInputByteCount=(nInputCount*sizeof(SPete_EffectNode*));

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			if (nInputCount>0) {

				SPete_EffectNode** ppInputs=(SPete_EffectNode**)
					(pDataStart+((U32)(pEffectNode->m_ppInputs)));

				int nCount;
				for (nCount=0; nCount<nInputCount; nCount+=1) {

					if (ppInputs[nCount]!=NULL) {

						ppInputs[nCount]=(SPete_EffectNode*)
							(pDataStart+((U32)(ppInputs[nCount])));

					}

				}

				pEffectNode->m_ppInputs=ppInputs;

			}

			if (nParameterCount>0) {

				SPete_ParameterNode** ppParameters=(SPete_ParameterNode**)
					(pDataStart+((U32)(pEffectNode->m_ppParameters)));

				int nCount;
				for (nCount=0; nCount<nParameterCount; nCount+=1) {

					if (ppParameters[nCount]!=NULL) {

						ppParameters[nCount]=(SPete_ParameterNode*)
							(pDataStart+((U32)(ppParameters[nCount])));

					}

				}

				pEffectNode->m_ppParameters=ppParameters;

			}

			nSize=sizeof(SPete_EffectNode)+
				nInputByteCount+
				nParameterByteCount;

		}break;

		case eType_Parameter_External:
		case eType_Parameter_Constant:
		case eType_Parameter_Keyboard:
		case eType_Parameter_Midi: {

			const int nParameterCount=GraphUtil_GetParameterCount(pNode);
			const int nParameterByteCount=
				(nParameterCount*sizeof(SPete_ParameterNode*));

			SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)pNode;

			if (nParameterCount>0) {

				SPete_ParameterNode** ppParameters=(SPete_ParameterNode**)
					(pDataStart+((U32)(pParamNode->m_ppParameters)));

				int nCount;
				for (nCount=0; nCount<nParameterCount; nCount+=1) {

					if (ppParameters[nCount]!=NULL) {

						ppParameters[nCount]=(SPete_ParameterNode*)
							(pDataStart+((U32)(ppParameters[nCount])));

					}

				}

				pParamNode->m_ppParameters=ppParameters;

			}

			nSize=sizeof(SPete_ParameterNode)+nParameterByteCount;

		}break;

		default: {
			// do nothing
		}break;

	}

	GraphUtil_MakeListEntry(pGraph,pNode);

	return nSize;

}

void GraphUtil_FreeFlattenedData(SPete_GraphData* pGraph) {

	if (pGraph->m_pFlattenedData!=NULL) {
		free(pGraph->m_pFlattenedData);
	}

	pGraph->m_pFlattenedData=NULL;
	pGraph->m_nFlattenedDataSize=0;

}

void GraphUtil_DoIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie,U32 CurrentVisitTime) {

	if (pNode==NULL) {
		return;
	}

	if (pNode->m_LastVisitTime==CurrentVisitTime) {
		return;
	} else {
		pNode->m_LastVisitTime=CurrentVisitTime;
	}

	const int nParamCount=GraphUtil_GetParameterCount(pNode);

	int nCount;
	for (nCount=0; nCount<nParamCount; nCount+=1) {

		SPete_ParameterNode* pParamNode=pNode->m_ppParameters[nCount];

		GraphUtil_DoIterate(pParamNode,pfnAction,pCookie,CurrentVisitTime);

	}

	const int nInputCount=GraphUtil_GetInputCount(pNode);
	for (nCount=0; nCount<nInputCount; nCount+=1) {

		SPete_EffectNode* pCurrentEffect=(SPete_EffectNode*)(pNode);

		SPete_EffectNode* pInputNode=pCurrentEffect->m_ppInputs[nCount];

		GraphUtil_DoIterate(pInputNode,pfnAction,pCookie,CurrentVisitTime);

	}

	(*pfnAction)(pNode,pCookie);

}

void GraphUtil_DoPrunedIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie,U32 CurrentVisitTime) {

	if (pNode==NULL) {
		return;
	}

	if (pNode->m_LastVisitTime==CurrentVisitTime) {
		return;
	} else {
		pNode->m_LastVisitTime=CurrentVisitTime;
	}

	const int nParamCount=GraphUtil_GetParameterCount(pNode);

	int nCount;
	for (nCount=0; nCount<nParamCount; nCount+=1) {

		SPete_ParameterNode* pParamNode=pNode->m_ppParameters[nCount];

		GraphUtil_DoPrunedIterate(pParamNode,pfnAction,pCookie,CurrentVisitTime);

	}

	if (pNode->m_eType==eType_Effect_Switcher) {

		SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

		const int nInputCount=pEffectNode->m_nSwitcherInputCount;
		const float ParamValue=pEffectNode->m_ppParameters[0]->m_OutputValue;
		const int nCurrentInput=(int)((ParamValue*(nInputCount-1))+0.5f);

		SPete_EffectNode* pInputNode=pEffectNode->m_ppInputs[nCurrentInput];
		GraphUtil_DoPrunedIterate(pInputNode,pfnAction,pCookie,CurrentVisitTime);

	} else {

		const int nInputCount=GraphUtil_GetInputCount(pNode);
		for (nCount=0; nCount<nInputCount; nCount+=1) {

			SPete_EffectNode* pCurrentEffect=(SPete_EffectNode*)(pNode);

			SPete_EffectNode* pInputNode=pCurrentEffect->m_ppInputs[nCount];

			GraphUtil_DoPrunedIterate(pInputNode,pfnAction,pCookie,CurrentVisitTime);

		}

	}

	(*pfnAction)(pNode,pCookie);

}

bool GraphUtil_DoLoadFlattenedDLL(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo) {

	char* pRawImageData=(char*)
		(pGraph->m_pFlattenedData+pPluginInfo->m_FlattenedDLLImageOffset);
	const int nRawImageByteCount=
		pPluginInfo->m_FlattenedDLLImageSize;

	char TempPathString[1024];
	DWORD GetTempPathResult=
		GetTempPath(1024,TempPathString);

	if (GetTempPathResult==0) {
		strcpy(TempPathString,".\\");
	}

	char TempFileName[MAX_PATH];
	DWORD GetTempFileNameResult=
		GetTempFileName(TempPathString,"FRF",0,TempFileName);

	HANDLE hTempFile=CreateFile(
		TempFileName,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hTempFile==INVALID_HANDLE_VALUE) {
		char MessageString[1024];
		sprintf(MessageString,"Temp file '%s' could not be created",TempFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

	DWORD nBytesWritten;
	BOOL bWriteFileResult=WriteFile(
		hTempFile,
		pRawImageData,
		nRawImageByteCount,
		&nBytesWritten,
		NULL);

	if (bWriteFileResult==0) {
		char MessageString[1024];
		sprintf(MessageString,"Temp file '%s' could not be written",TempFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

	CloseHandle(hTempFile);

	bool bLoadSucceded=false;

	pPluginInfo->m_hModuleHandle=LoadLibrary(TempFileName);

	if (pPluginInfo->m_hModuleHandle!=NULL) {

		FF_Main_FuncPtr pFreeFrameMain=
			(FF_Main_FuncPtr)
			GetProcAddress(
			pPluginInfo->m_hModuleHandle,
			"plugMain");

		if (pFreeFrameMain!=NULL) {
			pPluginInfo->m_pMainFunc=pFreeFrameMain;
			bLoadSucceded=true;
		} else {
			FreeLibrary(pPluginInfo->m_hModuleHandle);
		}

	}

	if (!bLoadSucceded) {
		char MessageString[1024];
		sprintf(MessageString,"Plugin '%s' could not be loaded",TempFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

//	char MessageString[1024];
//	sprintf(MessageString,"Plugin '%s' loaded successfully",TempFileName);
//	MessageBox(NULL,MessageString,"",MB_OK);

	return true;

}