/* Copyright (C) 2002 Pete Warden

Effect plugin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <windows.h>
#include <stdio.h>
#include <freeframe.h>
#include "PeteHelpers.h"
#include "FFTemplate.h"
#include "GraphUtil.h"
#include "RunGraph.h"

#include "resource.h"
	
/*-------------------------------------------------*/

typedef struct FrameInfoStruct {
	U32* pFrame;
} FrameInfoStruct;

typedef struct SPete_InstanceData {
	VideoInfoStruct m_VideoInfo;
	SPete_GraphData m_GraphData;
	SPete_RunTimeEnvironment m_Environment;
};

void					FFTemplate_Init(HMODULE hModule);
void					FFTemplate_DeInit(void);
struct PlugInfoStruct*	FFTemplate_GetInfo(SPete_InstanceData* pInstanceData);
int						FFTemplate_ProcessFrame(SPete_InstanceData* pInstanceData,void* pParam);
int						FFTemplate_GetNumParameters(SPete_InstanceData* pInstanceData);
char*					FFTemplate_GetParameterName(SPete_InstanceData* pInstanceData,int nIndex);
float					FFTemplate_GetParameterDefault(SPete_InstanceData* pInstanceData,int nIndex);
char*					FFTemplate_GetParameterDisplay(SPete_InstanceData* pInstanceData,int nIndex);
int						FFTemplate_SetParameter(SPete_InstanceData* pInstanceData,int nIndex,float Value);
float					FFTemplate_GetParameter(SPete_InstanceData* pInstanceData,int nIndex);
U32						FFTemplate_GetPluginCaps(SPete_InstanceData* pInstanceData,U32 nIndex);
SPete_InstanceData*		FFTemplate_Instantiate(VideoInfoStruct* pVideoInfo);
void					FFTemplate_DeInstantiate(SPete_InstanceData* pInstanceData);
PlugExtendedInfoStruct*	FFTemplate_GetExtendedInfo(SPete_InstanceData* pInstanceData);
int						FFTemplate_ProcessFrameCopy(SPete_InstanceData* pInstanceData,ProcessFrameCopyStruct* pArgs);
int						FFTemplate_GetParameterType(SPete_InstanceData* pInstanceData,int nIndex);

static SPete_Parameter* g_pParameters=NULL;
static int g_nParametersCount;
PlugExtendedInfoStruct g_ExtendedInfoStruct;
HMODULE g_hModule;

SPete_GraphData g_GraphTemplate;
SPete_RunTimeEnvironment g_EnvTemplate;

static struct PlugInfoStruct g_PluginInfo={
	PETE_FF_MAJORVERSION,
	PETE_FF_MINORVERSION,
	{'C','H','N','0'},
	"FFTemplate",
	0
};

/*-------------------------------------------------*/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:{

			g_hModule=(HMODULE)(hModule);
			FFTemplate_Init(g_hModule);

		}break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

/*-------------------------------------------------*/

void FFTemplate_Init(HMODULE hModule) {

	HRSRC hChainDataResource=FindResource(
		hModule,
		MAKEINTRESOURCE(CHAIN_DATA_RESOURCE_ID),
		"GRAPH");

	if (hChainDataResource==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource not found","",MB_OK);
		return;
	}
	
	HGLOBAL hChainData=LoadResource(
		hModule,hChainDataResource);

	if (hChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be loaded","",MB_OK);
		return;
	}

	const DWORD nSizeOfChainData=SizeofResource(
		hModule,hChainDataResource);

	if (nSizeOfChainData==0) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be sized","",MB_OK);
		return;
	}

	char* pChainData=(char*)LockResource(hChainData);
	if (pChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be locked","",MB_OK);
		return;
	}

	char* pFlatDataCopy=(char*)(malloc(nSizeOfChainData));

	if (pFlatDataCopy==NULL) {
		MessageBox(NULL,"Memory allocation failed","",MB_OK);
		return;
	}

	memcpy(pFlatDataCopy,pChainData,nSizeOfChainData);

	UnlockResource(hChainData);

	GraphUtil_InitFromFlat(&g_GraphTemplate,pFlatDataCopy,nSizeOfChainData);

	RunGraph_Init(g_GraphTemplate.m_pRootNode,
		16,16,
		&g_EnvTemplate);

}

/*-------------------------------------------------*/

void FFTemplate_DeInit(void) {


}

/*-------------------------------------------------*/

#ifndef TARGET_OS_MAC
__declspec(dllexport) void* __stdcall plugMain(DWORD functionCode, LPVOID pParam,DWORD InstanceCookie) 
#else // TARGET_OS_MAC
extern "C" void* plugMain(DWORD functionCode,void* pParam,DWORD InstanceCookie)
#endif // TARGET_OS_MAC
{

	void* pResult;

	SPete_InstanceData* pInstanceData=(SPete_InstanceData*)(InstanceCookie);

	switch (functionCode) {

		case FF_GETINFO:
			pResult=FFTemplate_GetInfo(pInstanceData);
		break;

		case FF_INITIALISE:
//			FFTemplate_Init(g_hModule);
		break;

		case FF_DEINITIALISE:
			FFTemplate_DeInit();
		break;

		case FF_PROCESSFRAME: {
			FFTemplate_ProcessFrame(pInstanceData,pParam);
			pResult=NULL;
		}break;

		case FF_GETNUMPARAMETERS:
			pResult=reinterpret_cast<void*>(FFTemplate_GetNumParameters(pInstanceData));
		break;

		case FF_GETPARAMETERNAME:
			pResult=(void*)
				FFTemplate_GetParameterName(pInstanceData,reinterpret_cast<int>(pParam));
		break;

		case FF_GETPARAMETERDEFAULT: {
			float ResultValue=
				FFTemplate_GetParameterDefault(pInstanceData,reinterpret_cast<int>(pParam));

			pResult=(void*)(*((U32*)(&ResultValue)));
		}break;

		case FF_GETPARAMETERDISPLAY:
			pResult=(void*)
				FFTemplate_GetParameterDisplay(
				pInstanceData,reinterpret_cast<int>(pParam));
		break;

		case FF_SETPARAMETER: {
			SetParameterStruct Args=*((SetParameterStruct*)pParam);
			pResult=reinterpret_cast<void*>
				(FFTemplate_SetParameter(pInstanceData,Args.index,Args.value));
		}break;

		case FF_GETPARAMETER: {
			float ResultValue=
				FFTemplate_GetParameter(pInstanceData,reinterpret_cast<int>(pParam));
			pResult=(void*)(*((U32*)(&ResultValue)));
		}break;

		case FF_GETPLUGINCAPS: {
			pResult=(void*)
				FFTemplate_GetPluginCaps(pInstanceData,reinterpret_cast<DWORD>(pParam));
		}break;

		case FF_INSTANTIATE: {
			VideoInfoStruct* pVideoInfo=(VideoInfoStruct*)(pParam);
			pInstanceData=FFTemplate_Instantiate(pVideoInfo);
			pResult=pInstanceData;
		}break;

		case FF_DEINSTANTIATE: {
			FFTemplate_DeInstantiate(pInstanceData);
		}break;

		case FF_GETEXTENDEDINFO: {
			pResult=reinterpret_cast<void*>
				(FFTemplate_GetExtendedInfo(pInstanceData));
		}break;

		case FF_PROCESSFRAMECOPY: {
			FFTemplate_ProcessFrameCopy(pInstanceData,(ProcessFrameCopyStruct*)pParam);
			pResult=NULL;
		}break;

		case FF_GETPARAMETERTYPE: {
			pResult=reinterpret_cast<void*>
				(FFTemplate_GetParameterType(pInstanceData,reinterpret_cast<int>(pParam)));
		}break;

		default:
			pResult=NULL;
		break;
	}

	return pResult;
}

/*-------------------------------------------------*/

struct PlugInfoStruct*	FFTemplate_GetInfo(SPete_InstanceData* pInstanceData) {
	return &g_PluginInfo;
}

/*-------------------------------------------------*/

int FFTemplate_ProcessFrame(SPete_InstanceData* pInstanceData,void* pParam) {

	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	VideoInfoStruct* pVideoInfo=&pInstanceData->m_VideoInfo;

	const int nWidth=pVideoInfo->frameWidth;
	const int nHeight=pVideoInfo->frameHeight;
	const int nScreenByteCount=(nWidth*nHeight*sizeof(U32));

	ProcessFrameCopyStruct CopyStruct;
	CopyStruct.InputFrames=(void**)(malloc(sizeof(void*)));
	CopyStruct.InputFrames[0]=(void*)(malloc(nScreenByteCount));

	if ((CopyStruct.InputFrames==NULL)||
		(CopyStruct.InputFrames[0]==NULL)) {
		return FF_FAIL;
	}

	memcpy(CopyStruct.InputFrames[0],pParam,nScreenByteCount);

	CopyStruct.numInputFrames=1;

	CopyStruct.OutputFrame=pParam;

	FFTemplate_ProcessFrameCopy(pInstanceData,&CopyStruct);

	free(CopyStruct.InputFrames[0]);
	free(CopyStruct.InputFrames);

	return FF_SUCCESS;

}

/*-------------------------------------------------*/

int FFTemplate_GetNumParameters(SPete_InstanceData* pInstanceData) {

	SPete_RunTimeEnvironment* pEnv=&g_EnvTemplate;

	return pEnv->m_nFFParamsCount;

}

/*-------------------------------------------------*/

char* FFTemplate_GetParameterName(SPete_InstanceData* pInstanceData,int nIndex) {

	SPete_RunTimeEnvironment* pEnv=&g_EnvTemplate;

	return pEnv->m_pParamsInfo[nIndex].m_pName;

}

/*-------------------------------------------------*/

float FFTemplate_GetParameterDefault(SPete_InstanceData* pInstanceData,int nIndex) {

	SPete_RunTimeEnvironment* pEnv=&g_EnvTemplate;

	return pEnv->m_pParamsInfo[nIndex].m_Default;

}

/*-------------------------------------------------*/

char* FFTemplate_GetParameterDisplay(SPete_InstanceData* pInstanceData,int nIndex) {

	return "Foo";

}

/*-------------------------------------------------*/

int FFTemplate_SetParameter(SPete_InstanceData* pInstanceData,int nIndex,float Value) {

	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	pEnv->m_pFFParams[nIndex]=Value;

	return FF_SUCCESS;
}

/*-------------------------------------------------*/

float FFTemplate_GetParameter(SPete_InstanceData* pInstanceData,int nIndex) {

	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	return pEnv->m_pFFParams[nIndex];

}

/*-------------------------------------------------*/

U32 FFTemplate_GetPluginCaps(SPete_InstanceData* pInstanceData,U32 nIndex) {

	U32 Result;
	
	switch (nIndex) {

		case FF_CAP_16BITVIDEO:
			Result=FF_FALSE;
		break;

		case FF_CAP_24BITVIDEO:
			Result=FF_FALSE;
		break;

		case FF_CAP_32BITVIDEO:
			Result=FF_TRUE;
		break;

		case FF_CAP_PROCESSFRAMECOPY:
			Result=FF_TRUE;
		break;

		case FF_CAP_MINIMUMINPUTFRAMES:
			Result=1;
		break;

		case FF_CAP_MAXIMUMINPUTFRAMES:
			Result=1;
		break;

		case FF_CAP_COPYORINPLACE:
			Result=FF_CAP_PREFER_COPY;
		break;

		default:
			Result=FF_FALSE;
		break;

	}

	return Result;
}

/*-------------------------------------------------*/

SPete_InstanceData* FFTemplate_Instantiate(VideoInfoStruct* pVideoInfo) {

	SPete_InstanceData* pInstanceData=(SPete_InstanceData*)
		malloc(sizeof(SPete_InstanceData));

	if (pInstanceData==NULL) {
		return NULL;
	}

	pInstanceData->m_VideoInfo=*pVideoInfo;

	SPete_GraphData* pGraph=&pInstanceData->m_GraphData;

	HRSRC hChainDataResource=FindResource(
		g_hModule,
		MAKEINTRESOURCE(CHAIN_DATA_RESOURCE_ID),
		"GRAPH");

	if (hChainDataResource==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource not found","",MB_OK);
		return NULL;
	}
	
	HGLOBAL hChainData=LoadResource(
		g_hModule,hChainDataResource);

	if (hChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be loaded","",MB_OK);
		return NULL;
	}

	const DWORD nSizeOfChainData=SizeofResource(
		g_hModule,hChainDataResource);

	if (nSizeOfChainData==0) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be sized","",MB_OK);
		return NULL;
	}

	char* pChainData=(char*)LockResource(hChainData);
	if (pChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be locked","",MB_OK);
		return NULL;
	}

	char* pFlatDataCopy=(char*)(malloc(nSizeOfChainData));

	if (pFlatDataCopy==NULL) {
		MessageBox(NULL,"Memory allocation failed","",MB_OK);
		return NULL;
	}

	memcpy(pFlatDataCopy,pChainData,nSizeOfChainData);

	UnlockResource(hChainData);

	GraphUtil_InitFromFlat(pGraph,pFlatDataCopy,nSizeOfChainData);

	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	RunGraph_Init(pGraph->m_pRootNode,
		pVideoInfo->frameWidth,pVideoInfo->frameHeight,
		pEnv);

	return pInstanceData;

}

/*-------------------------------------------------*/

void FFTemplate_DeInstantiate(SPete_InstanceData* pInstanceData) {

	SPete_GraphData* pGraph=&pInstanceData->m_GraphData;
	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	RunGraph_DeInit(pGraph->m_pRootNode,pEnv);

	GraphUtil_DeInit(&pInstanceData->m_GraphData);

	free(pInstanceData);

}

/*-------------------------------------------------*/

PlugExtendedInfoStruct*	FFTemplate_GetExtendedInfo(SPete_InstanceData* pInstanceData) {

	Pete_ZeroMemory(&g_ExtendedInfoStruct,sizeof(g_ExtendedInfoStruct));

	g_ExtendedInfoStruct.PluginMajorVersion=1;
	g_ExtendedInfoStruct.PluginMajorVersion=0;
	g_ExtendedInfoStruct.About="Chained Effects Plugin (c) various authors";
	g_ExtendedInfoStruct.Description="No description written";
	
	return &g_ExtendedInfoStruct;

}
/*-------------------------------------------------*/

int FFTemplate_ProcessFrameCopy(SPete_InstanceData* pInstanceData,ProcessFrameCopyStruct* pStruct) {

	SPete_GraphData* pGraph=&pInstanceData->m_GraphData;
	SPete_RunTimeEnvironment* pEnv=&pInstanceData->m_Environment;

	const int nInputsCount=pStruct->numInputFrames;

	int nCount;
	for (nCount=0; nCount<nInputsCount; nCount+=1) {
		pEnv->m_ppInputs[nCount]=(U32*)(pStruct->InputFrames[nCount]);
	}

	pEnv->m_nInputsCount=nInputsCount;

	pEnv->m_pOutput=(U32*)(pStruct->OutputFrame);

	RunGraph_Run(pGraph->m_pRootNode,pEnv);

	return FF_SUCCESS;

}

/*-------------------------------------------------*/

int FFTemplate_GetParameterType(SPete_InstanceData* pInstanceData,int nIndex) {

	return FF_TYPE_STANDARD;

}

