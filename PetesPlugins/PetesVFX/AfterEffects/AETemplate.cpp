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

#include "AETemplate.h"

#include "PeteHelpers.h"

typedef PF_Handle (*funcptr_AENewHandle)(long);
typedef void (*funcptr_AEDisposeHandle)(PF_Handle);

struct SAETemplate_InstanceData {
	PLUGIN_DATA m_PluginData;
	PLUGIN_SETTINGS m_PluginSettings;
	SPete_Parameter* m_pParameterData;
	int m_nParameterCount;
	int m_nSuperSampleSize;
	bool m_bIsSuperSampleAvailable;	
};

static funcptr_AENewHandle g_pfnAENewHandle=NULL;
static funcptr_AEDisposeHandle g_pfnAEDisposeHandle=NULL;

const int nSliderUnits=1000;
#define PETE_FLOAT_AS_INT_SLIDER_OFFSET (32) // Pete- Changed float sliders to integral to work with FCP, 
// Disk IDs of what used to be float params are offset by this, see AE SDK docs for how this prevents problems

static PF_Err SequenceSetup(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err SequenceSetdown(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err SequenceFlatten(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err SequenceResetup(PF_InData* in_data,PF_OutData* out_data,PF_ParamDef* params[],PF_LayerDef* output);
static PF_Err
HandleChangedParam(
	PF_InData					*in_data,
	PF_OutData					*out_data,
	PF_ParamDef					*params[],
	PF_LayerDef					*output,
	PF_UserChangedParamExtra	*extra);

void Pete_SuperSampleUp(PF_World* pSource,PF_World* pSuperSampledSource,int nSuperSampleSize);
void Pete_SuperSampleDown(PF_World* pSuperSampledOutput,PF_World* pOutput,int nSuperSampleSize);
int Pete_GetTransferModeFromPopupIndex(int nPopupIndex);
int Pete_ParamToSliderUnits(SPete_Parameter* pParam,float Input);
float Pete_SliderToParamUnits(SPete_Parameter* pParam,int nInput);
// Pete- FCP doesn't support the new/dispose_world util callbacks, so these versions
// emulate them with standard memory allocation
PF_Err Pete_NewWorld(int nWidth,int nHeight,PF_NewWorldFlags Flags,PF_World* poutWorld,SPete_MemHandle* poutHandle);
void Pete_DisposeWorld(SPete_MemHandle hHandle);

PF_Err 
main (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void* pExtra) {

	PF_Err		err = PF_Err_NONE;

	if (g_pfnAENewHandle==NULL) {
		g_pfnAENewHandle=in_data->utils->host_new_handle;
		g_pfnAEDisposeHandle=in_data->utils->host_dispose_handle;
	}

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
		case PF_Cmd_SEQUENCE_FLATTEN:
			err = SequenceFlatten(in_data,out_data,params,output);
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

	out_data->global_data=NULL;
	
	out_data->out_flags|=PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING;

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


	AEFX_CLR_STRUCT(def);
	def.flags=(PF_ParamFlag_SUPERVISE|
		PF_ParamFlag_CANNOT_TIME_VARY);

	PF_ADD_POPUP(	"Super-Sample", 
					4,
					1,
					"1x|4x|9x|16x",
					POPUP_SUPERSAMPLE_ID);

	int nParameterCount;
	SPete_Parameter* pParameterData;
	PLUGIN_GETPARAMETERS(&pParameterData,&nParameterCount);

	int nAEParametersAdded=2;

	int nCount;
	for (nCount=0; nCount<nParameterCount; nCount+=1) {

		SPete_Parameter* pCurrentParam=(pParameterData+nCount);
	
		AEFX_CLR_STRUCT(def);

		switch (pCurrentParam->Type) {

			case PETE_PARAM_FLOAT: {

				def.uu.id=(nAEParametersAdded+PETE_FLOAT_AS_INT_SLIDER_OFFSET);

				PF_ADD_SLIDER(
					pCurrentParam->pName,
					0,
					nSliderUnits,
					0,
					nSliderUnits,
					Pete_ParamToSliderUnits(pCurrentParam,pCurrentParam->Default),
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_INT: {

				PF_ADD_SLIDER(
					pCurrentParam->pName,
					pCurrentParam->Minimum,
					pCurrentParam->Maximum,
					pCurrentParam->Minimum,
					pCurrentParam->Maximum,
					pCurrentParam->Default,
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_BOOL: {

				PF_ADD_CHECKBOX(
					pCurrentParam->pName,
					"",
					(pCurrentParam->Default!=0.0f),
					0,
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_POS_X: {

				char pChoppedName[1024];
				strcpy(pChoppedName,pCurrentParam->pName);
				pChoppedName[strlen(pChoppedName)-1]='\0';

				SPete_Parameter* pParamY=(pCurrentParam+1);

				assert(nCount<(nParameterCount-1));

				PF_ADD_POINT(
					pChoppedName,
					((100<<16)*pCurrentParam->Default),
					((100<<16)*pParamY->Default),
					FALSE,
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_POS_Y: {
				// do nothing, handled above
			}break;

			case PETE_PARAM_ANGLE: {

				PF_ADD_ANGLE(
					pCurrentParam->pName,
					((pCurrentParam->Default*360.0f)/Pete_TwoPi),
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_COLOUR_R: {

				SPete_Parameter* pParamG=(pCurrentParam+1);
				SPete_Parameter* pParamB=(pCurrentParam+2);

				assert(nCount<(nParameterCount-2));

				PF_ADD_COLOR(
					pCurrentParam->pName,
					pCurrentParam->Default,
					pParamG->Default,
					pParamB->Default,
					nAEParametersAdded);

				nAEParametersAdded+=1;

			}break;

			case PETE_PARAM_COLOUR_G: {
				// do nothing, handled above
			}break;

			case PETE_PARAM_COLOUR_B: {
				// do nothing, handled above
			}break;

			default: {
				assert(false);
			}break;

		}


	}

	const int nCoreParameterEnd=nAEParametersAdded;

	PF_ADD_SLIDER(	"Opacity",
					0,100,
					0,100,
					100,
					nCoreParameterEnd+SLIDER_OPACITY_IDOFFSET);

	nAEParametersAdded+=1;

	PF_ADD_POPUP(	"Transfer Mode",
					18,1,
					"None|Dissolve|Add|Multiply|Screen|Overlay|Soft Light|Hard Light|"\
"Darken|Lighten|Difference|Hue|Saturation|Color|Luminosity|Dodge|Burn|Exclusion",
					nCoreParameterEnd+POPUP_TRANSFERMODE_IDOFFSET);

	nAEParametersAdded+=1;

	out_data->num_params = nAEParametersAdded;

	return err;
}

static PF_Err SequenceSetup (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{

	const int nWidth=in_data->width;
	const int nHeight=in_data->height;

	const int nSizeOfInstanceData=sizeof(SAETemplate_InstanceData);
	PF_Handle hInstanceData=PF_NEW_HANDLE(nSizeOfInstanceData);

	out_data->sequence_data=hInstanceData;
	
	SAETemplate_InstanceData* pInstanceData=
		(SAETemplate_InstanceData*)(*hInstanceData);
	Pete_ZeroMemory(pInstanceData,nSizeOfInstanceData);

	PLUGIN_GETPARAMETERS(&pInstanceData->m_pParameterData,&pInstanceData->m_nParameterCount);

	pInstanceData->m_nSuperSampleSize=1;

	PLUGIN_DATA* pPluginInstanceData=
		&pInstanceData->m_PluginData;
	
	PLUGIN_INIT(pPluginInstanceData,nWidth,nHeight);

	return PF_Err_NONE;

}

static PF_Err SequenceSetdown (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{
	SAETemplate_InstanceData* pInstanceData=
		(SAETemplate_InstanceData*)(*out_data->sequence_data);

	PLUGIN_DATA* pPluginInstanceData=
		&pInstanceData->m_PluginData;
	
	PLUGIN_DEINIT(pPluginInstanceData);
	
	if (in_data->sequence_data) {
		PF_DISPOSE_HANDLE(in_data->sequence_data);
		out_data->sequence_data = NULL;
	}

	return PF_Err_NONE;
}

static PF_Err SequenceFlatten (	PF_InData		*in_data,
								PF_OutData		*out_data,
								PF_ParamDef		*params[],
								PF_LayerDef		*output )
{

	return SequenceSetdown(in_data,out_data,params,output);

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

	const int nNewSuperSampleSize=(params[SUPERSAMPLE_PARAMINDEX]->u.bd.value);

	SAETemplate_InstanceData* pInstanceData=
		(SAETemplate_InstanceData*)(*out_data->sequence_data);

	if (nNewSuperSampleSize!=pInstanceData->m_nSuperSampleSize) {

		pInstanceData->m_nSuperSampleSize=nNewSuperSampleSize;

		PLUGIN_DATA* pPluginInstanceData=
			&pInstanceData->m_PluginData;
		
		PLUGIN_DEINIT(pPluginInstanceData);

		const int nWidth=(in_data->width*nNewSuperSampleSize);
		const int nHeight=(in_data->height*nNewSuperSampleSize);

		PLUGIN_INIT(pPluginInstanceData,nWidth,nHeight);

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

	PF_UtilCallbacks* pUtils=in_data->utils;

	SAETemplate_InstanceData* pInstanceData=
		(SAETemplate_InstanceData*)(*out_data->sequence_data);

	PLUGIN_DATA* pPluginInstanceData=
		&pInstanceData->m_PluginData;

	PLUGIN_SETTINGS* pPluginSettings=
		&pInstanceData->m_PluginSettings;

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
	
	PF_World* pOriginalSource=&params[SOURCELAYER_PARAMINDEX]->u.ld;
	PF_World* pOriginalOutput=output;

	PF_World SuperSampledSource;
	SPete_MemHandle hSuperSampledSourceMemory;
	PF_World SuperSampledOutput;
	SPete_MemHandle hSuperSampledOutputMemory;

	const int nSuperSampleSize=params[SUPERSAMPLE_PARAMINDEX]->u.bd.value;

	const int nParameterCount=pInstanceData->m_nParameterCount;
	SPete_Parameter* pParameterData=pInstanceData->m_pParameterData;

	int nCurrentAEParam=2;

	int nCount;
	for (nCount=0; nCount<nParameterCount; nCount+=1) {

		SPete_Parameter* pCurrentParam=(pParameterData+nCount);
		float* pCurrentPluginParam=((float*)pPluginSettings)+nCount;

		switch (pCurrentParam->Type) {

			case PETE_PARAM_FLOAT: {

				const int nSliderValue=(params[nCurrentAEParam]->u.sd.value);

				*pCurrentPluginParam=Pete_SliderToParamUnits(
					pCurrentParam,nSliderValue);

				nCurrentAEParam+=1;

			}break;

			case PETE_PARAM_INT: {

				*pCurrentPluginParam=(float)(params[nCurrentAEParam]->u.sd.value);

				nCurrentAEParam+=1;

			}break;

			case PETE_PARAM_BOOL: {

				*pCurrentPluginParam=(float)(params[nCurrentAEParam]->u.bd.value);

				nCurrentAEParam+=1;

			}break;

			case PETE_PARAM_POS_X: {

				PF_ParamDef* pPositionParam=params[nCurrentAEParam];
				const int nPositionX=pPositionParam->u.td.x_value;
				const int nPositionY=pPositionParam->u.td.y_value;
				pCurrentPluginParam[0]=(nPositionX)/(float)(pOriginalSource->width*(1<<16));
				pCurrentPluginParam[1]=(nPositionY)/(float)(pOriginalSource->height*(1<<16));
								   
				nCurrentAEParam+=1;

			}break;

			case PETE_PARAM_POS_Y: {
				// do nothing, handled in case above
			}break;

			case PETE_PARAM_COLOUR_R: {
				PF_Pixel AEColour=params[nCurrentAEParam]->u.cd.value;

				pCurrentPluginParam[0]=AEColour.red;
				pCurrentPluginParam[1]=AEColour.green;
				pCurrentPluginParam[2]=AEColour.blue;

				nCurrentAEParam+=1;
			}break;

			case PETE_PARAM_COLOUR_G: {
				// do nothing, handled in case above
			}break;
			
			case PETE_PARAM_COLOUR_B: {
				// do nothing, handled in case above
			}break;

			case PETE_PARAM_ANGLE: {

				*pCurrentPluginParam=(float)(params[nCurrentAEParam]->u.sd.value)*(Pete_TwoPi/(360.0f*(1<<16)));

				nCurrentAEParam+=1;

			}break;

			default: {
				assert(false);
			}break;

		}

	}


	const int nOpacity=(params[nCurrentAEParam+OPACITY_PARAMOFFSET]->u.bd.value*255)/100;
	const int nTransPopupIndex=params[nCurrentAEParam+TRANSFERMODE_PARAMOFFSET]->u.bd.value;
	const int nTransferMode=
		Pete_GetTransferModeFromPopupIndex(nTransPopupIndex);

	const bool bDoTransfer=
		((nTransferMode!=PF_Xfer_NONE)||
		(nOpacity!=255));

	PF_World OutputCopy;
	SPete_MemHandle hOutputCopyMemory;
	if (bDoTransfer) {

		err = Pete_NewWorld(
			output->width, 
			output->height, 
			PF_NewWorldFlag_NONE, 
			&OutputCopy,
			&hOutputCopyMemory);

		if (err) {
			return err;
		}

		pOriginalOutput=&OutputCopy;

	}

	const int nOriginalPluginWidth=pPluginInstanceData->nWidth;
	const int nOriginalPluginHeight=pPluginInstanceData->nHeight;

	const int nDesiredSSBufferWidth=
		(pOriginalSource->width*nSuperSampleSize);
	const int nDesiredSSBufferHeight=
		(pOriginalSource->height*nSuperSampleSize);

	if ((nOriginalPluginWidth!=nDesiredSSBufferWidth)||
		(nOriginalPluginHeight!=nDesiredSSBufferHeight)) {
		
		PLUGIN_DEINIT(pPluginInstanceData);
		PLUGIN_INIT(pPluginInstanceData,nDesiredSSBufferWidth,nDesiredSSBufferHeight);
		
	}		
	
	const int nSSBufferWidth=pPluginInstanceData->nWidth;
	const int nSSBufferHeight=pPluginInstanceData->nHeight;

	PF_World* pSource;
	PF_World* pOutput;
	if ((nSSBufferWidth!=pOriginalSource->width)||
		(nSSBufferHeight!=pOriginalSource->height)) {

		err = Pete_NewWorld(
			nSSBufferWidth, 
			nSSBufferHeight, 
			PF_NewWorldFlag_NONE, 
			&SuperSampledSource,
			&hSuperSampledSourceMemory);

		if (!err) {

			err = Pete_NewWorld(
				nSSBufferWidth, 
				nSSBufferHeight, 
				PF_NewWorldFlag_NONE, 
				&SuperSampledOutput,
				&hSuperSampledOutputMemory);

		}

		if ((err)||
			(SuperSampledSource.data==NULL)||
			(SuperSampledOutput.data==NULL)) {

			PF_STRCPY(out_data->return_msg, PLUGIN_NAMESTRING": Couldn't allocate super sample buffer");
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
	
	const int nScreenPixelCount=(nWidth*nHeight);
	const int nScreenByteCount=(nScreenPixelCount*sizeof(U32));

	SPete_MemHandle hConversionBuffer=Pete_NewHandle(nScreenByteCount*2);
	if ((hConversionBuffer==NULL)||(*hConversionBuffer==NULL)) {

		PF_STRCPY(out_data->return_msg, PLUGIN_NAMESTRING": Couldn't allocate conversion buffer");
		out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
		err = PF_Err_INTERNAL_STRUCT_DAMAGED;

		return err;

	}
	
	U32* pConversionSourceBuffer=(U32*)*(hConversionBuffer);
	U32* pConversionOutputBuffer=(U32*)*(hConversionBuffer);
	pConversionOutputBuffer+=(nWidth*nHeight);

	const int nConversionRowBytes=(4*nWidth);

	int nY;
	for (nY=0; nY<nHeight; nY+=1) {

		U32* pSourceRowStart=(U32*)
			(((char*)pSourceData)+(nY*nSourceRowBytes));
		U32* pSourceRowEnd=(pSourceRowStart+nWidth);

		U32* pConvRowStart=(U32*)
			(((char*)pConversionSourceBuffer)+(nY*nConversionRowBytes));

		U32* pCurrentSource=pSourceRowStart;
		U32* pCurrentConv=pConvRowStart;

		while (pCurrentSource<pSourceRowEnd) {

			U32 SourceColour=*pCurrentSource;

#ifndef PETE_MAC_OSX

			U32 OutputColour=
				(((SourceColour>>24)&0xff)<<0)|
				(((SourceColour>>16)&0xff)<<8)|
				(((SourceColour>>8)&0xff)<<16)|
				(((SourceColour>>0)&0xff)<<24);

#else // PETE_MAC_OSX

			U32 OutputColour=SourceColour;

#endif // PETE_MAC_OSX

			*pCurrentConv=OutputColour;

			pCurrentConv+=1;
			pCurrentSource+=1;
				
		}

	}
	
	PLUGIN_RENDER(pPluginInstanceData,pPluginSettings,pConversionSourceBuffer,pConversionOutputBuffer);

	for (nY=0; nY<nHeight; nY+=1) {

		U32* pSourceRowStart=(U32*)
			(((char*)pSourceData)+(nY*nSourceRowBytes));
			
		U32* pOutputRowStart=(U32*)
			(((char*)pOutputData)+(nY*nOutputRowBytes));
		U32* pOutputRowEnd=(pOutputRowStart+nWidth);

		U32* pConvRowStart=(U32*)
			(((char*)pConversionOutputBuffer)+(nY*nConversionRowBytes));

		memcpy(pOutputRowStart,pConvRowStart,(nWidth*sizeof(U32)));

		U32* pCurrentOutput=pOutputRowStart;
		U32* pCurrentOriginalSource=pSourceRowStart;

		while (pCurrentOutput<pOutputRowEnd) {

			U32 SourceColour=*pCurrentOutput;
			U32 OriginalSourceColour=*pCurrentOriginalSource;

#ifdef PLUGIN_HANDLES_ALPHA

#ifndef PETE_MAC_OSX

			U32 OriginalSourceAlpha=
				OriginalSourceColour&(0xff<<0);

			U32 OutputColour=
				(((SourceColour>>24)&0xff)<<0)|
				(((SourceColour>>16)&0xff)<<8)|
				(((SourceColour>>8)&0xff)<<16)|
				(((SourceColour>>0)&0xff)<<24);

#else // PETE_MAC_OSX

			U32 OutputColour=SourceColour;

#endif // PETE_MAC_OSX

#else // PLUGIN_HANDLES_ALPHA

#ifndef PETE_MAC_OSX

			U32 OriginalSourceAlpha=
				OriginalSourceColour&(0xff<<0);

			U32 OutputColour=
				OriginalSourceAlpha|
				(((SourceColour>>16)&0xff)<<8)|
				(((SourceColour>>8)&0xff)<<16)|
				(((SourceColour>>0)&0xff)<<24);

#else // PETE_MAC_OSX

			U32 OriginalSourceAlpha=
				OriginalSourceColour&(0xff<<24);

			U32 OutputColour=
				(SourceColour&0x00ffffff)|
				OriginalSourceAlpha;

#endif // PETE_MAC_OSX

#endif // PLUGIN_HANDLES_ALPHA

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;
			pCurrentOriginalSource+=1;
				
		}

	}

	Pete_FreeHandle(hConversionBuffer);

	if ((nSSBufferWidth!=pOriginalSource->width)||
		(nSSBufferHeight!=pOriginalSource->height)) {

		Pete_SuperSampleDown(&SuperSampledOutput,pOriginalOutput,nSuperSampleSize);

		Pete_DisposeWorld(hSuperSampledSourceMemory);
		Pete_DisposeWorld(hSuperSampledOutputMemory);

	}

	if (bDoTransfer) {

		if (!err) {

			if (nTransferMode==PF_Xfer_NONE) {

				PF_BLEND(pOriginalSource,&OutputCopy,(nOpacity<<8),output);

			} else {

				PF_COPY(pOriginalSource,output,NULL,NULL);
				PF_CompositeMode ModeData;
				ModeData.xfer=nTransferMode;
				ModeData.opacity=nOpacity;
				ModeData.rand_seed=in_data->current_time;
				ModeData.rgb_only=TRUE;

				PF_Rect SourceRect;
				SourceRect.left=0;
				SourceRect.top=0;
				SourceRect.right=output->width;
				SourceRect.bottom=output->height;

				PF_MaskWorld MaskWorld;
				MaskWorld.mask=*output;
				MaskWorld.what_is_mask=PF_MaskFlag_NONE;
				MaskWorld.offset.h=0;
				MaskWorld.offset.v=0;

				PF_TRANSFER_RECT(
					in_data->quality,
					PF_MF_Alpha_STRAIGHT,
					PF_Field_FRAME,
					&SourceRect,
					&OutputCopy,
					&ModeData,
					&MaskWorld,
					0,
					0,
					output);

			}

			Pete_DisposeWorld(hOutputCopyMemory);

		}

	}

	if (err2 && !err)
	{
		err = err2;
	}
	return err;
}

SPete_MemHandle Pete_NewHandle(int nBytesToAlloc) {

	return (*g_pfnAENewHandle)(nBytesToAlloc);

}

void Pete_FreeHandle(SPete_MemHandle InHandle) {

	(*g_pfnAEDisposeHandle)(InHandle);

}

void* Pete_LockHandle(SPete_MemHandle InHandle) {

	if (InHandle==NULL) {
		return NULL;
	} else {
		return *InHandle;
	}

}

void Pete_UnLockHandle(SPete_MemHandle InHandle) {

	//do nothing
	
}

int Pete_GetTransferModeFromPopupIndex(int nPopupIndex) {

	nPopupIndex-=1;

	int nResult;

	switch (nPopupIndex) {
		case 0:
			nResult=PF_Xfer_NONE;
		break;

		case 1:
			nResult=PF_Xfer_DISSOLVE;
		break;

		case 2:
			nResult=PF_Xfer_ADD;
		break;

		case 3:
			nResult=PF_Xfer_MULTIPLY;
		break;

		case 4:
			nResult=PF_Xfer_SCREEN;
		break;

		case 5:
			nResult=PF_Xfer_OVERLAY;
		break;

		case 6:
			nResult=PF_Xfer_SOFT_LIGHT;
		break;

		case 7:
			nResult=PF_Xfer_HARD_LIGHT;
		break;

		case 8:
			nResult=PF_Xfer_DARKEN;
		break;

		case 9:
			nResult=PF_Xfer_LIGHTEN;
		break;

		case 10:
			nResult=PF_Xfer_DIFFERENCE;
		break;

		case 11:
			nResult=PF_Xfer_HUE;
		break;

		case 12:
			nResult=PF_Xfer_SATURATION;
		break;

		case 13:
			nResult=PF_Xfer_COLOR;
		break;

		case 14:
			nResult=PF_Xfer_LUMINOSITY;
		break;

		case 15:
			nResult=PF_Xfer_COLOR_DODGE;
		break;

		case 16:
			nResult=PF_Xfer_COLOR_BURN;
		break;

		case 17:
			nResult=PF_Xfer_EXCLUSION;
		break;

		default:
			assert(false);
			nResult=PF_Xfer_NONE;
		break;

	}

	return nResult;

}

int Pete_ParamToSliderUnits(SPete_Parameter* pParam,float Input) {

	const float Minimum=pParam->Minimum;
	const float Maximum=pParam->Maximum;
	const float Range=(Maximum-Minimum);

	if (Range==0.0f) {
		return 0;
	}

	const float NormalisedInput=((Input-Minimum)/Range);

	const int nResult=(int)(NormalisedInput*nSliderUnits);

	return nResult;

}

float Pete_SliderToParamUnits(SPete_Parameter* pParam,int nInput) {

	const float Minimum=pParam->Minimum;
	const float Maximum=pParam->Maximum;
	const float Range=(Maximum-Minimum);

	const float NormalisedValue=(nInput/(float)nSliderUnits);

	const float Result=Minimum+(NormalisedValue*Range);

	return Result;

}

PF_Err Pete_NewWorld(int nWidth,int nHeight,PF_NewWorldFlags Flags,PF_World* poutWorld,SPete_MemHandle* poutHandle) {

	const int nScreenBufferSize=(nWidth*nHeight*sizeof(U32));

	*poutHandle=Pete_NewHandle(nScreenBufferSize);

	if ((*poutHandle==NULL)||(**poutHandle==NULL)) {
		return PF_Err_OUT_OF_MEMORY;
	}

	Pete_ZeroMemory(poutWorld,sizeof(*poutWorld));

	poutWorld->world_flags=PF_WorldFlag_WRITEABLE;
	poutWorld->data=(PF_PixelPtr)Pete_LockHandle(*poutHandle);

	if (Flags&PF_NewWorldFlag_CLEAR_PIXELS) {
		Pete_ZeroMemory(poutWorld->data,nScreenBufferSize);
	}

	poutWorld->rowbytes=(nWidth*sizeof(U32));
	poutWorld->width=nWidth;
	poutWorld->height=nHeight;

	poutWorld->extent_hint.left=0;
	poutWorld->extent_hint.top=0;
	poutWorld->extent_hint.right=nWidth;
	poutWorld->extent_hint.bottom=nHeight;

	return PF_Err_NONE;

}

void Pete_DisposeWorld(SPete_MemHandle hHandle) {

	Pete_FreeHandle(hHandle);

}
