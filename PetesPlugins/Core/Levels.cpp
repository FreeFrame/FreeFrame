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

#include "Levels.h"
#include "ChannelFunction.h"
#include "PeteHelpers.h"

void Pete_Levels_SetupCFSettings(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings);
void Pete_Levels_CalculateAutoLevels(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pSettings,U32* pSource);

static SPete_Parameter g_Parameters[]={
	{
		"Auto",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Uniform",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Allow Inversion",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Uniform Input Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Uniform Input Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Uniform Output Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Uniform Output Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Red Input Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Red Input Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Red Output Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Red Output Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Green Input Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Green Input Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Green Output Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Green Output Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Blue Input Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Blue Input Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Blue Output Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Blue Output Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Low Percentile",
		5.0f,
		0.0f,
		100.0f,
		PETE_PARAM_FLOAT
	},
	{
		"High Percentile",
		95.0f,
		0.0f,
		100.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Levels_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Levels_Init(SPete_Levels_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Levels_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Levels_DeInit(SPete_Levels_Data* pInstanceData) {

	// do nothing

}

void Pete_Levels_Render(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pSettings,U32* pSource,U32* pOutput) {

	SPete_ChannelFunction_Settings CFSettings;

	Pete_Levels_CalculateAutoLevels(pInstanceData,pSettings,pSource);

	Pete_Levels_SetupCFSettings(pInstanceData,pSettings,&CFSettings);

	Pete_ChannelFunction_Render(&CFSettings,pSource,pOutput);

}

void Pete_Levels_SetupCFSettings(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings) {

	const int cnFixedShift=16;
	const int cnFixedMult=(1<<cnFixedShift);
	const int cnFixedOne=1*cnFixedMult;

	if (pInSettings->m_DoUniform>0.0f) {

		const int nInputLow=static_cast<int>(pInSettings->m_UniformInputFloor);
		int nInputDelta=static_cast<int>(pInSettings->m_UniformInputCeiling-pInSettings->m_UniformInputFloor);

		const int nOutputLow=static_cast<int>(pInSettings->m_UniformOutputFloor);
		int nOutputDelta=static_cast<int>(pInSettings->m_UniformOutputCeiling-pInSettings->m_UniformOutputFloor);

		// avoid the possibility of divide-by-zeros
		if (pInSettings->m_DoAllowInversion>0.0f) {

			if (nInputDelta==0) {
				nInputDelta=1;
			}
			if (nOutputDelta==0) {
				nOutputDelta=1;
			}

			nInputDelta=GateInt(nInputDelta,-255,255);
			nOutputDelta=GateInt(nOutputDelta,-255,255);

		} else {

			nInputDelta=GateInt(nInputDelta,1,255);
			nOutputDelta=GateInt(nOutputDelta,1,255);

		}

		const int nRecipInputDelta=cnFixedOne/nInputDelta;

		int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
		int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
		int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

		int nCount;
		for (nCount=0; nCount<256; nCount+=1) {

			const int nSourceRed=nCount;
			const int nSourceGreen=nCount;
			const int nSourceBlue=nCount;

			const int nTempRed=(((nSourceRed-nInputLow)*256)*nRecipInputDelta)>>cnFixedShift;
			const int nTempGreen=(((nSourceGreen-nInputLow)*256)*nRecipInputDelta)>>cnFixedShift;
			const int nTempBlue=(((nSourceBlue-nInputLow)*256)*nRecipInputDelta)>>cnFixedShift;

			int nOutputRed=((nTempRed*nOutputDelta)/256)+nOutputLow;
			int nOutputGreen=((nTempGreen*nOutputDelta)/256)+nOutputLow;
			int nOutputBlue=((nTempBlue*nOutputDelta)/256)+nOutputLow;

			nOutputRed=GateInt(nOutputRed,0,255);
			nOutputGreen=GateInt(nOutputGreen,0,255);
			nOutputBlue=GateInt(nOutputBlue,0,255);

			pRedTable[nCount]=(nOutputRed<<SHIFT_RED);
			pGreenTable[nCount]=(nOutputGreen<<SHIFT_GREEN);
			pBlueTable[nCount]=(nOutputBlue<<SHIFT_BLUE);

		}

	} else {

		const int nRedInputLow=static_cast<int>(pInSettings->m_RedInputFloor);
		int nRedInputDelta=static_cast<int>(pInSettings->m_RedInputCeiling-pInSettings->m_RedInputFloor);

		const int nRedOutputLow=static_cast<int>(pInSettings->m_RedOutputFloor);
		int nRedOutputDelta=static_cast<int>(pInSettings->m_RedOutputCeiling-pInSettings->m_RedOutputFloor);

		const int nGreenInputLow=static_cast<int>(pInSettings->m_GreenInputFloor);
		int nGreenInputDelta=static_cast<int>(pInSettings->m_GreenInputCeiling-pInSettings->m_GreenInputFloor);

		const int nGreenOutputLow=static_cast<int>(pInSettings->m_GreenOutputFloor);
		int nGreenOutputDelta=static_cast<int>(pInSettings->m_GreenOutputCeiling-pInSettings->m_GreenOutputFloor);

		const int nBlueInputLow=static_cast<int>(pInSettings->m_BlueInputFloor);
		int nBlueInputDelta=static_cast<int>(pInSettings->m_BlueInputCeiling-pInSettings->m_BlueInputFloor);

		const int nBlueOutputLow=static_cast<int>(pInSettings->m_BlueOutputFloor);
		int nBlueOutputDelta=static_cast<int>(pInSettings->m_BlueOutputCeiling-pInSettings->m_BlueOutputFloor);

		// avoid the possibility of divide-by-zeros
		if (pInSettings->m_DoAllowInversion>0.0f) {

			if (nRedInputDelta==0) {
				nRedInputDelta=1;
			}
			if (nRedOutputDelta==0) {
				nRedOutputDelta=1;
			}

			if (nGreenInputDelta==0) {
				nGreenInputDelta=1;
			}
			if (nGreenOutputDelta==0) {
				nGreenOutputDelta=1;
			}

			if (nRedInputDelta==0) {
				nRedInputDelta=1;
			}
			if (nRedOutputDelta==0) {
				nRedOutputDelta=1;
			}

			nRedInputDelta=GateInt(nRedInputDelta,-255,255);
			nRedOutputDelta=GateInt(nRedOutputDelta,-255,255);

			nGreenInputDelta=GateInt(nGreenInputDelta,-255,255);
			nGreenOutputDelta=GateInt(nGreenOutputDelta,-255,255);

			nBlueInputDelta=GateInt(nBlueInputDelta,-255,255);
			nBlueOutputDelta=GateInt(nBlueOutputDelta,-255,255);

		} else {

			nRedInputDelta=GateInt(nRedInputDelta,1,255);
			nRedOutputDelta=GateInt(nRedOutputDelta,1,255);

			nGreenInputDelta=GateInt(nGreenInputDelta,1,255);
			nGreenOutputDelta=GateInt(nGreenOutputDelta,1,255);

			nBlueInputDelta=GateInt(nBlueInputDelta,1,255);
			nBlueOutputDelta=GateInt(nBlueOutputDelta,1,255);

		}

		const int nRedRecipInputDelta=cnFixedOne/nRedInputDelta;
		const int nGreenRecipInputDelta=cnFixedOne/nGreenInputDelta;
		const int nBlueRecipInputDelta=cnFixedOne/nBlueInputDelta;

		int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
		int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
		int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

		int nCount;
		for (nCount=0; nCount<256; nCount+=1) {

			const int nSourceRed=nCount;
			const int nSourceGreen=nCount;
			const int nSourceBlue=nCount;

			const int nTempRed=(((nSourceRed-nRedInputLow)*256)*nRedRecipInputDelta)>>cnFixedShift;
			const int nTempGreen=(((nSourceGreen-nGreenInputLow)*256)*nGreenRecipInputDelta)>>cnFixedShift;
			const int nTempBlue=(((nSourceBlue-nBlueInputLow)*256)*nBlueRecipInputDelta)>>cnFixedShift;

			int nOutputRed=((nTempRed*nRedOutputDelta)/256)+nRedOutputLow;
			int nOutputGreen=((nTempGreen*nGreenOutputDelta)/256)+nGreenOutputLow;
			int nOutputBlue=((nTempBlue*nBlueOutputDelta)/256)+nBlueOutputLow;

			nOutputRed=GateInt(nOutputRed,0,255);
			nOutputGreen=GateInt(nOutputGreen,0,255);
			nOutputBlue=GateInt(nOutputBlue,0,255);

			pRedTable[nCount]=(nOutputRed<<SHIFT_RED);
			pGreenTable[nCount]=(nOutputGreen<<SHIFT_GREEN);
			pBlueTable[nCount]=(nOutputBlue<<SHIFT_BLUE);


		}


	}

	pOutSettings->m_nWidth=pInstanceData->nWidth;
	pOutSettings->m_nHeight=pInstanceData->nHeight;

}

void Pete_Levels_CalculateAutoLevels(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pSettings,U32* pSource) {

	if (pSettings->m_DoAuto==0.0f) {
		return;
	}

	int	nRedHistogram[256];
	int	nGreenHistogram[256];
	int	nBlueHistogram[256];

	Pete_ZeroMemory(&nRedHistogram[0],256*sizeof(int));
	Pete_ZeroMemory(&nGreenHistogram[0],256*sizeof(int));
	Pete_ZeroMemory(&nBlueHistogram[0],256*sizeof(int));

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;
	
	U32* pCurrentSource=pSource;
	const U32* pSourceEnd=(pSource+nNumPixels);

	const int nSampleSpacing=8;

	while (pCurrentSource<pSourceEnd) {
		
		U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;
			
		while (pCurrentSource<pSourceLineEnd) {

			U32 SourceColour=*pCurrentSource;
			
			const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;

			nRedHistogram[nSourceRed]+=1;
			nGreenHistogram[nSourceGreen]+=1;
			nBlueHistogram[nSourceBlue]+=1;

			pCurrentSource+=nSampleSpacing;

		}

		pCurrentSource=
			pSourceLineStart+(nSampleSpacing*nWidth);

	}

	const int nSampleCount=
		(nWidth/nSampleSpacing)*
		(nHeight/nSampleSpacing);

	const int nStartThreshold=static_cast<int>((pSettings->m_LowPercentile*nSampleCount)/100.0f);
	const int nEndThreshold=static_cast<int>((pSettings->m_HighPercentile*nSampleCount)/100.0f);

	int nCurrentRedTotal;
	int nCurrentGreenTotal;
	int nCurrentBlueTotal;
	int nCurrentSlot;

	nCurrentRedTotal=0;
	nCurrentSlot=0;
	while ((nCurrentRedTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentRedTotal+=nRedHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nRedLow=(nCurrentSlot-1);

	nCurrentRedTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentRedTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentRedTotal-=nRedHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nRedHigh=(nCurrentSlot+1);

	nCurrentGreenTotal=0;
	nCurrentSlot=0;
	while ((nCurrentGreenTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentGreenTotal+=nGreenHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nGreenLow=(nCurrentSlot-1);

	nCurrentGreenTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentGreenTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentGreenTotal-=nGreenHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nGreenHigh=(nCurrentSlot+1);

	nCurrentBlueTotal=0;
	nCurrentSlot=0;
	while ((nCurrentBlueTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentBlueTotal+=nBlueHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nBlueLow=(nCurrentSlot-1);

	nCurrentBlueTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentBlueTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentBlueTotal-=nBlueHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nBlueHigh=(nCurrentSlot+1);

	pSettings->m_RedInputFloor=(float)(nRedLow);
	if (nRedLow!=nRedHigh){
		pSettings->m_RedInputCeiling=(float)(nRedHigh);
	} else if (nRedHigh<255) {
		pSettings->m_RedInputCeiling=(float)(nRedHigh+1);
	} else {
		pSettings->m_RedInputCeiling=(float)(nRedHigh-1);
	}

	pSettings->m_GreenInputFloor=(float)(nGreenLow);
	if (nGreenLow!=nGreenHigh){
		pSettings->m_GreenInputCeiling=(float)(nGreenHigh);
	} else if (nGreenHigh<255) {
		pSettings->m_GreenInputCeiling=(float)(nGreenHigh+1);
	} else {
		pSettings->m_GreenInputCeiling=(float)(nGreenHigh-1);
	}

	pSettings->m_BlueInputFloor=(float)(nBlueLow);
	if (nBlueLow!=nBlueHigh){
		pSettings->m_BlueInputCeiling=(float)(nBlueHigh);
	} else if (nBlueHigh<255) {
		pSettings->m_BlueInputCeiling=(float)(nBlueHigh+1);
	} else {
		pSettings->m_BlueInputCeiling=(float)(nBlueHigh-1);
	}

	int nLowLuminance = 
		((90 * nRedLow)+
		(115 * nGreenLow)+
		(51 * nBlueLow))/256;

	int nHighLuminance = 
		((90 * nRedHigh)+
		(115 * nGreenHigh)+
		(51 * nBlueHigh))/256;

	if (nStartThreshold<nEndThreshold) {
		if (nLowLuminance>=nHighLuminance) {
			nLowLuminance=GateInt(nHighLuminance-1,0,255);
		}
	}

	pSettings->m_UniformInputFloor=(float)(nLowLuminance);
	pSettings->m_UniformInputCeiling=(float)(nHighLuminance);

}
