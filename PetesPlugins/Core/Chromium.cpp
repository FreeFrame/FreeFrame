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

#include "Chromium.h"

#include <math.h>

#include "ChannelFunction.h"
#include "PeteHelpers.h"

const int nCosTablePi=512;
const int nCosTableTwoPi=(2*nCosTablePi);
const int nCosTableOne=512;
const int nCosTableMask=1023; // bitwise mask for modding index

static int g_nCosTable[nCosTableTwoPi];

void Pete_Chromium_SetupCFSettings(SPete_Chromium_Data* pInstanceData,SPete_Chromium_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings);
void SetupCosTable(void);

static SPete_Parameter g_Parameters[]={
	{
		"Edge1",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Edge2",
		128.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Tint Red",
		7.0f,
		0.0f,
		255.0f,
		PETE_PARAM_COLOUR_R
	},
	{
		"Tint Green",
		22.0f,
		0.0f,
		255.0f,
		PETE_PARAM_COLOUR_G
	},
	{
		"Tint Blue",
		30.0f,
		0.0f,
		255.0f,
		PETE_PARAM_COLOUR_B
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Chromium_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

inline int CosFromTable(int inAngle) {
	inAngle&=nCosTableMask;
	return g_nCosTable[inAngle];
}

int Pete_Chromium_Init(SPete_Chromium_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Chromium_DeInit(pInstanceData);

	SetupCosTable();

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Chromium_DeInit(SPete_Chromium_Data* pInstanceData) {

	// do nothing

}

void Pete_Chromium_Render(SPete_Chromium_Data* pInstanceData,SPete_Chromium_Settings* pSettings,U32* pSource,U32* pOutput) {

	SPete_ChannelFunction_Settings CFSettings;

	Pete_Chromium_SetupCFSettings(pInstanceData,pSettings,&CFSettings);

	Pete_ChannelFunction_Render(&CFSettings,pSource,pOutput);

}

void Pete_Chromium_SetupCFSettings(SPete_Chromium_Data* pInstanceData,SPete_Chromium_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings) {

	const int nEdge1=static_cast<int>(pInSettings->m_Edge1);
	const int nEdge2=static_cast<int>(pInSettings->m_Edge2);

	const int nRedTint=static_cast<int>(pInSettings->m_RedTint);
	const int nGreenTint=static_cast<int>(pInSettings->m_GreenTint);
	const int nBlueTint=static_cast<int>(pInSettings->m_BlueTint);

	int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
	int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
	int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

	int nCount;
	for (nCount=0; nCount<256; nCount+=1) {

		const int nSourceRed=nCount;
		const int nSourceGreen=nCount;
		const int nSourceBlue=nCount;

		unsigned int nOutputRed=AbsInt(
			CosFromTable(
				(nSourceRed+nEdge2)+
				((nSourceRed*nEdge1)/32))
			+nRedTint);

		unsigned int nOutputGreen=AbsInt(
			CosFromTable(
				(nSourceGreen+nEdge2)+
				((nSourceGreen*nEdge1)/32))
			+nGreenTint);

		unsigned int nOutputBlue=AbsInt(
			CosFromTable(
				(nSourceBlue+nEdge2)+
				((nSourceBlue*nEdge1)/32))
			+nBlueTint);

		nOutputRed=GateInt(nOutputRed,0,255);
		nOutputGreen=GateInt(nOutputGreen,0,255);
		nOutputBlue=GateInt(nOutputBlue,0,255);

		pRedTable[nCount]=(nOutputRed<<SHIFT_RED);
		pGreenTable[nCount]=(nOutputGreen<<SHIFT_GREEN);
		pBlueTable[nCount]=(nOutputBlue<<SHIFT_BLUE);

	}

	pOutSettings->m_nWidth=pInstanceData->nWidth;
	pOutSettings->m_nHeight=pInstanceData->nHeight;

}

void SetupCosTable(void) {

	int nAngle;
	for (nAngle=0; nAngle<nCosTableTwoPi; ++nAngle) {

		const float AngleInRadians=(static_cast<float>(nAngle)/nCosTablePi)*Pete_Pi;
		g_nCosTable[nAngle]=static_cast<int>(cos(AngleInRadians)*nCosTableOne);

	}

}
