/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2001 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*
	SDK_Noise.cpp

	Part of the Adobe After Effects 5.5 SDK.
	Copyright 2001 Adobe Systems Incorporated.
	All Rights Reserved.

	This plug-in demonstrates parameter supervision.

	Revision History

	Version		Change										Engineer	Date
	=======		======										========	======
	1.0			created										eks			8/31/99

	1.1			moved to C++, overhauled					bbb			10/10/01

*/


#include "FreeFrameHost.h"

#include "AEGP_SuiteHandler.h"

#include "FreeFrame.h"
#include "PeteHelpers.h"

#include <assert.h>

struct SFreeFrameHost_EffectData {
	HMODULE m_hModuleHandle;
	FF_Main_FuncPtr m_pMainFunc;
};

const int nMaxEffectCount=256;

struct SFreeFrameHost_InstanceData {
	SFreeFrameHost_EffectData m_Effects[nMaxEffectCount];
	int nEffectsCount;
	int nSelectedEffect;
	VideoInfoStruct VideoInfo;
	int nSuperSampleSize;
	int nSourceWidth;
	int nSourceHeight;
};

static PF_Err SequenceSetup(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err SequenceSetdown(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err SequenceResetup(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err
HandleChangedParam(
	PF_InData					*in_data,
	PF_OutData					*out_data,
	PF_ParamDef					*params[],
	PF_LayerDef					*output,
	PF_UserChangedParamExtra	*extra);

char* FreeFrame_GetPluginDirPrefix(void);
void FreeFrame_LoadAllPlugins(SFreeFrameHost_InstanceData* pInstanceData);
bool FreeFrame_LoadPlugin(SFreeFrameHost_EffectData* pEffectData,char* pFileName);
char* FreeFrame_BuildPopupString(SFreeFrameHost_InstanceData* pInstanceData);
void FreeFrame_InitEffect(SFreeFrameHost_InstanceData* pInstanceData);
void FreeFrame_DeInitEffect(SFreeFrameHost_InstanceData* pInstanceData);
void FreeFrame_InitEffectParams(PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output,
								SFreeFrameHost_InstanceData* pEffectData,
								bool bValueChangeAllowed);

char* FreeFrame_GetEffectName(SFreeFrameHost_EffectData* pEffectData);
void FreeFrame_LoadPluginsFromDir(SFreeFrameHost_InstanceData* pInstanceData,char* pCurrentDir,int nRecursionLevel);

/*	Use of AEGP_SuiteHandler requires the implementation of the following
	member function.
*/

void AEGP_SuiteHandler::MissingSuiteError() const
{
//	throw PF_Err_INTERNAL_STRUCT_DAMAGED;	// quit out to the main app with an error code
};

PF_Err
main (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void* pExtra) {

	PF_Err		err = PF_Err_NONE;

	switch (cmd)
	{
		case PF_Cmd_ABOUT:
			err = About(in_data,out_data,params,output);
			break;
		case PF_Cmd_GLOBAL_SETUP:
			err = GlobalSetup(in_data,out_data,params,output);
			break;
		case PF_Cmd_PARAMS_SETUP:
			err = ParamsSetup(in_data,out_data,params,output);
			break;
		case PF_Cmd_SEQUENCE_SETUP:
			err = SequenceSetup(in_data,out_data,params,output);
			break;
		case PF_Cmd_SEQUENCE_SETDOWN:
			err = SequenceSetdown(in_data,out_data,params,output);
			break;
		case PF_Cmd_SEQUENCE_RESETUP:
			err = SequenceResetup(in_data,out_data,params,output);
			break;
		case PF_Cmd_RENDER:
			err = Render(in_data,out_data,params,output);
			break;
		case PF_Cmd_USER_CHANGED_PARAM:
			err = HandleChangedParam(	in_data,
										out_data,
										params,
										output,
										reinterpret_cast<PF_UserChangedParamExtra*>(pExtra));
			break;
	}
	return err;
}

static PF_Err
About (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_SPRINTF(	out_data->return_msg,
				"%s, v%d.%d\r%s",
				NAME,
				MAJOR_VERSION,
				MINOR_VERSION,
				DESCRIPTION);

	return PF_Err_NONE;
}

static PF_Err
GlobalSetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err	err				= PF_Err_NONE;

	out_data->my_version	= PF_VERSION(	MAJOR_VERSION,
											MINOR_VERSION,
											BUG_VERSION,
											STAGE_VERSION,
											BUILD_VERSION);

	const int nSizeOfInstanceData=sizeof(SFreeFrameHost_InstanceData);
	PF_Handle hInstanceData=PF_NEW_HANDLE(nSizeOfInstanceData);

	out_data->global_data=hInstanceData;

	SFreeFrameHost_InstanceData* pInstanceData=
		(SFreeFrameHost_InstanceData*)(*hInstanceData);
	Pete_ZeroMemory(pInstanceData,nSizeOfInstanceData);

	pInstanceData->nSuperSampleSize=1;

	FreeFrame_LoadAllPlugins(pInstanceData);

	return err;
}

static PF_Err
ParamsSetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err			err = PF_Err_NONE;
	PF_ParamDef		def;

	SFreeFrameHost_InstanceData* pInstanceData=
		(SFreeFrameHost_InstanceData*)(*out_data->global_data);

	char* pPopupString=FreeFrame_BuildPopupString(pInstanceData);

	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	AEFX_CLR_STRUCT(def);
	def.flags=(PF_ParamFlag_SUPERVISE|
		PF_ParamFlag_CANNOT_TIME_VARY);

	PF_ADD_POPUP(	"Effect",
					pInstanceData->nEffectsCount,
					1,
					pPopupString,
					POPUP_EFFECT_ID);

	AEFX_CLR_STRUCT(def);
	def.flags=(PF_ParamFlag_SUPERVISE|
		PF_ParamFlag_CANNOT_TIME_VARY);

	PF_ADD_POPUP(	"Super-Sample",
					4,
					1,
					"1x|4x|9x|16x",
					POPUP_SUPERSAMPLE_ID);

	int nNumParameters;
	if (pFreeFrameMain!=NULL) {
		nNumParameters=(int)pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);
	} else {
		nNumParameters=0;
	}

	int nCount;
	for (nCount=0; nCount<nNumParameters; nCount+=1) {

		static void* pCastCount;
		pCastCount=reinterpret_cast<void*>(nCount);

		char* pName=(char*)(pEffectData->m_pMainFunc(FF_GETPARAMETERNAME,pCastCount,0));

		bool bNullCharFound=false;
		int nCharCount;
		for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
			bNullCharFound=(pName[nCharCount]=='\0');
		}
		if (!bNullCharFound) {
			pName[15]='\0'; // Pete- Getting unterminated strings returned
		}

		if (pName==NULL) {
			pName="<NULL>";
		}

		static U32 ResultAsU32;
		ResultAsU32=(U32)pEffectData->m_pMainFunc(FF_GETPARAMETERDEFAULT,pCastCount,0);
		const float DefaultValue=*((float*)&ResultAsU32);

		const int nParamID=(SLIDER_PARAM0_ID+nCount);

		AEFX_CLR_STRUCT(def);
		def.flags&=~PF_ParamFlag_COLLAPSE_TWIRLY;
		PF_ADD_SLIDER(	pName,
						0.0f,
						1000.0f,
						0.0f,
						1000.0f,
						DefaultValue*1000.0f,
						nParamID);
	}

	for (nCount=nNumParameters; nCount<nFreeFrameParamCount; nCount+=1) {

		const int nParamID=(SLIDER_PARAM0_ID+nCount);

		AEFX_CLR_STRUCT(def);
		def.flags|=PF_ParamFlag_COLLAPSE_TWIRLY;
		def.ui_flags=PF_PUI_DISABLED;
		if (nCount==nNumParameters) {
			def.ui_flags|=PF_PUI_ECW_SEPARATOR;
		}

		PF_ADD_SLIDER("Unused",
			0.0f,
			1000.0f,
			0.0f,
			1000.0f,
			500.0f,
			nParamID);

	}

	out_data->num_params = FILTER_NUM_PARAMS;

	return err;
}

static PF_Err SequenceSetup (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{
	const int nWidth=in_data->width;
	const int nHeight=in_data->height;
	const int nScreenPixelCount=(nWidth*nHeight);
	const int nScreenByteCount=(nScreenPixelCount*sizeof(U32));

	const int nTotalByteCount=(nScreenByteCount*16);

	out_data->sequence_data = PF_NEW_HANDLE(nTotalByteCount);
	if (!out_data->sequence_data) {
		return PF_Err_OUT_OF_MEMORY;
	}

	SFreeFrameHost_InstanceData* pInstanceData=
		(SFreeFrameHost_InstanceData*)(*out_data->global_data);

	pInstanceData->nSourceWidth=nWidth;
	pInstanceData->nSourceHeight=nHeight;

	FreeFrame_InitEffect(pInstanceData);

	return PF_Err_NONE;

}

void FreeFrame_InitEffect(SFreeFrameHost_InstanceData* pInstanceData) {

	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	VideoInfoStruct* pVideoInfo=&pInstanceData->VideoInfo;

	const int nSourceWidth=pInstanceData->nSourceWidth;
	const int nSourceHeight=pInstanceData->nSourceHeight;

	const int nSuperSampleSize=pInstanceData->nSuperSampleSize;

	pVideoInfo->frameWidth=(nSourceWidth*nSuperSampleSize);
	pVideoInfo->frameHeight=(nSourceHeight*nSuperSampleSize);

	if (pFreeFrameMain!=NULL) {

		pFreeFrameMain(FF_DEINITIALISE,NULL,0);

		if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)2,0)==FF_TRUE) {
			pVideoInfo->bitDepth=2;
		} else if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)1,0)==FF_TRUE) {
			pVideoInfo->bitDepth=1;
		} else if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)0,0)==FF_TRUE) {
			pVideoInfo->bitDepth=0;
		} else {
			assert(FALSE);
			pVideoInfo->bitDepth=2;
		}

		pFreeFrameMain(FF_INITIALISE,pVideoInfo,0);

	}

}

void FreeFrame_DeInitEffect(SFreeFrameHost_InstanceData* pInstanceData) {

	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	if (pFreeFrameMain!=NULL) {

		pFreeFrameMain(FF_DEINITIALISE,NULL,0);

	}

}

void FreeFrame_InitEffectParams(PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output,
								SFreeFrameHost_InstanceData* pInstanceData,
								bool bValueChangeAllowed) {

	PF_ParamUtilsSuite1* pus=NULL;
	PF_Err err=AEFX_AcquireSuite(in_data,
		out_data,
		kPFParamUtilsSuite,
		kPFParamUtilsSuiteVersion1,
		"Couldn't load suite.",
		(void**)&pus);

	PF_ParamDef			def;

	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	if (!err && pus ) {

		int nNumParameters;
		if (pFreeFrameMain!=NULL) {
			nNumParameters=(int)pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);
		} else {
			nNumParameters=0;
		}

		int nCount;
		for (nCount=nNumParameters; nCount<nFreeFrameParamCount; nCount+=1) {

			const int nParamID=(SLIDER_PARAM0_ID+nCount);
			PF_ParamDef* pParamDef=params[nParamID];

			def=*pParamDef;

			def.flags|=PF_ParamFlag_COLLAPSE_TWIRLY;
			def.ui_flags=PF_PUI_DISABLED;
			if (nCount==nNumParameters) {
				def.ui_flags|=PF_PUI_ECW_SEPARATOR;
			}
			PF_STRCPY(def.name,"Unused");

			err=(pus->PF_UpdateParamUI)(
				in_data->effect_ref,
				nParamID,
				&def);

		}

		for (nCount=0; nCount<nNumParameters; nCount+=1) {

			static void* pCastCount;
			pCastCount=reinterpret_cast<void*>(nCount);

			char* pName=(char*)(pEffectData->m_pMainFunc(FF_GETPARAMETERNAME,pCastCount,0));

			bool bNullCharFound=false;
			int nCharCount;
			for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
				bNullCharFound=(pName[nCharCount]=='\0');
			}
			if (!bNullCharFound) {
				pName[15]='\0'; // Pete- Getting unterminated strings returned
			}

			if (pName==NULL) {
				pName="<NULL>";
			}

			static U32 ResultAsU32;
			ResultAsU32=(U32)pEffectData->m_pMainFunc(FF_GETPARAMETERDEFAULT,pCastCount,0);
			const float DefaultValue=*((float*)&ResultAsU32);

			const int nParamID=(SLIDER_PARAM0_ID+nCount);
			PF_ParamDef* pParamDef=params[nParamID];

			def=*pParamDef;
			PF_STRCPY(def.name,pName);
			def.flags&=~PF_ParamFlag_COLLAPSE_TWIRLY;
			def.ui_flags&=~(PF_PUI_DISABLED|PF_PUI_ECW_SEPARATOR);

			err=(pus->PF_UpdateParamUI)(
				in_data->effect_ref,
				nParamID,
				&def);

			if (bValueChangeAllowed) {

				pParamDef->u.sd.value=(DefaultValue*1000.0f);
				pParamDef->uu.change_flags=PF_ChangeFlag_CHANGED_VALUE;

			}

		}

	}

}

static PF_Err SequenceSetdown (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{
	if (in_data->sequence_data) {
		PF_DISPOSE_HANDLE(in_data->sequence_data);
		out_data->sequence_data = NULL;
	}

	SFreeFrameHost_InstanceData* pInstanceData=
		(SFreeFrameHost_InstanceData*)(*out_data->global_data);

	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	if (pFreeFrameMain!=NULL) {

		pFreeFrameMain(FF_DEINITIALISE,NULL,0);

	}

	return PF_Err_NONE;
}


static PF_Err SequenceResetup (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{
	if (!in_data->sequence_data) {
		return SequenceSetup(in_data, out_data, params, output);
	}
	return PF_Err_NONE;
}

static PF_Err
HandleChangedParam(
	PF_InData					*in_data,
	PF_OutData					*out_data,
	PF_ParamDef					*params[],
	PF_LayerDef					*output,
	PF_UserChangedParamExtra	*extra) {

	PF_Err				err					= PF_Err_NONE;

	SFreeFrameHost_InstanceData* pInstanceData=
		(SFreeFrameHost_InstanceData*)(*out_data->global_data);

	const int nOldEffect=pInstanceData->nSelectedEffect;
	const int nNewEffect=(params[FILTER_EFFECT]->u.bd.value-1);

	if (nNewEffect!=nOldEffect) {
		FreeFrame_DeInitEffect(pInstanceData);
	}

	pInstanceData->nSelectedEffect=nNewEffect;

	pInstanceData->nSuperSampleSize=(params[FILTER_SUPERSAMPLE]->u.bd.value);

	FreeFrame_InitEffect(pInstanceData);

	if (nNewEffect!=nOldEffect) {
		FreeFrame_InitEffectParams(in_data,out_data,params,output,pInstanceData,true);
	}

	return err;

}

void Pete_SuperSampleUp(PF_World* pSource,PF_World* pSuperSampledSource,int nSuperSampleSize) {

	const int nSourceWidth=pSource->width;
	const int nSourceHeight=pSource->height;

	const int nSSWidth=pSuperSampledSource->width;
	const int nSSHeight=pSuperSampledSource->height;

	if ((nSSWidth==0)||(nSSHeight==0)) {
		return;
	}

	const float ScaleX=nSourceWidth/(float)nSSWidth;
	const float ScaleY=nSourceHeight/(float)nSSHeight;

	char* pSourceData=(char*)(pSource->data);
	const int nSourceRowBytes=pSource->rowbytes;

	char* pSuperSampleData=(char*)(pSuperSampledSource->data);
	const int nSuperSampleRowBytes=pSuperSampledSource->rowbytes;

	int nY;
	for (nY=0; nY<nSSHeight; nY+=1) {

		const int nSourceY=(nY*ScaleY);

		PF_Pixel* pSourceRow=(PF_Pixel*)
			(pSourceData+
			(nSourceY*nSourceRowBytes));

		PF_Pixel* pSuperSampleRow=(PF_Pixel*)
			(pSuperSampleData+
			(nY*nSuperSampleRowBytes));

		int nX;
		for (nX=0; nX<nSSWidth; nX+=1) {

			const int nSourceX=(nX*ScaleX);

			PF_Pixel* pCurrentSource=pSourceRow+nSourceX;
			PF_Pixel SourceColour=*pCurrentSource;

			PF_Pixel* pCurrentSuperSample=pSuperSampleRow+nX;
			*pCurrentSuperSample=SourceColour;

		}

	}

}

void Pete_SuperSampleDown(PF_World* pSuperSampledOutput,PF_World* pOutput,int nSuperSampleSize) {

	const int nOutputWidth=pOutput->width;
	const int nOutputHeight=pOutput->height;

	const int nSSWidth=pSuperSampledOutput->width;
	const int nSSHeight=pSuperSampledOutput->height;

	if ((nOutputWidth==0)||(nOutputHeight==0)) {
		return;
	}

	const float ScaleX=nSSWidth/(float)nOutputWidth;
	const float ScaleY=nSSHeight/(float)nOutputHeight;

	char* pOutputData=(char*)(pOutput->data);
	const int nOutputRowBytes=pOutput->rowbytes;

	char* pSuperSampleData=(char*)(pSuperSampledOutput->data);
	const int nSuperSampleRowBytes=pSuperSampledOutput->rowbytes;

	int nY;
	for (nY=0; nY<nOutputHeight; nY+=1) {

		PF_Pixel* pOutputRow=(PF_Pixel*)
			(pOutputData+
			(nY*nOutputRowBytes));

		int nX;
		for (nX=0; nX<nOutputWidth; nX+=1) {

			int nTotalRed=0;
			int nTotalGreen=0;
			int nTotalBlue=0;
			int nTotalAlpha=0;

			int nSuperSampleCount=0;

			const int nSuperSampleYStart=(int)(nY*ScaleY);
			const int nSuperSampleYEnd=(int)((nY+1)*ScaleY);

			int nSuperSampleY;
			for (nSuperSampleY=nSuperSampleYStart; nSuperSampleY<nSuperSampleYEnd; nSuperSampleY+=1) {

				PF_Pixel* pSuperSampleRow=(PF_Pixel*)
					(pSuperSampleData+
					(nSuperSampleY*nSuperSampleRowBytes));

				const int nSuperSampleXStart=(int)(nX*ScaleX);
				const int nSuperSampleXEnd=(int)((nX+1)*ScaleX);

				int nSuperSampleX;
				for (nSuperSampleX=nSuperSampleXStart; nSuperSampleX<nSuperSampleXEnd; nSuperSampleX+=1) {

					PF_Pixel* pCurrentSuperSample=pSuperSampleRow+nSuperSampleX;
					PF_Pixel SuperSampleColour=*pCurrentSuperSample;

					nTotalRed+=SuperSampleColour.red;
					nTotalGreen+=SuperSampleColour.green;
					nTotalBlue+=SuperSampleColour.blue;
					nTotalAlpha+=SuperSampleColour.alpha;

					nSuperSampleCount+=1;

				}

			}

			if (nSuperSampleCount==0) {
				nSuperSampleCount=1;
			}

			const int nAverageRed=(nTotalRed/nSuperSampleCount);
			const int nAverageGreen=(nTotalGreen/nSuperSampleCount);
			const int nAverageBlue=(nTotalBlue/nSuperSampleCount);
			const int nAverageAlpha=(nTotalAlpha/nSuperSampleCount);

			PF_Pixel OutputColour;
			OutputColour.red=nAverageRed;
			OutputColour.green=nAverageGreen;
			OutputColour.blue=nAverageBlue;
			OutputColour.alpha=nAverageAlpha;

			PF_Pixel* pCurrentOutput=pOutputRow+nX;

			*pCurrentOutput=OutputColour;

		}

	}


}

static PF_Err Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err				err		= PF_Err_NONE,
						err2	= PF_Err_NONE;

	FilterInfo			fi;
	short				lines	= 0;

	// Suites
	PF_Iterate8Suite1		*isP	= NULL;
	PF_WorldTransformSuite1	*wtsP	= NULL;
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	AEFX_CLR_STRUCT(fi);

	err = AEFX_AcquireSuite(in_data,
							out_data,
							kPFIterate8Suite,
							kPFIterate8SuiteVersion1,
							"Couldn't load Iterate Suite",
							reinterpret_cast<void**>(&isP));
	if(!err)
	{
		err = AEFX_AcquireSuite(in_data,
								out_data,
								kPFWorldTransformSuite,
								kPFWorldTransformSuiteVersion1,
								"Couldn't load World Transform Suite",
								reinterpret_cast<void**>(&wtsP));
	}

	if(!err && isP && wtsP)
	{

		SFreeFrameHost_InstanceData* pInstanceData=
			(SFreeFrameHost_InstanceData*)(*out_data->global_data);

		const float AspectRatio=
			(in_data->pixel_aspect_ratio.num)/
			(float)(in_data->pixel_aspect_ratio.den);

		const float DownSampleX=
			(in_data->downsample_x.num)/
			(float)(in_data->downsample_x.den);

		const float DownSampleY=
			(in_data->downsample_y.num)/
			(float)(in_data->downsample_y.den);

		const float AxisScaleX=1.0f/DownSampleX;
		const float AxisScaleY=AspectRatio/DownSampleY;

		PF_World* pOriginalSource=&params[FILTER_INPUT]->u.ld;
		PF_World* pOriginalOutput=output;

		PF_World SuperSampledSource;
		PF_World SuperSampledOutput;

		const int nSuperSampleSize=params[FILTER_SUPERSAMPLE]->u.bd.value;

		const int nSSBufferWidth=pInstanceData->VideoInfo.frameWidth;//(nSuperSampleSize*pOriginalOutput->width);
		const int nSSBufferHeight=pInstanceData->VideoInfo.frameHeight;//(nSuperSampleSize*pOriginalOutput->height);

		PF_World* pSource;
		PF_World* pOutput;
		if ((nSSBufferWidth!=pOriginalSource->width)||
			(nSSBufferHeight!=pOriginalSource->height)) {

			err = suites.PFWorldSuite()->new_world(NULL,
													nSSBufferWidth,
													nSSBufferHeight,
													PF_NewWorldFlag_NONE,
													&SuperSampledSource);

			if (!err) {

				err = suites.PFWorldSuite()->new_world(NULL,
														nSSBufferWidth,
														nSSBufferHeight,
														PF_NewWorldFlag_NONE,
														&SuperSampledOutput);

			}

			if ((err)||
				(SuperSampledSource.data==NULL)||
				(SuperSampledOutput.data==NULL)) {

				PF_STRCPY(out_data->return_msg, "MetaImage: Couldn't allocate super sample buffer, out of memory?");
				out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
				err = PF_Err_INTERNAL_STRUCT_DAMAGED;

				return err;

			}

			Pete_SuperSampleUp(pOriginalSource,&SuperSampledSource,nSuperSampleSize);

			pSource=&SuperSampledSource;
			pOutput=&SuperSampledOutput;

		} else {

			pSource=pOriginalSource;
			pOutput=pOriginalOutput;

		}

		const int nWidth=pSource->width;
		const int nHeight=pSource->height;

		U32* pSourceData=(U32*)pSource->data;
		const int nSourceRowBytes=pSource->rowbytes;

		U32* pOutputData=(U32*)pOutput->data;
		const int nOutputRowBytes=pOutput->rowbytes;

		const int nSelectedEffect=pInstanceData->nSelectedEffect;
		SFreeFrameHost_EffectData* pEffectData=
			&pInstanceData->m_Effects[nSelectedEffect];

		FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

		VideoInfoStruct* pVideoInfo=&pInstanceData->VideoInfo;

		if (pFreeFrameMain==NULL) {
			return PF_Err_INTERNAL_STRUCT_DAMAGED;
		}

		int nNumParameters;
		if (pFreeFrameMain!=NULL) {
			nNumParameters=(int)pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);
		} else {
			nNumParameters=0;
		}

		int nCount;
		for (nCount=0; nCount<nNumParameters; nCount+=1) {

			SetParameterStruct ArgStruct;
			ArgStruct.index=nCount;

			const int nParamID=(SLIDER_PARAM0_ID+nCount);
			ArgStruct.value=(float)(params[nParamID]->u.sd.value)/1000.0f;

			pFreeFrameMain(FF_SETPARAMETER,&ArgStruct,0);

		}

		const int nNumPixels=nWidth*nHeight;
		const int nNumBytes=(nNumPixels*sizeof(U32));

		const int nBitDepth=pVideoInfo->bitDepth;

		U32* pConversionBuffer=(U32*)*(out_data->sequence_data);
		int nConversionRowBytes;

		switch (nBitDepth) {

			case 2: {
				nConversionRowBytes=(4*nWidth);
			}break;

			case 1: {
				nConversionRowBytes=(3*nWidth);
			}break;

			case 0: {
				nConversionRowBytes=(2*nWidth);
			}break;

			default: {
				assert(FALSE);
				nConversionRowBytes=nWidth*sizeof(U32);
			}

		}

		int nY;
		for (nY=0; nY<nHeight; nY+=1) {

			U32* pSourceRowStart=(U32*)
				(((char*)pSourceData)+(nY*nSourceRowBytes));
			U32* pSourceRowEnd=(pSourceRowStart+nWidth);

			U32* pConvRowStart=(U32*)
				(((char*)pConversionBuffer)+(nY*nConversionRowBytes));

			U32* pCurrentSource=pSourceRowStart;
			U32* pCurrentConv=pConvRowStart;

			while (pCurrentSource<pSourceRowEnd) {

				U32 SourceColour=*pCurrentSource;

				U32 OutputColour=
					(((SourceColour>>24)&0xff)<<0)|
					(((SourceColour>>16)&0xff)<<8)|
					(((SourceColour>>8)&0xff)<<16)|
					(((SourceColour>>0)&0xff)<<24);

				*pCurrentConv=OutputColour;

				pCurrentConv+=1;
				pCurrentSource+=1;

			}

			switch (nBitDepth) {

				case 2: { // 32 bit
					// do nothing
				}break;

				case 1: { // 24 bit
					Pete_CopyAndConvert32BitTo24Bit(pConvRowStart,pConvRowStart,nWidth);
				}break;

				case 0: { // 16 bit
					Pete_CopyAndConvert32BitTo16Bit565(pConvRowStart,(U16*)pConvRowStart,nWidth);
				}break;

				default: {
					assert(FALSE);
				}break;

			}

		}

		pFreeFrameMain(FF_PROCESSFRAME,pConversionBuffer,0);

		for (nY=0; nY<nHeight; nY+=1) {

			U32* pOutputRowStart=(U32*)
				(((char*)pOutputData)+(nY*nOutputRowBytes));
			U32* pOutputRowEnd=(pOutputRowStart+nWidth);

			U32* pConvRowStart=(U32*)
				(((char*)pConversionBuffer)+(nY*nConversionRowBytes));

			switch (nBitDepth) {

				case 2: { // 32 bit
					memcpy(pOutputRowStart,pConvRowStart,(nWidth*sizeof(U32)));
				}break;

				case 1: { // 24 bit
					Pete_CopyAndConvert24BitTo32Bit(pConvRowStart,pOutputRowStart,nWidth);
				}break;

				case 0: { // 16 bit
					Pete_CopyAndConvert16Bit565To32Bit((U16*)pConvRowStart,pOutputRowStart,nWidth);
				}break;

				default: {
					assert(FALSE);
				}break;

			}

			U32* pCurrentOutput=pOutputRowStart;

			while (pCurrentOutput<pOutputRowEnd) {

				U32 SourceColour=*pCurrentOutput;

				U32 OutputColour=
					(((0xff000000>>24)&0xff)<<0)|
					(((SourceColour>>16)&0xff)<<8)|
					(((SourceColour>>8)&0xff)<<16)|
					(((SourceColour>>0)&0xff)<<24);

				*pCurrentOutput=OutputColour;

				pCurrentOutput+=1;

			}

		}

		if ((nSSBufferWidth!=pOriginalSource->width)||
			(nSSBufferHeight!=pOriginalSource->height)) {

			Pete_SuperSampleDown(&SuperSampledOutput,pOriginalOutput,nSuperSampleSize);

			(void)suites.PFWorldSuite()->dispose_world(NULL, &SuperSampledSource);
			(void)suites.PFWorldSuite()->dispose_world(NULL, &SuperSampledOutput);

		}

	}

	if(isP)
	{
		err2 = AEFX_ReleaseSuite(in_data,
								out_data,
								kPFIterate8Suite,
								kPFIterate8SuiteVersion1,
								"Couldn't unload Iterate Suite");
		if(!err && wtsP)
		{
			err2 = AEFX_ReleaseSuite(in_data,
									out_data,
									kPFWorldTransformSuite,
									kPFWorldTransformSuiteVersion1,
									"Couldn't unload World Transform Suite");
		}
	}

	if (err2 && !err)
	{
		err = err2;
	}
	return err;
}

char* FreeFrame_GetPluginDirPrefix(void) {

	HKEY AEKey;

	char* pRootName="SOFTWARE\\Adobe\\After Effects";

	LONG RegOpenKeyResult=RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,         // handle to open key
		pRootName,  // address of name of subkey to open
		0,   // reserved
		KEY_ALL_ACCESS, // security access mask
		&AEKey);

	if (RegOpenKeyResult!=ERROR_SUCCESS) {
		return "";
	}

	int nCount=0;
	LONG RegEnumKeyResult;

	do {

		char pSubKeyName[1024];
		DWORD nSubKeyNameLength=1024;

		char pClassName[1024];
		DWORD nClassNameLength=1024;

		FILETIME LastWriteTime;

		RegEnumKeyResult=RegEnumKeyEx(
			AEKey,          // handle to key to enumerate
			nCount,      // index of subkey to enumerate
			pSubKeyName,      // address of buffer for subkey name
			&nSubKeyNameLength,   // address for size of subkey buffer
			NULL, // reserved
			pClassName,     // address of buffer for class string
			&nClassNameLength,  // address for size of class buffer
			&LastWriteTime // address for time key last written to
		);

		char pSubKeyFullName[1024];
		sprintf(pSubKeyFullName,"%s\\%s",pRootName,pSubKeyName);

		HKEY SubKey;

		LONG RegOpenSubKeyResult=RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,         // handle to open key
			pSubKeyFullName,  // address of name of subkey to open
			0,   // reserved
			KEY_ALL_ACCESS, // security access mask
			&SubKey);

		if (RegOpenSubKeyResult!=ERROR_SUCCESS) {
			break;
		}

		DWORD ValueType;
		static char ValueString[1024];
		DWORD ValueStringLength=1024;

		LONG RegQueryValueResult=RegQueryValueEx(
			SubKey,           // handle to key to query
			"InstallPath",  // address of name of value to query
			NULL,  // reserved
			&ValueType,      // address of buffer for value type
			(BYTE*)ValueString,       // address of data buffer
			&ValueStringLength     // address of data buffer size
		);

		if (RegQueryValueResult==ERROR_SUCCESS) {
			strcat(ValueString,"plug-ins\\");
			return ValueString;
		}

		nCount+=1;

	} while (RegEnumKeyResult==ERROR_SUCCESS);

	return "";
}

void FreeFrame_LoadAllPlugins(SFreeFrameHost_InstanceData* pInstanceData) {

	char* pPluginDirPrefix=FreeFrame_GetPluginDirPrefix();

	FreeFrame_LoadPluginsFromDir(pInstanceData,pPluginDirPrefix,0);

}

void FreeFrame_LoadPluginsFromDir(SFreeFrameHost_InstanceData* pInstanceData,char* pCurrentDir,int nRecursionLevel) {

	char pSearchString[1024];

	strcpy(pSearchString,pCurrentDir);
	strcat(pSearchString,"*.dll");

	WIN32_FIND_DATA FindDataStruct;
	ZeroMemory(&FindDataStruct,sizeof(FindDataStruct));
	HANDLE FileHandle=FindFirstFile(pSearchString,&FindDataStruct);

	bool bFileFound=(FileHandle!=INVALID_HANDLE_VALUE);

	while (bFileFound) {

		int nEffectsLoaded=pInstanceData->nEffectsCount;

		SFreeFrameHost_EffectData* pCurrentEffect=
			&pInstanceData->m_Effects[nEffectsLoaded];

		char pFullFileName[MAX_PATH];
		strcpy(pFullFileName,pCurrentDir);
		strcat(pFullFileName,FindDataStruct.cFileName);

		const bool bEffectLoaded=
			FreeFrame_LoadPlugin(pCurrentEffect,pFullFileName);

		if (bEffectLoaded) {
			nEffectsLoaded+=1;
		}

		pInstanceData->nEffectsCount=nEffectsLoaded;

		bFileFound=FindNextFile(FileHandle,&FindDataStruct);

	}

	nRecursionLevel+=1;

	if (nRecursionLevel>=10) {
		return;
	}

	strcpy(pSearchString,pCurrentDir);
	strcat(pSearchString,"*.");

	ZeroMemory(&FindDataStruct,sizeof(FindDataStruct));
	FileHandle=FindFirstFile(pSearchString,&FindDataStruct);

	bFileFound=(FileHandle!=INVALID_HANDLE_VALUE);

	while (bFileFound) {

		int nFileNameLength=strlen(FindDataStruct.cFileName);
		if (FindDataStruct.cFileName[nFileNameLength-1]!='.') {

			char pFullFileName[MAX_PATH];
			strcpy(pFullFileName,pCurrentDir);
			strcat(pFullFileName,FindDataStruct.cFileName);
			strcat(pFullFileName,"\\");

			FreeFrame_LoadPluginsFromDir(pInstanceData,pFullFileName,nRecursionLevel);

		}

		bFileFound=FindNextFile(FileHandle,&FindDataStruct);

	}

}

bool FreeFrame_LoadPlugin(SFreeFrameHost_EffectData* pEffectData,char* pFileName) {

	bool bLoadSucceeded=false;

	pEffectData->m_hModuleHandle=LoadLibrary(pFileName);

	if (pEffectData->m_hModuleHandle!=INVALID_HANDLE_VALUE) {

		FF_Main_FuncPtr pFreeFrameMain=
			(FF_Main_FuncPtr)
			GetProcAddress(
			pEffectData->m_hModuleHandle,
			"plugMain");

		if (pFreeFrameMain!=NULL) {
			pEffectData->m_pMainFunc=pFreeFrameMain;
			bLoadSucceeded=true;
		} else {
			FreeLibrary(pEffectData->m_hModuleHandle);
		}

	}

	return bLoadSucceeded;

}

char* FreeFrame_GetEffectName(SFreeFrameHost_EffectData* pEffectData) {

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	if (pFreeFrameMain==NULL) {
		return "<NULL>";
	}

	PlugInfoStruct* pInfoStruct=(PlugInfoStruct*)pEffectData->m_pMainFunc(FF_GETINFO,NULL,0);

	if (pInfoStruct==NULL) {
		return "<PNULL>";
	}

	char* pPluginName=(char*)(&pInfoStruct->pluginName[0]);
	static char TerminatedName[MAX_PATH];

	int nCharCount=0;
	while ((pPluginName[nCharCount]!='\0')&&(nCharCount<16)) {
		TerminatedName[nCharCount]=pPluginName[nCharCount];
		nCharCount+=1;
	}
	TerminatedName[nCharCount]='\0';

	return TerminatedName;
}

char* FreeFrame_BuildPopupString(SFreeFrameHost_InstanceData* pInstanceData) {

	static char pPopupString[8192];

	const int nEffectsCount=pInstanceData->nEffectsCount;

	int nCount;
	for (nCount=0; nCount<nEffectsCount; nCount+=1) {

		SFreeFrameHost_EffectData* pEffectData=&pInstanceData->m_Effects[nCount];

		char* pEffectName=FreeFrame_GetEffectName(pEffectData);

		if (nCount==0) {
			strcpy(pPopupString,pEffectName);
		} else {
			sprintf(pPopupString,"%s|%s",pPopupString,pEffectName);
		}

	}

	if (nEffectsCount==0) {
		strcpy(pPopupString,"No FF dlls found");
	}

	return pPopupString;

}