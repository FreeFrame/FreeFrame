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


#include "DeInterlace.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"PAL 800x600 Fix",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_DeInterlace_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_DeInterlace_Init(SPete_DeInterlace_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_DeInterlace_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_DeInterlace_DeInit(SPete_DeInterlace_Data* pInstanceData) {

	// do nothing

}

void Pete_DeInterlace_Render(SPete_DeInterlace_Data* pInstanceData,SPete_DeInterlace_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;

	const int nPalFixStride=25;
	int nPalFixCounter=0;

	bool bPalStretchFix;
	if (pSettings->m_DoPalStretchFix>0.0f) {
		bPalStretchFix=true;
	} else {
		bPalStretchFix=false;
	}

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {
		
		const U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		U32* pPrevSource=pCurrentSource-nWidth;

		if (bPalStretchFix&&(nPalFixCounter==nPalFixStride)) {

			pPrevSource-=nWidth;
			nPalFixCounter=0;

		}

		nPalFixCounter+=1;
		
		if (pPrevSource<pSource) {
			pPrevSource=pSource;
		}

		while (pCurrentSource!=pSourceLineEnd) {

			U32 SourceColour=*pCurrentSource;
			U32 PrevSourceColour=*pPrevSource;

			SourceColour&=0xfefefefe;
			PrevSourceColour&=0xfefefefe;

			U32 OutputColour=
				SourceColour+PrevSourceColour;

			OutputColour>>=1;

			*pCurrentOutput=OutputColour;

			pCurrentSource+=1;
			pPrevSource+=1;
			pCurrentOutput+=1;

		}

	}


}

