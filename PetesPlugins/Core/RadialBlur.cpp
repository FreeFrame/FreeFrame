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

// jan 13, 2007 ck: MMX version was trashing two bytes beyond output buffer,
// due to movq when movd was intended

#include "RadialBlur.h"

#include <math.h>

// ck: my cheesy substitutes for mmintrin.h
#define __m64 __int64
#define _mm_set_pi16(a, b, c, d) (__int64(a) << 48) + (__int64(b) << 32) + (__int64(c) << 16) + (__int64(d) << 0)
#define _m_empty __asm emms;

#ifndef PETE_MAC_OSX
//#include "mmintrin.h"
#endif // PETE_MAC_OSX

static SPete_Parameter g_Parameters[]={
	{
		"Layer Scale",
		0.03f,
		0.0f,
		0.1f,
		PETE_PARAM_FLOAT
	},
	{
		"Quality",
		2.0f,
		0.0f,
		2.0f,
		PETE_PARAM_INT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_RadialBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

#ifdef PETE_MAC_OSX
#define PETE_USE_C
#endif // PETE_MAC_OSX

#ifndef PETE_USE_C
void Pete_RadialBlur_Render4(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight);
void Pete_RadialBlur_Render8(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight);
void Pete_RadialBlur_Render16(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight);
#else
void Pete_RadialBlur_RenderCVersion(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight,int nLayersCountShift);
#endif // PETE_USE_C

int Pete_RadialBlur_Init(SPete_RadialBlur_Data* pInstanceData,int nWidth,int nHeight) {

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;
}

void Pete_RadialBlur_DeInit(SPete_RadialBlur_Data* pInstanceData) {

	// do nothing

}

void Pete_RadialBlur_Render(SPete_RadialBlur_Data* pInstanceData,SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput) {

	assert(pSettings!=NULL);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

#ifdef PETE_USE_C

	switch ((int)(pSettings->m_eQuality)) {

		case SPete_RadialBlur_Settings::eLowQuality:
			Pete_RadialBlur_RenderCVersion(pSettings,pSource,pOutput,nWidth,nHeight,2);
		break;

		case SPete_RadialBlur_Settings::eMediumQuality:
			Pete_RadialBlur_RenderCVersion(pSettings,pSource,pOutput,nWidth,nHeight,3);
		break;

		case SPete_RadialBlur_Settings::eHighQuality:
			Pete_RadialBlur_RenderCVersion(pSettings,pSource,pOutput,nWidth,nHeight,4);
		break;
	}

#else // PETE_USE_C

	switch ((int)(pSettings->m_eQuality)) {

		case SPete_RadialBlur_Settings::eLowQuality:
			Pete_RadialBlur_Render4(pSettings,pSource,pOutput,nWidth,nHeight);
		break;

		case SPete_RadialBlur_Settings::eMediumQuality:
			Pete_RadialBlur_Render8(pSettings,pSource,pOutput,nWidth,nHeight);
		break;

		case SPete_RadialBlur_Settings::eHighQuality:
			Pete_RadialBlur_Render16(pSettings,pSource,pOutput,nWidth,nHeight);
		break;
	}

#endif // PETE_USE_C

}

#ifndef PETE_USE_C

void Pete_RadialBlur_Render4(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight) {

	const int nFixedShift=16;
	const int nFixedMult=(1<<nFixedShift);

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const int nLayersCountShift=2;
	const int nLayersCount=(1<<nLayersCountShift);
	const int nLayerScaleIncrement=static_cast<int>(pSettings->m_LayerScale*nFixedMult);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const U32* pSourceCentre=pSource+(nHalfHeight*nWidth)+(nHalfWidth);

//	const __m64 ZeroReg=_m_from_int(0);	// ck: doesn't do anything
	__asm {

		pxor		mm0,mm0

	}

	int nY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		int nX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			__asm {

				pxor		mm1,mm1

			}
			int nLayerScale=(1*nFixedMult);

			int nCurrentLayer;
			for (nCurrentLayer=0; nCurrentLayer<nLayersCount; ++nCurrentLayer) {

				const int nSourceX=(nX*nLayerScale)>>nFixedShift;
				const int nSourceY=(nY*nLayerScale)>>nFixedShift;

				nLayerScale-=nLayerScaleIncrement;

				const U32* pCurrentSource=(pSourceCentre+(nSourceY*nWidth)+nSourceX);

				__asm {

					mov			esi, pCurrentSource

//					movq		mm2,[esi]	// ck: only need 32 bits
					movd		mm2,[esi]	// ck: 32-bit move
					punpcklbw	mm2,mm0

					paddw		mm1,mm2

				}


			}

			__asm {

				psrlw		mm1,2

				packuswb	mm1,mm0

				mov			esi,pCurrentOutput
//				movq		[esi],mm1	// ck: TRASHES 2 bytes beyond output buffer
				movd		[esi],mm1	// ck: 32-bit move

			}

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

	_m_empty();

}

void Pete_RadialBlur_Render8(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight) {

	const int nFixedShift=16;
	const int nFixedMult=(1<<nFixedShift);

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const int nLayersCountShift=3;
	const int nLayersCount=(1<<nLayersCountShift);
	const int nLayerScaleIncrement=static_cast<int>(pSettings->m_LayerScale*nFixedMult);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const U32* pSourceCentre=pSource+(nHalfHeight*nWidth)+(nHalfWidth);

//	const __m64 ZeroReg=_m_from_int(0);	// ck: doesn't do anything
	__asm {

		pxor		mm0,mm0

	}

	int nY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		int nX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			__asm {

				pxor		mm1,mm1

			}
			int nLayerScale=(1*nFixedMult);

			int nCurrentLayer;
			for (nCurrentLayer=0; nCurrentLayer<nLayersCount; ++nCurrentLayer) {

				const int nSourceX=(nX*nLayerScale)>>nFixedShift;
				const int nSourceY=(nY*nLayerScale)>>nFixedShift;

				nLayerScale-=nLayerScaleIncrement;

				const U32* pCurrentSource=(pSourceCentre+(nSourceY*nWidth)+nSourceX);

				__asm {

					mov			esi, pCurrentSource

//					movq		mm2,[esi]	// ck: only need 32 bits
					movd		mm2,[esi]	// ck: 32-bit move
					punpcklbw	mm2,mm0

					paddw		mm1,mm2

				}


			}

			__asm {

				psrlw		mm1,3

				packuswb	mm1,mm0

				mov			esi,pCurrentOutput
//				movq		[esi],mm1	// ck: TRASHES 2 bytes beyond output buffer
				movd		[esi],mm1	// ck: 32-bit move

			}

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

	_m_empty();

}

void Pete_RadialBlur_Render16(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight) {

	const int nFixedShift=16;
	const int nFixedMult=(1<<nFixedShift);

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const int nLayersCountShift=4;
	const int nLayersCount=(1<<nLayersCountShift);
	const int nLayerScaleIncrement=static_cast<int>(pSettings->m_LayerScale*nFixedMult);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const U32* pSourceCentre=pSource+(nHalfHeight*nWidth)+(nHalfWidth);

//	const __m64 ZeroReg=_m_from_int(0);	// ck: doesn't do anything
	__asm {

		pxor		mm0,mm0

	}

	int nY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		int nX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			__asm {

				pxor		mm1,mm1

			}
			int nLayerScale=(1*nFixedMult);

			int nCurrentLayer;
			for (nCurrentLayer=0; nCurrentLayer<nLayersCount; ++nCurrentLayer) {

				const int nSourceX=(nX*nLayerScale)>>nFixedShift;
				const int nSourceY=(nY*nLayerScale)>>nFixedShift;

				nLayerScale-=nLayerScaleIncrement;

				const U32* pCurrentSource=(pSourceCentre+(nSourceY*nWidth)+nSourceX);

				__asm {

					mov			esi, pCurrentSource

//					movq		mm2,[esi]	// ck: only need 32 bits
					movd		mm2,[esi]	// ck: 32-bit move
					punpcklbw	mm2,mm0

					paddw		mm1,mm2

				}


			}

			__asm {

				psrlw		mm1,4

				packuswb	mm1,mm0

				mov			esi,pCurrentOutput
//				movq		[esi],mm1	// ck: TRASHES 2 bytes beyond output buffer
				movd		[esi],mm1	// ck: 32-bit move

			}

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

	_m_empty();

}

#else // PETE_USE_C

void Pete_RadialBlur_RenderCVersion(SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput,int nWidth,int nHeight,int nLayersCountShift) {

	const int nFixedShift=16;
	const int nFixedMult=(1<<nFixedShift);

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const int nLayersCount=(1<<nLayersCountShift);
	const int nLayerScaleIncrement=static_cast<int>(pSettings->m_LayerScale*nFixedMult);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const U32* pSourceCentre=pSource+(nHalfHeight*nWidth)+(nHalfWidth);

	int nY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		int nX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			int nLayerScale=(1*nFixedMult);

			int nTotalRed=0;
			int nTotalGreen=0;
			int nTotalBlue=0;
			int nTotalAlpha=0;

			int nCurrentLayer;
			for (nCurrentLayer=0; nCurrentLayer<nLayersCount; ++nCurrentLayer) {

				const int nSourceX=(nX*nLayerScale)>>nFixedShift;
				const int nSourceY=(nY*nLayerScale)>>nFixedShift;

				nLayerScale-=nLayerScaleIncrement;

				const U32* pCurrentSource=(pSourceCentre+(nSourceY*nWidth)+nSourceX);
				const U32 SourceColour=(*pCurrentSource);

				const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				nTotalRed+=nSourceRed;
				nTotalGreen+=nSourceGreen;
				nTotalBlue+=nSourceBlue;
				nTotalAlpha+=nSourceAlpha;

			}

			const int nOutputRed=(nTotalRed>>nLayersCountShift);
			const int nOutputGreen=(nTotalGreen>>nLayersCountShift);
			const int nOutputBlue=(nTotalBlue>>nLayersCountShift);
			const int nOutputAlpha=(nTotalAlpha>>nLayersCountShift);

			const U32 OutputColour=
				(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE)|
				(nOutputAlpha<<SHIFT_ALPHA);

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

}

#endif // PETE_USE_C