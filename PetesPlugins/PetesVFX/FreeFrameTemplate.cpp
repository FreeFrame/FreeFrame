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

/*
	This module is a terrible hack to make adapting my plugins to the
	FreeFrame api easier

*/

#ifdef PLUGIN_INPUT_COUNT
#define PLUGIN_HAS_MULTIPLE_INPUTS
#endif // PLUGIN_INPUT_COUNT

#ifndef TARGET_OS_MAC
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{


	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#endif // TARGET_OS_MAC

/*-------------------------------------------------*/

typedef struct FrameInfoStruct {
	U32* pFrame;
} FrameInfoStruct;

typedef struct SPete_GlobalData {
	VideoInfoStruct m_VideoInfo;
	PLUGIN_SETTINGS m_Settings;
	PLUGIN_DATA m_InstanceData;
#ifndef PLUGIN_HAS_MULTIPLE_INPUTS
	PETE_PIXELDATA32* m_pSourceBuffer;
#else // PLUGIN_HAS_MULTIPLE_INPUTS
	PETE_PIXELDATA32* m_ppSourceBuffers[PLUGIN_INPUT_COUNT];
#endif // PLUGIN_HAS_MULTIPLE_INPUTS
	PETE_PIXELDATA32* m_pOutputBuffer;
} SPete_GlobalData;

PlugInfoStruct*	getInfo(SPete_GlobalData* pGlobalData);
int						initialise(VideoInfoStruct* pVideoInfo,SPete_GlobalData* pGlobalData);
int 					getNumParameters(SPete_GlobalData* pGlobalData);
char*					getParameterName(int n,SPete_GlobalData* pGlobalData);
float					getParameterDefault(int n,SPete_GlobalData* pGlobalData);
char*					getParameterDisplay(int n,SPete_GlobalData* pGlobalData);
int						setParameter(int n,float Value,SPete_GlobalData* pGlobalData);
float					getParameter(int n,SPete_GlobalData* pGlobalData);
void					processFrame(void* pFrame,SPete_GlobalData* pGlobalData);
void					processFrame16Bit(void* pFrame,SPete_GlobalData* pGlobalData);
void					processFrame24Bit(void* pFrame,SPete_GlobalData* pGlobalData);
void					processFrame32Bit(void* pFrame,SPete_GlobalData* pGlobalData);
int						deInitialise(SPete_GlobalData* pGlobalData);
U32						getPluginCaps(U32 nIndex,SPete_GlobalData* pGlobalData);
void*					instantiate(VideoInfoStruct* pVideoInfo);
int						deinstantiate(SPete_GlobalData* pGlobalData);
PlugExtendedInfoStruct*	getExtendedInfo(SPete_GlobalData* pGlobalData);
void					processFrameCopy(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData);
void					processFrameCopy16Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData);
void					processFrameCopy24Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData);
void					processFrameCopy32Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData);
int						getParameterType(int n,SPete_GlobalData* pGlobalData);

static SPete_Parameter* g_pParameters=NULL;
static int g_nParametersCount;
PlugExtendedInfoStruct g_ExtendedInfoStruct;

/*-------------------------------------------------*/

void initParameters(void) {

	PLUGIN_GETPARAMETERS(&g_pParameters,&g_nParametersCount);

}

/*-------------------------------------------------*/

float* getSettingsValue(int nIndex,SPete_GlobalData* pGlobalData) {
	return ((float*)&(pGlobalData->m_Settings))+nIndex;
}

/*-------------------------------------------------*/

#ifndef TARGET_OS_MAC
__declspec(dllexport) void* __stdcall plugMain(DWORD functionCode, LPVOID pParam,DWORD InstanceCookie)
#else // TARGET_OS_MAC
extern "C" void* plugMain(DWORD functionCode,void* pParam,DWORD InstanceCookie)
#endif // TARGET_OS_MAC
{

	void* pResult;

	SPete_GlobalData* pGlobalData=(SPete_GlobalData*)(InstanceCookie);

	switch (functionCode) {

		case FF_GETINFO:
			pResult=getInfo(pGlobalData);
		break;

		case FF_INITIALISE:
			pResult=reinterpret_cast<void*>(initialise((VideoInfoStruct*)(pParam),pGlobalData));
		break;

		case FF_DEINITIALISE:
			pResult=reinterpret_cast<void*>(deInitialise(pGlobalData));
		break;

		case FF_PROCESSFRAME: {
#ifndef PLUGIN_HAS_MULTIPLE_INPUTS
			processFrame(pParam,pGlobalData);
			pResult=NULL;
#else // PLUGIN_HAS_MULTIPLE_INPUTS
			pResult=(void*)FF_FAIL;
#endif // PLUGIN_HAS_MULTIPLE_INPUTS
		}break;

		case FF_GETNUMPARAMETERS:
			pResult=reinterpret_cast<void*>(getNumParameters(pGlobalData));
		break;

		case FF_GETPARAMETERNAME:
			pResult=getParameterName(reinterpret_cast<int>(pParam),pGlobalData);
		break;

		case FF_GETPARAMETERDEFAULT: {
			float ResultValue=getParameterDefault(reinterpret_cast<int>(pParam),pGlobalData);
			pResult=(void*)(*((U32*)(&ResultValue)));
		}break;

		case FF_GETPARAMETERDISPLAY:
			pResult=getParameterDisplay(reinterpret_cast<int>(pParam),pGlobalData);
		break;

		case FF_SETPARAMETER: {
			SetParameterStruct Args=*((SetParameterStruct*)pParam);
			pResult=reinterpret_cast<void*>(setParameter(Args.index,Args.value,pGlobalData));
		}break;

		case FF_GETPARAMETER: {
			float ResultValue=getParameter(reinterpret_cast<int>(pParam),pGlobalData);
			pResult=(void*)(*((U32*)(&ResultValue)));
		}break;

		case FF_GETPLUGINCAPS: {
			pResult=(void*)getPluginCaps(reinterpret_cast<DWORD>(pParam),pGlobalData);
		}break;

		case FF_INSTANTIATE: {
			pResult=instantiate((VideoInfoStruct*)(pParam));
		}break;

		case FF_DEINSTANTIATE: {
			pResult=reinterpret_cast<void*>(deinstantiate(pGlobalData));
		}break;

		case FF_GETEXTENDEDINFO: {
			pResult=reinterpret_cast<void*>(getExtendedInfo(pGlobalData));
		}break;

		case FF_PROCESSFRAMECOPY: {
			processFrameCopy((ProcessFrameCopyStruct*)pParam,pGlobalData);
			pResult=NULL;
		}break;

		case FF_GETPARAMETERTYPE: {
			pResult=reinterpret_cast<void*>(getParameterType(reinterpret_cast<int>(pParam),pGlobalData));
		}break;

		default:
			pResult=NULL;
		break;
	}

	return pResult;
}

/*-------------------------------------------------*/

PlugInfoStruct*	getInfo(SPete_GlobalData* pGlobalData) {
	return &g_PluginInfo;
}

/*-------------------------------------------------*/

int initialise(VideoInfoStruct* pVideoInfo,SPete_GlobalData* pGlobalData) {

	return FF_SUCCESS;

}

/*-------------------------------------------------*/

int getNumParameters(SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	return g_nParametersCount;

}

/*-------------------------------------------------*/

char* getParameterName(int n,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return "";
	}

	return g_pParameters[n].pName;

}

/*-------------------------------------------------*/

float getParameterDefault(int n,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return FF_FAIL;
	}

	SPete_Parameter* pParam=&g_pParameters[n];
	const float Range=(pParam->Maximum-pParam->Minimum);
	const float Minimum=pParam->Minimum;
	const float Result=(pParam->Default-Minimum)/Range;

	return Result;

}

/*-------------------------------------------------*/

char* getParameterDisplay(int n,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return "";
	}

	static char pResult[256];

	float Value=*(getSettingsValue(n,pGlobalData));
	int nType=g_pParameters[n].Type;

	switch (nType) {

		case PETE_PARAM_FLOAT: {
			sprintf(pResult,"%.1f",Value);
		}break;

		case PETE_PARAM_BOOL: {
			if (Value>0.0f) {
				sprintf(pResult,"On");
			} else {
				sprintf(pResult,"Off");
			}
		}break;

		case PETE_PARAM_INT: {
			sprintf(pResult,"%d",int(ceilf(Value)));
		}break;

		default: {
			sprintf(pResult,"<Unknown Type>");
		}break;

	}

	return pResult;
}

/*-------------------------------------------------*/

int setParameter(int n,float Value,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return FF_FAIL;
	}

	SPete_Parameter* pParam=&g_pParameters[n];
	const float Range=(pParam->Maximum-pParam->Minimum);
	const float Minimum=pParam->Minimum;

	Value*=Range;
	Value+=Minimum;

	*(getSettingsValue(n,pGlobalData))=Value;

	return FF_SUCCESS;
}

/*-------------------------------------------------*/

float getParameter(int n,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return 0.0f;
	}

	SPete_Parameter* pParam=&g_pParameters[n];
	const float Range=(pParam->Maximum-pParam->Minimum);
	const float Minimum=pParam->Minimum;
	const float Result=((*(getSettingsValue(n,pGlobalData)))-Minimum)/Range;

	return Result;

}

/*-------------------------------------------------*/
#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

void processFrame(void* pFrame,SPete_GlobalData* pGlobalData) {

	switch (pGlobalData->m_VideoInfo.bitDepth) {

		case 0:
			processFrame16Bit(pFrame,pGlobalData);
		break;

		case 1:
			processFrame24Bit(pFrame,pGlobalData);
		break;

		case 2:
			processFrame32Bit(pFrame,pGlobalData);
		break;

		default:
			assert(FALSE); // should never get here
		break;

	}

}

/*-------------------------------------------------*/

void processFrame16Bit(void* pFrame,SPete_GlobalData* pGlobalData) {

#ifndef PLUGIN_INPUT_COUNT

	PETE_PIXELDATA16* pOutput=(PETE_PIXELDATA16*)pFrame;

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA16);

	PETE_PIXELDATA32* pSourceBuffer=pGlobalData->m_pSourceBuffer;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	Pete_CopyAndConvert16Bit565To32Bit(pOutput,pSourceBuffer,nNumPixels);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pSourceBuffer,
		pOutputBuffer);

	Pete_CopyAndConvert32BitTo16Bit565(pOutputBuffer,pOutput,nNumPixels);

#endif // PLUGIN

}

/*-------------------------------------------------*/

void processFrame24Bit(void* pFrame,SPete_GlobalData* pGlobalData) {

	PETE_PIXELDATA24* pOutput=(PETE_PIXELDATA24*)pFrame;

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA24);

	PETE_PIXELDATA32* pSourceBuffer=pGlobalData->m_pSourceBuffer;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	Pete_CopyAndConvert24BitTo32Bit(pOutput,pSourceBuffer,nNumPixels);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pSourceBuffer,
		pOutputBuffer);

	Pete_CopyAndConvert32BitTo24Bit(pOutputBuffer,pOutput,nNumPixels);

}

/*-------------------------------------------------*/

void processFrame32Bit(void* pFrame,SPete_GlobalData* pGlobalData) {

	PETE_PIXELDATA32* pOutput=(PETE_PIXELDATA32*)pFrame;
	PETE_PIXELDATA32* pSource=pGlobalData->m_pSourceBuffer;

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA32);

	memcpy(pSource,pOutput,nNumBytes);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pSource,
		pOutput);

}

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

/*-------------------------------------------------*/

int deInitialise(SPete_GlobalData* pGlobalData) {

	return FF_SUCCESS;

}

/*-------------------------------------------------*/

U32 getPluginCaps(U32 nIndex,SPete_GlobalData* pGlobalData) {

	U32 Result;

	switch (nIndex) {

		case FF_CAP_16BITVIDEO:
			Result=FF_TRUE;
		break;

		case FF_CAP_24BITVIDEO:
			Result=FF_TRUE;
		break;

		case FF_CAP_32BITVIDEO:
			Result=FF_TRUE;
		break;

		case FF_CAP_PROCESSFRAMECOPY:
			Result=FF_TRUE;
		break;

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

		case FF_CAP_MINIMUMINPUTFRAMES:
			Result=1;
		break;

		case FF_CAP_MAXIMUMINPUTFRAMES:
			Result=1;
		break;

#else // PLUGIN_HAS_MULTIPLE_INPUTS

		case FF_CAP_MINIMUMINPUTFRAMES:
			Result=PLUGIN_INPUT_COUNT;
		break;

		case FF_CAP_MAXIMUMINPUTFRAMES:
			Result=PLUGIN_INPUT_COUNT;
		break;

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

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

void* instantiate(VideoInfoStruct* pVideoInfo) {

	int nStructByteCount=sizeof(SPete_GlobalData);

	SPete_GlobalData* pGlobalData=(SPete_GlobalData*)
		Pete_NewHandle(nStructByteCount);

	Pete_ZeroMemory(pGlobalData,nStructByteCount);

	pGlobalData->m_VideoInfo=*pVideoInfo;

	const int nWidth=pVideoInfo->frameWidth;
	const int nHeight=pVideoInfo->frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA32);

	int nCount;

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

	pGlobalData->m_pSourceBuffer=(PETE_PIXELDATA32*)Pete_NewHandle(nNumBytes);
	if (pGlobalData->m_pSourceBuffer==NULL) {
		deinstantiate(pGlobalData);
		return NULL;
	}

#else // PLUGIN_HAS_MULTIPLE_INPUTS

	for (nCount=0; nCount<PLUGIN_INPUT_COUNT; nCount+=1) {

		pGlobalData->m_ppSourceBuffers[nCount]=(PETE_PIXELDATA32*)Pete_NewHandle(nNumBytes);
		if (pGlobalData->m_ppSourceBuffers[nCount]==NULL) {
			deinstantiate(pGlobalData);
			return NULL;
		}

	}

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

	pGlobalData->m_pOutputBuffer=(PETE_PIXELDATA32*)Pete_NewHandle(nNumBytes);
	if (pGlobalData->m_pOutputBuffer==NULL) {
		deinstantiate(pGlobalData);
		return NULL;
	}

	for (nCount=0; nCount<getNumParameters(pGlobalData); nCount+=1) {
		setParameter(nCount,getParameterDefault(nCount,pGlobalData),pGlobalData);
	}

	Pete_ZeroMemory(&pGlobalData->m_InstanceData,sizeof(pGlobalData->m_InstanceData));

	PLUGIN_INIT(&pGlobalData->m_InstanceData,nWidth,nHeight);

	return (void*)(pGlobalData);
}

/*-------------------------------------------------*/

int deinstantiate(SPete_GlobalData* pGlobalData) {

	PLUGIN_DEINIT(&pGlobalData->m_InstanceData);

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

	if (pGlobalData->m_pSourceBuffer!=NULL) {
		Pete_FreeHandle(pGlobalData->m_pSourceBuffer);
		pGlobalData->m_pSourceBuffer=NULL;
	}

#else // PLUGIN_HAS_MULTIPLE_INPUTS

	int nCount;
	for (nCount=0; nCount<PLUGIN_INPUT_COUNT; nCount+=1) {

		if (pGlobalData->m_ppSourceBuffers[nCount]!=NULL) {
			Pete_FreeHandle(pGlobalData->m_ppSourceBuffers[nCount]);
			pGlobalData->m_ppSourceBuffers[nCount]=NULL;
		}

	}

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

	if (pGlobalData->m_pOutputBuffer!=NULL) {
		Pete_FreeHandle(pGlobalData->m_pOutputBuffer);
		pGlobalData->m_pOutputBuffer=NULL;
	}

	int nStructByteCount=sizeof(SPete_GlobalData);

	Pete_ZeroMemory(pGlobalData,nStructByteCount);

	Pete_FreeHandle(pGlobalData);

	return FF_SUCCESS;

}

/*-------------------------------------------------*/

PlugExtendedInfoStruct*	getExtendedInfo(SPete_GlobalData* pGlobalData) {

	Pete_ZeroMemory(&g_ExtendedInfoStruct,sizeof(g_ExtendedInfoStruct));

	g_ExtendedInfoStruct.PluginMajorVersion=1;
	g_ExtendedInfoStruct.PluginMajorVersion=0;
	g_ExtendedInfoStruct.About="Effects Plugin by Pete Warden";
	g_ExtendedInfoStruct.Description="No description written";

	return &g_ExtendedInfoStruct;

}

/*-------------------------------------------------*/

void processFrameCopy(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData) {

	switch (pGlobalData->m_VideoInfo.bitDepth) {

		case 0:
			processFrameCopy16Bit(pArgs,pGlobalData);
		break;

		case 1:
			processFrameCopy24Bit(pArgs,pGlobalData);
		break;

		case 2:
			processFrameCopy32Bit(pArgs,pGlobalData);
		break;

		default:
			assert(FALSE); // should never get here
		break;

	}

}

/*-------------------------------------------------*/

void processFrameCopy16Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData) {

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA16* pInput=*((PETE_PIXELDATA16**)(pArgs->InputFrames));
	PETE_PIXELDATA16* pOutput=(PETE_PIXELDATA16*)(pArgs->OutputFrame);

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA16);

	PETE_PIXELDATA32* pSourceBuffer=pGlobalData->m_pSourceBuffer;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	Pete_CopyAndConvert16Bit565To32Bit(pInput,pSourceBuffer,nNumPixels);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pSourceBuffer,
		pOutputBuffer);

#else // PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA16** ppInputs=((PETE_PIXELDATA16**)(pArgs->InputFrames));
	PETE_PIXELDATA16* pOutput=(PETE_PIXELDATA16*)(pArgs->OutputFrame);

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA16);

	PETE_PIXELDATA32** ppSourceBuffers=pGlobalData->m_ppSourceBuffers;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	int nCount;
	for (nCount=0; nCount<PLUGIN_INPUT_COUNT; nCount+=1) {

		Pete_CopyAndConvert16Bit565To32Bit(ppInputs[nCount],ppSourceBuffers[nCount],nNumPixels);

	}

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		ppSourceBuffers,
		pOutputBuffer);

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

	Pete_CopyAndConvert32BitTo16Bit565(pOutputBuffer,pOutput,nNumPixels);

}

/*-------------------------------------------------*/

void processFrameCopy24Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData) {

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA24* pInput=*((PETE_PIXELDATA24**)(pArgs->InputFrames));
	PETE_PIXELDATA24* pOutput=(PETE_PIXELDATA24*)(pArgs->OutputFrame);

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA24);

	PETE_PIXELDATA32* pSourceBuffer=pGlobalData->m_pSourceBuffer;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	Pete_CopyAndConvert24BitTo32Bit(pInput,pSourceBuffer,nNumPixels);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pSourceBuffer,
		pOutputBuffer);

#else // PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA24** ppInputs=((PETE_PIXELDATA24**)(pArgs->InputFrames));
	PETE_PIXELDATA24* pOutput=(PETE_PIXELDATA24*)(pArgs->OutputFrame);

	const int nWidth=pGlobalData->m_VideoInfo.frameWidth;
	const int nHeight=pGlobalData->m_VideoInfo.frameHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*SIZEOF_PETE_PIXELDATA16);

	PETE_PIXELDATA32** ppSourceBuffers=pGlobalData->m_ppSourceBuffers;
	PETE_PIXELDATA32* pOutputBuffer=pGlobalData->m_pOutputBuffer;

	int nCount;
	for (nCount=0; nCount<PLUGIN_INPUT_COUNT; nCount+=1) {

		Pete_CopyAndConvert24BitTo32Bit(ppInputs[nCount],ppSourceBuffers[nCount],nNumPixels);

	}

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		ppSourceBuffers,
		pOutputBuffer);

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

	Pete_CopyAndConvert32BitTo24Bit(pOutputBuffer,pOutput,nNumPixels);

}

/*-------------------------------------------------*/

void processFrameCopy32Bit(ProcessFrameCopyStruct* pArgs,SPete_GlobalData* pGlobalData) {

#ifndef PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA32* pInput=*((PETE_PIXELDATA32**)(pArgs->InputFrames));
	PETE_PIXELDATA32* pOutput=(PETE_PIXELDATA32*)(pArgs->OutputFrame);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		pInput,
		pOutput);

#else // PLUGIN_HAS_MULTIPLE_INPUTS

	PETE_PIXELDATA32** ppInputs=((PETE_PIXELDATA32**)(pArgs->InputFrames));
	PETE_PIXELDATA32* pOutput=(PETE_PIXELDATA32*)(pArgs->OutputFrame);

	PLUGIN_RENDER(&pGlobalData->m_InstanceData,
		&pGlobalData->m_Settings,
		ppInputs,
		pOutput);

#endif // PLUGIN_HAS_MULTIPLE_INPUTS

}

/*-------------------------------------------------*/

int getParameterType(int n,SPete_GlobalData* pGlobalData) {

	if (g_pParameters==NULL) {
		initParameters();
	}

	if ((n<0)||(n>=g_nParametersCount)||(g_pParameters==NULL)) {
		return FF_TYPE_STANDARD;
	}

	int nResult;

	int nType=g_pParameters[n].Type;

	switch (nType) {

		case PETE_PARAM_FLOAT: {
			nResult=FF_TYPE_STANDARD;
		}break;

		case PETE_PARAM_BOOL: {
			nResult=FF_TYPE_BOOLEAN;
		}break;

		case PETE_PARAM_INT: {
			nResult=FF_TYPE_STANDARD;
		}break;

		case PETE_PARAM_POS_X: {
			nResult=FF_TYPE_XPOS;
		}break;

		case PETE_PARAM_POS_Y: {
			nResult=FF_TYPE_YPOS;
		}break;

		case PETE_PARAM_COLOUR_R: {
			nResult=FF_TYPE_RED;
		}break;

		case PETE_PARAM_COLOUR_G: {
			nResult=FF_TYPE_GREEN;
		}break;

		case PETE_PARAM_COLOUR_B: {
			nResult=FF_TYPE_BLUE;
		}break;

		default: {
			nResult=FF_TYPE_STANDARD;
		}break;

	}

	return nResult;

}

/*-------------------------------------------------*/
