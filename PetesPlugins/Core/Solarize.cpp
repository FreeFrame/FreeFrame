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

#include "Solarize.h"
#include "ChannelFunction.h"
#include "PeteHelpers.h"

void Pete_Solarize_SetupCFSettings(SPete_Solarize_Data* pInstanceData,SPete_Solarize_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings);

static SPete_Parameter g_Parameters[]={
	{
		"Threshold",
		128.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Start",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"End",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Limit",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Solarize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Solarize_Init(SPete_Solarize_Data* pInstanceData,int nWidth,int nHeight) {

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;
}

void Pete_Solarize_DeInit(SPete_Solarize_Data* pInstanceData) {

	// do nothing

}

void Pete_Solarize_Render(SPete_Solarize_Data* pInstanceData,SPete_Solarize_Settings* pSettings,U32* pSource,U32* pOutput) {

	SPete_ChannelFunction_Settings CFSettings;

	Pete_Solarize_SetupCFSettings(pInstanceData,pSettings,&CFSettings);

	Pete_ChannelFunction_Render(&CFSettings,pSource,pOutput);

}

void Pete_Solarize_SetupCFSettings(SPete_Solarize_Data* pInstanceData,SPete_Solarize_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings) {

	const int nThreshold=static_cast<int>(pInSettings->m_Threshold);
	const int nStart=static_cast<int>(pInSettings->m_Start);
	const int nEnd=static_cast<int>(pInSettings->m_End);
	const bool bDoLimit=(pInSettings->m_DoLimit>0.0f);
	const int nFloor=static_cast<int>(pInSettings->m_Floor);
	const int nCeiling=static_cast<int>(pInSettings->m_Ceiling);

	int nPeriod=(nEnd-nStart);
	if (nPeriod==0) {
		nPeriod=1;
	}

	int nUpLength=(nThreshold-nStart);
	if (nUpLength==0) {
		nUpLength=1;
	}

	int nDownLength=(nEnd-nThreshold);
	if (nDownLength==0) {
		nDownLength=1;
	}

	const int nHeightScale=(nCeiling-nFloor);

	int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
	int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
	int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

	int nCount;
	for (nCount=0; nCount<nChannelFunction_Levels; nCount+=1) {

		int nParam;

		if (bDoLimit) {

			nParam=nCount-nStart;
			nParam=GateInt(nParam,0,(nPeriod-1));

		} else {

			// Pete- magic code to get an always positive modulo
			nParam=nCount;
			nParam+=nChannelFunction_Levels;
			nParam-=nStart;
			nParam%=nPeriod;

		}

		int nOutput;
		if (nParam<nUpLength) {
			
			nOutput=(nParam*nHeightScale);
			nOutput/=nUpLength;
			nOutput+=nFloor;

		} else {

			nOutput=nDownLength-(nParam-nUpLength);
			nOutput*=nHeightScale;
			nOutput/=nDownLength;
			nOutput+=nFloor;

		}

		pRedTable[nCount]=(nOutput<<SHIFT_RED);
		pGreenTable[nCount]=(nOutput<<SHIFT_GREEN);
		pBlueTable[nCount]=(nOutput<<SHIFT_BLUE);

	}

	pOutSettings->m_nWidth=pInstanceData->nWidth;
	pOutSettings->m_nHeight=pInstanceData->nHeight;

}
