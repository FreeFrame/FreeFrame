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

#include "Slide.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Horizontal",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Mirror",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Mirror Flip",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Proportion",
		0.3141583f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Speed 1",
		1.0f,
		-16.0f,
		16.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Speed 2",
		-0.5f,
		-16.0f,
		16.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Slide_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Slide_Init(SPete_Slide_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Slide_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Slide_DeInit(SPete_Slide_Data* pInstanceData) {

	// do nothing

}

void Pete_Slide_Render(SPete_Slide_Data* pInstanceData,SPete_Slide_Settings* pSettings,U32* pSource,U32* pOutput) {

	const float CurrentTime=pInstanceData->m_Time;
	pInstanceData->m_Time+=1.0f;

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const bool bHorizontal=(pSettings->m_DoHorizontal>0.0f);
	const bool bMirror=(pSettings->m_DoMirror>0.0f);
	const bool bMirrorFlip=(pSettings->m_DoMirrorFlip>0.0f);

	if (bHorizontal) {

		int nXOffset1=static_cast<int>(pSettings->m_Speed1*CurrentTime);
		nXOffset1%=nWidth;
		if (nXOffset1<0) {
			nXOffset1+=nWidth;
		}

		int nXOffset2=static_cast<int>(pSettings->m_Speed2*CurrentTime);
		nXOffset2%=nWidth;
		if (nXOffset2<0) {
			nXOffset2+=nWidth;
		}

		const int nYThreshold=static_cast<int>(pSettings->m_Proportion*nHeight);

		int nY=0;
		while (pCurrentOutput!=pOutputEnd) {

			const U32* pOutputLineStart=pCurrentOutput;
			const U32* pOutputLineEnd=pOutputLineStart+nWidth;

			const bool bInFirstZone=(nY<nYThreshold);

			int nX;

			if (bInFirstZone) {
				nX=nXOffset1;
			} else {
				nX=nXOffset2;
			}

			const int nMirroredY=(nYThreshold+nHeight)-(nY-nYThreshold);

			int nSourceY;
			if (bInFirstZone) {

				if (bMirror&&bMirrorFlip) {
						nSourceY=nMirroredY;
				} else {
					nSourceY=nY;
				}

			} else {

				if (bMirror&&!bMirrorFlip) {
					nSourceY=nMirroredY;
				} else {
					nSourceY=nY;
				}
			}

			nSourceY%=nHeight;

			while (pCurrentOutput!=pOutputLineEnd) {

				if (nX>=nWidth) {
					nX-=nWidth;
				}

				U32*const pCurrentSource=pSource+(nSourceY*nWidth)+nX;

				const U32 SourceColour=*pCurrentSource;

				*pCurrentOutput=SourceColour;

				pCurrentOutput+=1;

				nX+=1;

			}

			nY+=1;

		}

	} else {

		int nYOffset1=static_cast<int>(pSettings->m_Speed1*CurrentTime);
		nYOffset1%=nWidth;
		if (nYOffset1<0) {
			nYOffset1+=nWidth;
		}

		int nYOffset2=static_cast<int>(pSettings->m_Speed2*CurrentTime);
		nYOffset2%=nWidth;
		if (nYOffset2<0) {
			nYOffset2+=nWidth;
		}

		const int nXThreshold=static_cast<int>(pSettings->m_Proportion*nWidth);

		int nY=0;
		while (pCurrentOutput!=pOutputEnd) {

			const U32* pOutputLineStart=pCurrentOutput;
			const U32* pOutputLineEnd=pOutputLineStart+nWidth;

			int nX=0;

			int nSourceY=nY+nYOffset1;
			nSourceY%=nHeight;

			bool bUseMirrorX=(bMirror&&bMirrorFlip);

			int nSourceX;
			int nXInc;
			if (bUseMirrorX) {
				nSourceX=(nXThreshold+nWidth)-(nX-nXThreshold);
				nXInc=-1;
			} else {
				nSourceX=0;
				nXInc=1;
			}

			while (nX<nXThreshold) {

				if (nSourceX<0) {
					nSourceX+=nWidth;
				}

				if (nSourceX>=nWidth) {
					nSourceX-=nWidth;
				}

				U32*const pCurrentSource=pSource+(nSourceY*nWidth)+nSourceX;

				const U32 SourceColour=*pCurrentSource;

				*pCurrentOutput=SourceColour;

				pCurrentOutput+=1;

				nX+=1;
				nSourceX+=nXInc;

			}

			nSourceY=nY+nYOffset2;
			nSourceY%=nHeight;

			bUseMirrorX=(bMirror&&!bMirrorFlip);

			nSourceX;
			nXInc;
			if (bUseMirrorX) {
				nSourceX=(nXThreshold+nWidth)-(nX-nXThreshold);
				nXInc=-1;
			} else {
				nSourceX=0;
				nXInc=1;
			}

			while (nX<nWidth) {

				if (nSourceX<0) {
					nSourceX+=nWidth;
				}

				if (nSourceX>=nWidth) {
					nSourceX-=nWidth;
				}

				U32*const pCurrentSource=pSource+(nSourceY*nWidth)+nSourceX;

				const U32 SourceColour=*pCurrentSource;

				*pCurrentOutput=SourceColour;

				pCurrentOutput+=1;

				nSourceX+=nXInc;
				nX+=1;

			}

			nY+=1;

		}

	}

}