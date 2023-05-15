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

#include "ColourSuppress.h"
#include "PeteHelpers.h"

#include "ColourConversion.h"

#include "math.h"

const int nPaletteShift=10;
const int cnPaletteEntries=(1<<10);

void Pete_ColourSuppress_SetupPalette(SPete_ColourSuppress_Data* pInstanceData,SPete_ColourSuppress_Settings* pSettings);
void Pete_ColourSuppress_SetupGradientPalette(SPete_ColourSuppress_Data* pInstanceData,SPete_ColourSuppress_Settings* pSettings);
void Pete_ColourSuppress_SetupSpectrumPalette(SPete_ColourSuppress_Data* pInstanceData,SPete_ColourSuppress_Settings* pSettings);

const int nParamScale=100;
static SPete_Parameter g_Parameters[]={
	{
		"Red Suppression",
		0.0f*nParamScale,
		0.0f*nParamScale,
		2.0f*nParamScale,
		PETE_PARAM_INT
	},
	{
		"Red Balance",
		0.0f*nParamScale,
		-1.0f*nParamScale,
		1.0f*nParamScale,
		PETE_PARAM_INT
	},
	{
		"Green Suppression",
		0.0f*nParamScale,
		0.0f*nParamScale,
		2.0f*nParamScale,
		PETE_PARAM_FLOAT
	},
	{
		"Green Balance",
		0.0f*nParamScale,
		-1.0f*nParamScale,
		1.0f*nParamScale,
		PETE_PARAM_INT
	},
	{
		"Blue Suppression",
		0.0f*nParamScale,
		0.0f*nParamScale,
		2.0f*nParamScale,
		PETE_PARAM_FLOAT
	},
	{
		"Blue Balance",
		0.0f*nParamScale,
		-1.0f*nParamScale,
		1.0f*nParamScale,
		PETE_PARAM_INT
	},
	{
		"Use Minimum",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_ColourSuppress_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_ColourSuppress_Init(SPete_ColourSuppress_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_ColourSuppress_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_ColourSuppress_DeInit(SPete_ColourSuppress_Data* pInstanceData) {

	// do nothing

}

void Pete_ColourSuppress_Render(SPete_ColourSuppress_Data* pInstanceData,SPete_ColourSuppress_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFPShift=16;
	const int nFPMult=(1<<nFPShift);


	const float SuppressionRed=
		(pSettings->m_SuppressionRed/nParamScale);

	const float SuppressionGreen=
		(pSettings->m_SuppressionGreen/nParamScale);

	const float SuppressionBlue=
		(pSettings->m_SuppressionBlue/nParamScale);


	const float BalanceRed=
		(pSettings->m_BalanceRed/nParamScale);

	const float BalanceGreen=
		(pSettings->m_BalanceGreen/nParamScale);

	const float BalanceBlue=
		(pSettings->m_BalanceBlue/nParamScale);


	const bool bUseMinimum=(pSettings->m_DoUseMinimum!=0.0f);

	const bool bRedPreservesGreen=(BalanceRed<0.0f);
	const bool bGreenPreservesRed=(BalanceGreen<0.0f);
	const bool bBluePreservesRed=(BalanceBlue<0.0f);

	const int nOneMinusLerpRed=(int)(SuppressionRed*nFPMult);
	const int nLerpValueRed=(nFPMult-nOneMinusLerpRed);

	const int nOneMinusLerpGreen=(int)(SuppressionGreen*nFPMult);
	const int nLerpValueGreen=(nFPMult-nOneMinusLerpGreen);

	const int nOneMinusLerpBlue=(int)(SuppressionBlue*nFPMult);
	const int nLerpValueBlue=(nFPMult-nOneMinusLerpBlue);

	const int nBalanceLerpValueRed=AbsInt(BalanceRed*nFPMult);
	const int nBalanceOneMinusLerpRed=(nFPMult-nBalanceLerpValueRed);

	const int nBalanceLerpValueGreen=AbsInt(BalanceGreen*nFPMult);
	const int nBalanceOneMinusLerpGreen=(nFPMult-nBalanceLerpValueGreen);

	const int nBalanceLerpValueBlue=AbsInt(BalanceBlue*nFPMult);
	const int nBalanceOneMinusLerpBlue=(nFPMult-nBalanceLerpValueBlue);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelsCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelsCount);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;

	while (pCurrentSource<pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;

		const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
		const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

		const bool bIsGreenLessThanBlue=(nSourceGreen<nSourceBlue);
		const bool bIsRedLessThanBlue=(nSourceRed<nSourceBlue);
		const bool bIsRedLessThanGreen=(nSourceRed<nSourceGreen);

		int nNonRedReference;
		int nNonGreenReference;
		int nNonBlueReference;

		if (bUseMinimum) {

			if (bIsGreenLessThanBlue) {

				if (bRedPreservesGreen) {
					nNonRedReference=nSourceGreen;
				} else {
					nNonRedReference=
						(nSourceGreen*nBalanceLerpValueRed)+
						(nSourceBlue*nBalanceOneMinusLerpRed);
				}

			} else {

				if (!bRedPreservesGreen) {
					nNonRedReference=nSourceBlue;
				} else {
					nNonRedReference=
						(nSourceBlue*nBalanceLerpValueRed)+
						(nSourceGreen*nBalanceOneMinusLerpRed);
				}

			}


			if (bIsRedLessThanBlue) {

				if (bGreenPreservesRed) {
					nNonGreenReference=nSourceRed;
				} else {
					nNonGreenReference=
						(nSourceRed*nBalanceLerpValueGreen)+
						(nSourceBlue*nBalanceOneMinusLerpGreen);
				}

			} else {

				if (!bGreenPreservesRed) {
					nNonGreenReference=nSourceBlue;
				} else {
					nNonGreenReference=
						(nSourceBlue*nBalanceLerpValueGreen)+
						(nSourceRed*nBalanceOneMinusLerpGreen);
				}

			}


			if (bIsGreenLessThanBlue) {

				if (bRedPreservesGreen) {
					nNonRedReference=nSourceGreen;
				} else {
					nNonRedReference=
						(nSourceGreen*nBalanceLerpValueRed)+
						(nSourceBlue*nBalanceOneMinusLerpRed);

				}

			} else {

				if (!bRedPreservesGreen) {
					nNonRedReference=nSourceBlue;
				} else {
					nNonRedReference=
						(nSourceBlue*nBalanceLerpValueRed)+
						(nSourceGreen*nBalanceOneMinusLerpRed);
				}

			}



		} else {

			nNonRedReference=MaximumInt(nSourceGreen,nSourceBlue);
			nNonGreenReference=MaximumInt(nSourceRed,nSourceBlue);
			nNonBlueReference=MaximumInt(nSourceRed,nSourceGreen);

		}

		int nOutputRed;
		if (nSourceRed>nNonRedReference) {

			const int nOutputRedFP=
				(nSourceRed*nLerpValueRed)+
				(nNonRedReference*nOneMinusLerpRed);

			nOutputRed=(nOutputRedFP>>nFPShift);
			nOutputRed=GateInt(nOutputRed,0,255);

		} else {

			nOutputRed=nSourceRed;

		}

		int nOutputGreen;
		if (nSourceGreen>nNonGreenReference) {

			const int nOutputGreenFP=
				(nSourceGreen*nLerpValueGreen)+
				(nNonGreenReference*nOneMinusLerpGreen);

			nOutputGreen=(nOutputGreenFP>>nFPShift);
			nOutputGreen=GateInt(nOutputGreen,0,255);

		} else {

			nOutputGreen=nSourceGreen;

		}

		int nOutputBlue;
		if (nSourceBlue>nNonBlueReference) {

			const int nOutputBlueFP=
				(nSourceBlue*nLerpValueBlue)+
				(nNonBlueReference*nOneMinusLerpBlue);

			nOutputBlue=(nOutputBlueFP>>nFPShift);
			nOutputBlue=GateInt(nOutputBlue,0,255);

		} else {

			nOutputBlue=nSourceBlue;

		}

		const int nOutputAlpha=nSourceAlpha;

		const U32 OutputColour=
			(nOutputRed<<SHIFT_RED)|
			(nOutputGreen<<SHIFT_GREEN)|
			(nOutputBlue<<SHIFT_BLUE)|
			(nOutputAlpha<<SHIFT_ALPHA);

		*pCurrentOutput=OutputColour;

		pCurrentSource+=1;
		pCurrentOutput+=1;

	}

}

