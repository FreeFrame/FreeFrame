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

#include "Posterize.h"
#include "ChannelFunction.h"
#include "PeteHelpers.h"

void Pete_Posterize_SetupCFSettings(SPete_Posterize_Data* pInstanceData,SPete_Posterize_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings);

static SPete_Parameter g_Parameters[]={
	{
		"Levels",
		8.0f,
		1.0f,
		31.0f,
		PETE_PARAM_INT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Posterize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Posterize_Init(SPete_Posterize_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Posterize_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Posterize_DeInit(SPete_Posterize_Data* pInstanceData) {

	// do nothing

}

void Pete_Posterize_Render(SPete_Posterize_Data* pInstanceData,SPete_Posterize_Settings* pSettings,U32* pSource,U32* pOutput) {

	SPete_ChannelFunction_Settings CFSettings;

	Pete_Posterize_SetupCFSettings(pInstanceData,pSettings,&CFSettings);

	Pete_ChannelFunction_Render(&CFSettings,pSource,pOutput);

}

void Pete_Posterize_SetupCFSettings(SPete_Posterize_Data* pInstanceData,SPete_Posterize_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings) {

	const float Levels=pInSettings->m_PosterizeLevels;
	int nInterval=static_cast<int>(255/Levels);
	const int nIntegralLevels=static_cast<int>(Levels-1);
	int nMaxOutput=(nInterval*nIntegralLevels);

	nInterval=GateInt(nInterval,1,255);
	nMaxOutput=GateInt(nMaxOutput,1,255);

	int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
	int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
	int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

	int nCount;
	for (nCount=0; nCount<nChannelFunction_Levels; nCount+=1) {

		int nOutput=nCount;
		nOutput/=nInterval;
		nOutput*=(255*nInterval);
		nOutput/=nMaxOutput;
		nOutput=GateInt(nOutput,0,255);

		pRedTable[nCount]=(nOutput<<SHIFT_RED);
		pGreenTable[nCount]=(nOutput<<SHIFT_GREEN);
		pBlueTable[nCount]=(nOutput<<SHIFT_BLUE);

	}

	pOutSettings->m_nWidth=pInstanceData->nWidth;
	pOutSettings->m_nHeight=pInstanceData->nHeight;

}
