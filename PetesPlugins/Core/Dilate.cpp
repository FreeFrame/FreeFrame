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


#include "Dilate.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Erode",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Dilate_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Dilate_Init(SPete_Dilate_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Dilate_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Dilate_DeInit(SPete_Dilate_Data* pInstanceData) {

	// do nothing

}

void Pete_Dilate_Render(SPete_Dilate_Data* pInstanceData,SPete_Dilate_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	if (pSettings->m_DoErode>0.0f) {

		while (pCurrentSource!=pSourceEnd) {

			const U32* pSourceLineStart=pCurrentSource;
			const U32* pSourceLineEnd=pCurrentSource+nWidth;

			while (pCurrentSource!=pSourceLineEnd) {
				const U32 SourceColour=*pCurrentSource;

				U32* pUp=(pCurrentSource-nWidth);
				if (pUp<pSource) {
					pUp=pCurrentSource;
				}

				U32* pLeft=(pCurrentSource-1);
				if (pLeft<pSourceLineStart) {
					pLeft=pCurrentSource;
				}

				U32* pDown=(pCurrentSource+nWidth);
				if (pDown>=pSourceEnd) {
					pDown=pCurrentSource;
				}

				U32* pRight=(pCurrentSource+1);
				if (pRight>=pSourceLineEnd) {
					pRight=pCurrentSource;
				}

				const U32 UpColour=(*pUp);
				const U32 LeftColour=(*pLeft);
				const U32 DownColour=(*pDown);
				const U32 RightColour=(*pRight);

				U32 OutputColour=SourceColour;
				int nOutputLuminance=GetLuminance(SourceColour);

				const int nDownLuminance=GetLuminance(DownColour);
				if (nDownLuminance<nOutputLuminance) {
					OutputColour=DownColour;
					nOutputLuminance=nDownLuminance;
				}

				const int nRightLuminance=GetLuminance(RightColour);
				if (nRightLuminance<nOutputLuminance) {
					OutputColour=RightColour;
					nOutputLuminance=nRightLuminance;
				}

				const int nUpLuminance=GetLuminance(UpColour);
				if (nUpLuminance<nOutputLuminance) {
					OutputColour=UpColour;
					nOutputLuminance=nUpLuminance;
				}

				const int nLeftLuminance=GetLuminance(LeftColour);
				if (nLeftLuminance<nOutputLuminance) {
					OutputColour=LeftColour;
					nOutputLuminance=nLeftLuminance;
				}

				*pCurrentOutput=OutputColour;

				pCurrentSource+=1;
				pCurrentOutput+=1;

			}

		}

	} else {

		while (pCurrentSource!=pSourceEnd) {

			const U32* pSourceLineStart=pCurrentSource;
			const U32* pSourceLineEnd=pCurrentSource+nWidth;

			while (pCurrentSource!=pSourceLineEnd) {
				const U32 SourceColour=*pCurrentSource;

				U32* pUp=(pCurrentSource-nWidth);
				if (pUp<pSource) {
					pUp=pCurrentSource;
				}

				U32* pLeft=(pCurrentSource-1);
				if (pLeft<pSourceLineStart) {
					pLeft=pCurrentSource;
				}

				U32* pDown=(pCurrentSource+nWidth);
				if (pDown>=pSourceEnd) {
					pDown=pCurrentSource;
				}

				U32* pRight=(pCurrentSource+1);
				if (pRight>=pSourceLineEnd) {
					pRight=pCurrentSource;
				}

				const U32 UpColour=(*pUp);
				const U32 LeftColour=(*pLeft);
				const U32 DownColour=(*pDown);
				const U32 RightColour=(*pRight);

				U32 OutputColour=SourceColour;
				int nOutputLuminance=GetLuminance(SourceColour);

				const int nDownLuminance=GetLuminance(DownColour);
				if (nDownLuminance>nOutputLuminance) {
					OutputColour=DownColour;
					nOutputLuminance=nDownLuminance;
				}

				const int nRightLuminance=GetLuminance(RightColour);
				if (nRightLuminance>nOutputLuminance) {
					OutputColour=RightColour;
					nOutputLuminance=nRightLuminance;
				}

				const int nUpLuminance=GetLuminance(UpColour);
				if (nUpLuminance>nOutputLuminance) {
					OutputColour=UpColour;
					nOutputLuminance=nUpLuminance;
				}

				const int nLeftLuminance=GetLuminance(LeftColour);
				if (nLeftLuminance>nOutputLuminance) {
					OutputColour=LeftColour;
					nOutputLuminance=nLeftLuminance;
				}

				*pCurrentOutput=OutputColour;

				pCurrentSource+=1;
				pCurrentOutput+=1;

			}

		}

	}

}

