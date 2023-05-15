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


#include "LumaMask.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Threshold Start",
		120.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Threshold End",
		132.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_LumaMask_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_LumaMask_Init(SPete_LumaMask_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_LumaMask_DeInit(pInstanceData);

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_LumaMask_DeInit(SPete_LumaMask_Data* pInstanceData) {
	// do nothing
}

void Pete_LumaMask_Render(SPete_LumaMask_Data* pInstanceData,SPete_LumaMask_Settings* pSettings,U32* pSourceA,U32* pSourceB,U32* pMask,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	const int nThresholdStart=static_cast<int>(pSettings->m_ThresholdStart);

	int nThresholdEnd=static_cast<int>(pSettings->m_ThresholdEnd);
	if (nThresholdEnd<nThresholdStart) {
		nThresholdEnd=nThresholdStart;
	}

	int nThresholdRange=(nThresholdEnd-nThresholdStart);
	nThresholdRange=GateInt(nThresholdRange,1,255);

	const int nThresholdMult=((255<<8)/nThresholdRange);

	U32* pCurrentSourceA=pSourceA;
	U32* pCurrentSourceB=pSourceB;
	U32* pCurrentMask=pMask;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	while (pCurrentOutput!=pOutputEnd) {

		const U32 MaskColour=*pCurrentMask;

		const int nMaskRed=(MaskColour>>SHIFT_RED)&0xff;
		const int nMaskGreen=(MaskColour>>SHIFT_GREEN)&0xff;
		const int nMaskBlue=(MaskColour>>SHIFT_BLUE)&0xff;

		const int nMaskLuminance=
			((nMaskRed*90)+
			(nMaskGreen*115)+
			(nMaskBlue*51))>>8;

		if (nMaskLuminance<nThresholdStart) {

			*pCurrentOutput=*pCurrentSourceB;

		} else if (nMaskLuminance>=nThresholdEnd) {

			*pCurrentOutput=*pCurrentSourceA;

		} else {

			const U32 SourceAColour=*pCurrentSourceA;

			const int nSourceARed=(SourceAColour>>SHIFT_RED)&0xff;
			const int nSourceAGreen=(SourceAColour>>SHIFT_GREEN)&0xff;
			const int nSourceABlue=(SourceAColour>>SHIFT_BLUE)&0xff;

			const U32 SourceBColour=*pCurrentSourceB;

			const int nSourceBRed=(SourceBColour>>SHIFT_RED)&0xff;
			const int nSourceBGreen=(SourceBColour>>SHIFT_GREEN)&0xff;
			const int nSourceBBlue=(SourceBColour>>SHIFT_BLUE)&0xff;

			const int nAlpha=
				(nMaskLuminance-nThresholdStart)*
				nThresholdMult;

			const int nOneMinusAlpha=((255<<8)-nAlpha);

			int nOutputRed=
				(nSourceARed*nAlpha)+
				(nSourceBRed*nOneMinusAlpha);
			nOutputRed>>=16;

			int nOutputGreen=
				(nSourceAGreen*nAlpha)+
				(nSourceBGreen*nOneMinusAlpha);
			nOutputGreen>>=16;

			int nOutputBlue=
				(nSourceABlue*nAlpha)+
				(nSourceBBlue*nOneMinusAlpha);
			nOutputBlue>>=16;

			const U32 OutputColour=
				(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE);

			*pCurrentOutput=OutputColour;

		}

		pCurrentOutput+=1;
		pCurrentSourceA+=1;
		pCurrentSourceB+=1;
		pCurrentMask+=1;

	}

}
