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


#include "LumaOffset.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Offset Scale",
		12.0f,
		-127.0f,
		127.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Line Gap",
		1.2f,
		0.0f,
		32.0f,
		PETE_PARAM_INT
	},
	{
		"Do Filled Lines",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Do Smooth Fill",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_LumaOffset_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_LumaOffset_Init(SPete_LumaOffset_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_LumaOffset_DeInit(pInstanceData);

	pInstanceData->hPreviousLineHeights=
		Pete_NewHandle(nWidth*sizeof(U32*));
	if (pInstanceData->hPreviousLineHeights==NULL) {
		Pete_LumaOffset_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_LumaOffset_DeInit(SPete_LumaOffset_Data* pInstanceData) {

	if (pInstanceData->hPreviousLineHeights!=NULL) {
		Pete_FreeHandle(pInstanceData->hPreviousLineHeights);
		pInstanceData->hPreviousLineHeights=NULL;
	}

}

void Pete_LumaOffset_Render(SPete_LumaOffset_Data* pInstanceData,SPete_LumaOffset_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);
	
	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;

	const int nOffsetScale=static_cast<int>(pSettings->m_OffsetScale);
	const int nLineGap=static_cast<int>(pSettings->m_LineGap);

	Pete_ZeroMemory(pOutput,(nNumPixels*sizeof(U32)));
	
	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	
	U32* pSourceEnd=(pSource+nNumPixels);
	U32* pOutputEnd=(pOutput+nNumPixels);

	const bool bDoFilledLines=(pSettings->m_DoFilledLines!=0.0f);

	if (!bDoFilledLines) {

		while (pCurrentSource<pSourceEnd) {
			
			const U32* pSourceLineEnd=pCurrentSource+nWidth;
				
			while (pCurrentSource!=pSourceLineEnd) {

				const U32 SourceColour=*pCurrentSource;

				int nLuma=GetLuminance(SourceColour);
				nLuma-=(128*255);
				
				const int nOffset=(nLuma*nOffsetScale)>>16;

				U32*const pOffsetOutput=
					pCurrentOutput+(nOffset*nWidth);

				if ((pOffsetOutput<pOutputEnd)&&
					(pOffsetOutput>=pOutput)) {

					*pOffsetOutput=SourceColour;

				}

				pCurrentSource+=1;
				pCurrentOutput+=1;
				
			}

			pCurrentSource+=(nWidth*nLineGap);
			pCurrentOutput+=(nWidth*nLineGap);

		}

	} else {

		const bool bDoSmoothFill=(pSettings->m_DoSmoothFill!=0.0f);

		U32** ppPreviousLineHeights=(U32**)Pete_LockHandle(pInstanceData->hPreviousLineHeights);
		if (ppPreviousLineHeights==NULL) {
			return;
		}

		Pete_ZeroMemory(ppPreviousLineHeights,(nWidth*sizeof(U32*)));

		int nCurrentY=0;
		while (pCurrentSource<pSourceEnd) {
			
			const U32* pSourceLineEnd=pCurrentSource+nWidth;
				
			U32** ppCurrentLineHeight=ppPreviousLineHeights;

			if (bDoSmoothFill) {

				while (pCurrentSource!=pSourceLineEnd) {

					const U32 SourceColour=*pCurrentSource;
					const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
					const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
					const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
					const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

					int nLuma=GetLuminance(SourceColour);
					nLuma-=(128*255);
					
					const int nOffset=(nLuma*nOffsetScale)>>16;

					U32* pOffsetOutputStart=
						pCurrentOutput+(nOffset*nWidth);

					U32* pOffsetOutput=pOffsetOutputStart;
					U32* pPreviousOffsetOutput=*ppCurrentLineHeight;

					int nDestRed;
					int nDestGreen;
					int nDestBlue;
					int nDestAlpha;
					int nDestDistance;
					if (pPreviousOffsetOutput==NULL) {
						nDestRed=0;
						nDestGreen=0;
						nDestBlue=0;
						nDestAlpha=0;
						nDestDistance=10000;
					} else {
						const U32 DestColour=*pPreviousOffsetOutput;
						nDestRed=(DestColour>>SHIFT_RED)&0xff;
						nDestGreen=(DestColour>>SHIFT_GREEN)&0xff;
						nDestBlue=(DestColour>>SHIFT_BLUE)&0xff;
						nDestAlpha=(DestColour>>SHIFT_ALPHA)&0xff;
						nDestDistance=(pOffsetOutput-pPreviousOffsetOutput)/nWidth;
						if (nDestDistance==0) {
							nDestDistance=1;
						}
					}

					const int nDeltaRed=(nDestRed-nSourceRed);
					const int nDeltaGreen=(nDestGreen-nSourceGreen);
					const int nDeltaBlue=(nDestBlue-nSourceBlue);
					const int nDeltaAlpha=(nDestAlpha-nSourceAlpha);

					const int nIncRed=(nDeltaRed/nDestDistance);
					const int nIncGreen=(nDeltaGreen/nDestDistance);
					const int nIncBlue=(nDeltaBlue/nDestDistance);
					const int nIncAlpha=(nDeltaAlpha/nDestDistance);

					int nCurrentRed=nSourceRed;
					int nCurrentGreen=nSourceGreen;
					int nCurrentBlue=nSourceBlue;
					int nCurrentAlpha=nSourceAlpha;

					while ((pOffsetOutput<pOutputEnd)&&
						(pOffsetOutput>=pOutput)&&
						(pOffsetOutput>pPreviousOffsetOutput)) {

						U32 CurrentColour=
							(nCurrentRed<<SHIFT_RED)|
							(nCurrentGreen<<SHIFT_GREEN)|
							(nCurrentBlue<<SHIFT_BLUE)|
							(nCurrentAlpha<<SHIFT_ALPHA);

						*pOffsetOutput=CurrentColour;

						nCurrentRed+=nIncRed;
						nCurrentGreen+=nIncGreen;
						nCurrentBlue+=nIncBlue;
						nCurrentAlpha+=nIncAlpha;
					
						pOffsetOutput-=nWidth;

					}

					*ppCurrentLineHeight=pOffsetOutputStart;

					pCurrentSource+=1;
					pCurrentOutput+=1;
					ppCurrentLineHeight+=1;
					
				}

			} else {

				while (pCurrentSource!=pSourceLineEnd) {

					const U32 SourceColour=*pCurrentSource;

					int nLuma=GetLuminance(SourceColour);
					nLuma-=(128*255);
					
					const int nOffset=(nLuma*nOffsetScale)>>16;

					U32* pOffsetOutputStart=
						pCurrentOutput+(nOffset*nWidth);

					U32* pOffsetOutput=pOffsetOutputStart;
					U32* pPreviousOffsetOutput=*ppCurrentLineHeight;

					while ((pOffsetOutput<pOutputEnd)&&
						(pOffsetOutput>=pOutput)&&
						(pOffsetOutput>pPreviousOffsetOutput)) {

						*pOffsetOutput=SourceColour;
					
						pOffsetOutput-=nWidth;

					}

					*ppCurrentLineHeight=pOffsetOutputStart;

					pCurrentSource+=1;
					pCurrentOutput+=1;
					ppCurrentLineHeight+=1;
					
				}


			}

			pCurrentSource+=(nWidth*nLineGap);
			pCurrentOutput+=(nWidth*nLineGap);

		}

		U32* pFinalLineStart=pOutputEnd-nWidth;
		U32* pFinalLineEnd=pOutputEnd;
				
		U32** ppCurrentLineHeight=ppPreviousLineHeights;

		U32* pCurrentOutput=pFinalLineStart;

		while (pCurrentOutput<pFinalLineEnd) {

			U32* pPreviousOffsetOutput=*ppCurrentLineHeight;

			const U32 SourceColour=*pPreviousOffsetOutput;

			U32* pOffsetOutput=pCurrentOutput;

			while ((pOffsetOutput<pOutputEnd)&&
				(pOffsetOutput>=pOutput)&&
				(pOffsetOutput>pPreviousOffsetOutput)) {

				*pOffsetOutput=SourceColour;
			
				pOffsetOutput-=nWidth;

			}

			pCurrentOutput+=1;
			ppCurrentLineHeight+=1;
			
		}


	}


}

