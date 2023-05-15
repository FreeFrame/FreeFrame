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

#include "Radiant.h"
#include "PeteHelpers.h"

#include <mmintrin.h>

static SPete_Parameter g_Parameters[]={
	{
		"Spike Scale",
		127.0f,
		0.0f,
		1024.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Source Point X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Source Point Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Boost Light",
		1.0f,
		0.0f,
		5.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Fade",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},

};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Radiant_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Radiant_Init(SPete_Radiant_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Radiant_DeInit(pInstanceData);

	const int nScreenPixels=(nWidth*nHeight);
	const int nAverageBufferByteCount=(nScreenPixels*sizeof(SPete_Radiant_AveragePixel));
	pInstanceData->hAverageBuffer=Pete_NewHandle(nAverageBufferByteCount);
	if (pInstanceData->hAverageBuffer==NULL) {
		Pete_Radiant_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Radiant_DeInit(SPete_Radiant_Data* pInstanceData) {

	if (pInstanceData->hAverageBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hAverageBuffer);
		pInstanceData->hAverageBuffer=NULL;
	}

}

void Pete_Radiant_Render(SPete_Radiant_Data* pInstanceData,SPete_Radiant_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFixedShift=8;
	const int nFixedMult=(1<<nFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const bool bDoFade=(pSettings->m_DoFade!=0.0f);

	const int nBoostLight=(int)(pSettings->m_BoostLight*256.0f);

	SPete_Radiant_AveragePixel* pAverageBuffer=
		(SPete_Radiant_AveragePixel*)Pete_LockHandle(pInstanceData->hAverageBuffer);
	if (pAverageBuffer==NULL) {
		return;
	}
	Pete_ZeroMemory(pAverageBuffer,sizeof(SPete_Radiant_AveragePixel)*nNumPixels);

	const U32* pSourceEnd=(pSource+nNumPixels);

	const int nSpikeScale=static_cast<int>((pSettings->m_SpikeScale*4));

	const int nOriginX=(pSettings->m_SourcePointX*nWidth);
	const int nOriginY=(pSettings->m_SourcePointY*nHeight);

	const int nDeltaStartX=(0-nOriginX);
	const int nDeltaEndX=(nWidth-nOriginX);

	const int nScaledDeltaStartX=(nDeltaStartX*nSpikeScale)>>8;
	const int nScaledDeltaEndX=(nDeltaEndX*nSpikeScale)>>8;
	const int nScaledFinalStartX=(0+nScaledDeltaStartX);
	const int nScaledFinalEndX=(nWidth+nScaledDeltaEndX);

	int nSourceY=0;
	U32* pCurrentSource=pSource;
	while (pCurrentSource!=pSourceEnd) {

		const U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		const int nDeltaY=(nSourceY-nOriginY);

		int nRayFanHeight=(nDeltaY*nSpikeScale)>>8;
		nRayFanHeight*=nWidth;
		nRayFanHeight/=nHeight;
		int nIncY;
		if (nRayFanHeight>0) {
			nIncY=1;
		} else {
			nIncY=-1;
		}
		const int nScaledFinalY=(nSourceY+nRayFanHeight+nIncY);

		int nLerpIncFP;
		if (nRayFanHeight!=0) {
			nLerpIncFP=(1<<16)/AbsInt(nRayFanHeight);
		} else {
			nLerpIncFP=0;
		}
		int nLerpValueFP=0;

		int nDestY;
		for (nDestY=nSourceY; nDestY!=nScaledFinalY; nDestY+=nIncY,nLerpValueFP+=nLerpIncFP) {

			if ((nDestY<0)||(nDestY>=nHeight)) {
				continue;
			}

			const int nOneMinusLerpFP=(1<<16)-nLerpValueFP;

			const int nFanLineStartX=((nScaledFinalStartX*nLerpValueFP)+
				(0*nOneMinusLerpFP))>>16;
			const int nFanLineEndX=((nScaledFinalEndX*nLerpValueFP)+
				(nWidth*nOneMinusLerpFP))>>16;

			const int nFanLineLength=(nFanLineEndX-nFanLineStartX);

			const int nSourceFPXInc=(nWidth<<16)/nFanLineLength;

			int nSourceFPX=0;

			SPete_Radiant_AveragePixel* pDestLineStart=
				pAverageBuffer+(nDestY*nWidth);

			const int nYDistanceFromOrigin=AbsInt(nDestY-nOriginY);

			const int nClipMinX=GateInt(nOriginX-nYDistanceFromOrigin,0,(nWidth-1));
			const int nClipMaxX=GateInt((nOriginX+nYDistanceFromOrigin+1),1,nWidth);

			const int nStartPixelsClipped=(nClipMinX-nFanLineStartX);

			nSourceFPX+=(nSourceFPXInc*nStartPixelsClipped);

			int nWeight;
			if (bDoFade) {
				nWeight=((1<<16)-nLerpValueFP)>>8;
			} else {
				nWeight=127;
			}

			const __m64 WeightMultReg=
				_mm_set_pi16(
				(nWeight),
				(nWeight),
				(nWeight),
				(nWeight));

			const __m64 AlphaIncReg=
				_mm_set_pi16(
				(1),
				(0),
				(0),
				(0));

			const __m64 MultiplyMask=
				_mm_set_pi16(
				(0x00ff),
				(0x00ff),
				(0x00ff),
				(0x00ff));

			// mm0 is zero register
			// mm1 is weight multiplier for this line
			// mm2 is alpha increment
			// mm3 is multiply mask
			__asm {

				pxor		mm0,mm0
				movq		mm1,WeightMultReg
				movq		mm2,AlphaIncReg
				movq		mm3,MultiplyMask

			}

			int nDestX;
			for (nDestX=nClipMinX; nDestX<nClipMaxX; nDestX+=1,nSourceFPX+=nSourceFPXInc) {

				const int nSourceX=(nSourceFPX>>16);

				const U32* pCurrentSource=pSourceLineStart+nSourceX;
				SPete_Radiant_AveragePixel* pCurrentDest=
					(pDestLineStart+nDestX);

#if 0
				const U32 SourceColour=*pCurrentSource;

				unsigned short nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				unsigned short nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				unsigned short nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				unsigned short nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				nSourceRed<<=7;
				nSourceGreen<<=7;
				nSourceBlue<<=7;
				nSourceAlpha<<=7;

				unsigned short nDestIncRed=(nSourceRed*nWeight)>>16;
				unsigned short nDestIncGreen=(nSourceGreen*nWeight)>>16;
				unsigned short nDestIncBlue=(nSourceBlue*nWeight)>>16;
				unsigned short nDestIncAlpha=(nSourceAlpha*nWeight)>>16;

				nDestIncAlpha+=1;

				pCurrentDest->nRed+=nDestIncRed;
				pCurrentDest->nGreen+=nDestIncGreen;
				pCurrentDest->nBlue+=nDestIncBlue;
				pCurrentDest->nWeight+=nDestIncAlpha;

#else
				__asm {

					mov			esi,pCurrentSource
					movq		mm7,[esi]		// load the source colour
					punpcklbw	mm7,mm0			// unpack the source colour into 16 bit words

					psllw		mm7,7			// put the lower bytes into the top bytes of the source

					mov			edi,pCurrentDest
					movq		mm6,[edi]		// load the average buffer entry

					pmulhw		mm7,mm1			// colour=colour*weight

					pand		mm7,mm3			// mask out the top bytes of each word, multiply is signed so messes them up

					paddw		mm7,mm2			// add the alpha/weight increment fudge

					paddusw		mm6,mm7			// add the colour to the average buffer entry

					movq		[edi],mm6		// store the average buffer entry

				}
#endif

			}


		}

		nSourceY+=1;
		pCurrentSource+=nWidth;

	}

	const int nDeltaStartY=(0-nOriginY);
	const int nDeltaEndY=(nHeight-nOriginY);

	const int nScaledDeltaStartY=(nDeltaStartY*nSpikeScale)>>8;
	const int nScaledDeltaEndY=(nDeltaEndY*nSpikeScale)>>8;
	const int nScaledFinalStartY=(0+nScaledDeltaStartY);
	const int nScaledFinalEndY=(nHeight+nScaledDeltaEndY);

	int nSourceX=0;
	U32* pSourceLineEnd=(pSource+nWidth);
	pCurrentSource=pSource;
	while (pCurrentSource!=pSourceLineEnd) {

		const U32* pSourceRowStart=pCurrentSource;

		const int nDeltaX=(nSourceX-nOriginX);

		int nRayFanWidth=(nDeltaX*nSpikeScale)>>8;
		nRayFanWidth*=nHeight;
		nRayFanWidth/=nWidth;
		int nIncX;
		if (nRayFanWidth>0) {
			nIncX=1;
		} else {
			nIncX=-1;
		}
		const int nScaledFinalX=(nSourceX+nRayFanWidth+nIncX);

		int nLerpIncFP;
		if (nRayFanWidth!=0) {
			nLerpIncFP=(1<<16)/AbsInt(nRayFanWidth);
		} else {
			nLerpIncFP=0;
		}
		int nLerpValueFP=0;

		int nDestX;
		for (nDestX=nSourceX; nDestX!=nScaledFinalX; nDestX+=nIncX,nLerpValueFP+=nLerpIncFP) {

			if ((nDestX<0)||(nDestX>=nWidth)) {
				continue;
			}

			const int nOneMinusLerpFP=(1<<16)-nLerpValueFP;

			const int nFanLineStartY=((nScaledFinalStartY*nLerpValueFP)+
				(0*nOneMinusLerpFP))>>16;
			const int nFanLineEndY=((nScaledFinalEndY*nLerpValueFP)+
				(nHeight*nOneMinusLerpFP))>>16;

			const int nFanLineLength=(nFanLineEndY-nFanLineStartY);

			const int nSourceFPYInc=(nHeight<<16)/nFanLineLength;

			int nSourceFPY=0;

			SPete_Radiant_AveragePixel* pDestRowStart=
				pAverageBuffer+nDestX;

			const int nXDistanceFromOrigin=AbsInt(nDestX-nOriginX);

			const int nClipMinY=GateInt(nOriginY-nXDistanceFromOrigin,0,(nHeight-1));
			const int nClipMaxY=GateInt((nOriginY+nXDistanceFromOrigin+1),1,nHeight);

			const int nStartPixelsClipped=(nClipMinY-nFanLineStartY);

			nSourceFPY+=(nSourceFPYInc*nStartPixelsClipped);

			int nWeight;
			if (bDoFade) {
				nWeight=((1<<16)-nLerpValueFP)>>8;
			} else {
				nWeight=127;
			}
			const __m64 WeightMultReg=
				_mm_set_pi16(
				(nWeight),
				(nWeight),
				(nWeight),
				(nWeight));

			const __m64 AlphaIncReg=
				_mm_set_pi16(
				(1),
				(0),
				(0),
				(0));

			const __m64 MultiplyMask=
				_mm_set_pi16(
				(0x00ff),
				(0x00ff),
				(0x00ff),
				(0x00ff));

			// mm0 is zero register
			// mm1 is weight multiplier for this line
			// mm2 is alpha increment
			// mm3 is multiply mask
			__asm {

				pxor		mm0,mm0
				movq		mm1,WeightMultReg
				movq		mm2,AlphaIncReg
				movq		mm3,MultiplyMask

			}

			int nDestY;
			for (nDestY=nClipMinY; nDestY<nClipMaxY; nDestY+=1,nSourceFPY+=nSourceFPYInc) {

				const int nSourceY=(nSourceFPY>>16);

				const U32* pCurrentSource=pSourceRowStart+(nSourceY*nWidth);
				SPete_Radiant_AveragePixel* pCurrentDest=
					(pDestRowStart+(nDestY*nWidth));
#if 0

				const U32 SourceColour=*pCurrentSource;

				unsigned short nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				unsigned short nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				unsigned short nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				unsigned short nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				nSourceRed<<=7;
				nSourceGreen<<=7;
				nSourceBlue<<=7;
				nSourceAlpha<<=7;

				unsigned short nDestIncRed=(nSourceRed*nWeight)>>16;
				unsigned short nDestIncGreen=(nSourceGreen*nWeight)>>16;
				unsigned short nDestIncBlue=(nSourceBlue*nWeight)>>16;
				unsigned short nDestIncAlpha=(nSourceAlpha*nWeight)>>16;

				nDestIncAlpha+=1;

				pCurrentDest->nRed+=nDestIncRed;
				pCurrentDest->nGreen+=nDestIncGreen;
				pCurrentDest->nBlue+=nDestIncBlue;
				pCurrentDest->nWeight+=nDestIncAlpha;

#else
				__asm {

					mov			esi,pCurrentSource
					movq		mm7,[esi]		// load the source colour
					punpcklbw	mm7,mm0			// unpack the source colour into 16 bit words

					psllw		mm7,7			// put the lower bytes into the top bytes of the source

					mov			edi,pCurrentDest
					movq		mm6,[edi]		// load the average buffer entry

					pmulhw		mm7,mm1			// colour=colour*weight

					pand		mm7,mm3			// mask out the top bytes of each word, multiply is signed so messes them up

					paddw		mm7,mm2			// add the alpha/weight increment fudge

					paddusw		mm6,mm7			// add the colour to the average buffer entry

					movq		[edi],mm6		// store the average buffer entry

				}
#endif

			}


		}

		nSourceX+=1;
		pCurrentSource+=1;

	}

	SPete_Radiant_AveragePixel* pCurrentAverage=pAverageBuffer;

	U32* pOutputEnd=(pOutput+nNumPixels);
	U32* pCurrentOutput=pOutput;
	while (pCurrentOutput!=pOutputEnd) {

		const unsigned int nWeight=(pCurrentAverage->nWeight);
		if (nWeight==0) {
			*pCurrentOutput=0x00000000;
		} else {

			const unsigned int nAverageRed=(pCurrentAverage->nRed<<8);
			const unsigned int nAverageGreen=(pCurrentAverage->nGreen<<8);
			const unsigned int nAverageBlue=(pCurrentAverage->nBlue<<8);

			unsigned int nOutputRed=(nAverageRed/nWeight);
			nOutputRed*=nBoostLight;
			nOutputRed>>=8;
			if (nOutputRed>255) {
				nOutputRed=255;
			}

			unsigned int nOutputGreen=(nAverageGreen/nWeight);
			nOutputGreen*=nBoostLight;
			nOutputGreen>>=8;
			if (nOutputGreen>255) {
				nOutputGreen=255;
			}

			unsigned int nOutputBlue=(nAverageBlue/nWeight);
			nOutputBlue*=nBoostLight;
			nOutputBlue>>=8;
			if (nOutputBlue>255) {
				nOutputBlue=255;
			}

			const unsigned int nOutputAlpha=0xff;

			const U32 OutputColour=
				(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE)|
				(nOutputAlpha<<SHIFT_ALPHA);

			*pCurrentOutput=OutputColour;

		}

		pCurrentOutput+=1;
		pCurrentAverage+=1;

	}

	_m_empty();

}

void Pete_Radiant_Render_CVersion(SPete_Radiant_Data* pInstanceData,SPete_Radiant_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFixedShift=8;
	const int nFixedMult=(1<<nFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;


	SPete_Radiant_AveragePixel* pAverageBuffer=
		(SPete_Radiant_AveragePixel*)Pete_LockHandle(pInstanceData->hAverageBuffer);
	if (pAverageBuffer==NULL) {
		return;
	}
	Pete_ZeroMemory(pAverageBuffer,sizeof(SPete_Radiant_AveragePixel)*nNumPixels);

	const U32* pSourceEnd=(pSource+nNumPixels);

	const int nSpikeScale=static_cast<int>((pSettings->m_SpikeScale*2));

	const int nOriginX=(pSettings->m_SourcePointX*nWidth);
	const int nOriginY=(pSettings->m_SourcePointY*nHeight);

	const int nDeltaStartX=(0-nOriginX);
	const int nDeltaEndX=(nWidth-nOriginX);

	const int nScaledDeltaStartX=(nDeltaStartX*nSpikeScale)>>8;
	const int nScaledDeltaEndX=(nDeltaEndX*nSpikeScale)>>8;
	const int nScaledFinalStartX=(0+nScaledDeltaStartX);
	const int nScaledFinalEndX=(nWidth+nScaledDeltaEndX);

	int nSourceY=0;
	U32* pCurrentSource=pSource;
	while (pCurrentSource!=pSourceEnd) {

		const U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		const int nDeltaY=(nSourceY-nOriginY);

		int nRayFanHeight=(nDeltaY*nSpikeScale)>>8;
		nRayFanHeight*=nWidth;
		nRayFanHeight/=nHeight;
		int nIncY;
		if (nRayFanHeight>0) {
			nIncY=1;
		} else {
			nIncY=-1;
		}
		const int nScaledFinalY=(nSourceY+nRayFanHeight+nIncY);

		int nLerpIncFP;
		if (nRayFanHeight!=0) {
			nLerpIncFP=(1<<16)/AbsInt(nRayFanHeight);
		} else {
			nLerpIncFP=0;
		}
		int nLerpValueFP=0;

		int nDestY;
		for (nDestY=nSourceY; nDestY!=nScaledFinalY; nDestY+=nIncY,nLerpValueFP+=nLerpIncFP) {

			if ((nDestY<0)||(nDestY>=nHeight)) {
				continue;
			}

			const int nOneMinusLerpFP=(1<<16)-nLerpValueFP;

			const int nFanLineStartX=((nScaledFinalStartX*nLerpValueFP)+
				(0*nOneMinusLerpFP))>>16;
			const int nFanLineEndX=((nScaledFinalEndX*nLerpValueFP)+
				(nWidth*nOneMinusLerpFP))>>16;

			const int nFanLineLength=(nFanLineEndX-nFanLineStartX);

			const int nSourceFPXInc=(nWidth<<16)/nFanLineLength;

			int nSourceFPX=0;

			SPete_Radiant_AveragePixel* pDestLineStart=
				pAverageBuffer+(nDestY*nWidth);

			const int nYDistanceFromOrigin=AbsInt(nDestY-nOriginY);

			const int nClipMinX=GateInt(nOriginX-nYDistanceFromOrigin,0,(nWidth-1));
			const int nClipMaxX=GateInt((nOriginX+nYDistanceFromOrigin+1),1,nWidth);

			const int nStartPixelsClipped=(nClipMinX-nFanLineStartX);

			nSourceFPX+=(nSourceFPXInc*nStartPixelsClipped);

			int nWeight=((1<<16)-nLerpValueFP)>>8;

			int nDestX;
			for (nDestX=nClipMinX; nDestX<nClipMaxX; nDestX+=1,nSourceFPX+=nSourceFPXInc) {

				const int nSourceX=(nSourceFPX>>16);

				const U32* pCurrentSource=pSourceLineStart+nSourceX;
				const U32 SourceColour=*pCurrentSource;

				unsigned int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				unsigned int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				unsigned int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				unsigned int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				nSourceAlpha+=1;

				SPete_Radiant_AveragePixel* pCurrentDest=
					(pDestLineStart+nDestX);

				pCurrentDest->nRed+=(nSourceRed*nWeight)>>8;
				pCurrentDest->nGreen+=(nSourceGreen*nWeight)>>8;
				pCurrentDest->nBlue+=(nSourceBlue*nWeight)>>8;
				pCurrentDest->nWeight+=(nSourceAlpha*nWeight)>>8;

			}


		}

		nSourceY+=1;
		pCurrentSource+=nWidth;

	}

	const int nDeltaStartY=(0-nOriginY);
	const int nDeltaEndY=(nHeight-nOriginY);

	const int nScaledDeltaStartY=(nDeltaStartY*nSpikeScale)>>8;
	const int nScaledDeltaEndY=(nDeltaEndY*nSpikeScale)>>8;
	const int nScaledFinalStartY=(0+nScaledDeltaStartY);
	const int nScaledFinalEndY=(nHeight+nScaledDeltaEndY);

	int nSourceX=0;
	U32* pSourceLineEnd=(pSource+nWidth);
	pCurrentSource=pSource;
	while (pCurrentSource!=pSourceLineEnd) {

		const U32* pSourceRowStart=pCurrentSource;

		const int nDeltaX=(nSourceX-nOriginX);

		int nRayFanWidth=(nDeltaX*nSpikeScale)>>8;
		nRayFanWidth*=nHeight;
		nRayFanWidth/=nWidth;
		int nIncX;
		if (nRayFanWidth>0) {
			nIncX=1;
		} else {
			nIncX=-1;
		}
		const int nScaledFinalX=(nSourceX+nRayFanWidth+nIncX);

		int nLerpIncFP;
		if (nRayFanWidth!=0) {
			nLerpIncFP=(1<<16)/AbsInt(nRayFanWidth);
		} else {
			nLerpIncFP=0;
		}
		int nLerpValueFP=0;

		int nDestX;
		for (nDestX=nSourceX; nDestX!=nScaledFinalX; nDestX+=nIncX,nLerpValueFP+=nLerpIncFP) {

			if ((nDestX<0)||(nDestX>=nWidth)) {
				continue;
			}

			const int nOneMinusLerpFP=(1<<16)-nLerpValueFP;

			const int nFanLineStartY=((nScaledFinalStartY*nLerpValueFP)+
				(0*nOneMinusLerpFP))>>16;
			const int nFanLineEndY=((nScaledFinalEndY*nLerpValueFP)+
				(nHeight*nOneMinusLerpFP))>>16;

			const int nFanLineLength=(nFanLineEndY-nFanLineStartY);

			const int nSourceFPYInc=(nHeight<<16)/nFanLineLength;

			int nSourceFPY=0;

			SPete_Radiant_AveragePixel* pDestLineStart=
				pAverageBuffer+nDestX;

			const int nXDistanceFromOrigin=AbsInt(nDestX-nOriginX);

			const int nClipMinY=GateInt(nOriginY-nXDistanceFromOrigin,0,(nHeight-1));
			const int nClipMaxY=GateInt((nOriginY+nXDistanceFromOrigin+1),1,nHeight);

			const int nStartPixelsClipped=(nClipMinY-nFanLineStartY);

			nSourceFPY+=(nSourceFPYInc*nStartPixelsClipped);

			const int nWeight=((1<<16)-nLerpValueFP)>>8;

			int nDestY;
			for (nDestY=nClipMinY; nDestY<nClipMaxY; nDestY+=1,nSourceFPY+=nSourceFPYInc) {

				const int nSourceY=(nSourceFPY>>16);

				const U32* pCurrentSource=pSourceRowStart+(nSourceY*nWidth);
				const U32 SourceColour=*pCurrentSource;

				unsigned int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				unsigned int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				unsigned int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				unsigned int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				nSourceAlpha+=1;

				SPete_Radiant_AveragePixel* pCurrentDest=
					(pDestLineStart+(nDestY*nWidth));

				pCurrentDest->nRed+=(nSourceRed*nWeight)>>8;
				pCurrentDest->nGreen+=(nSourceGreen*nWeight)>>8;
				pCurrentDest->nBlue+=(nSourceBlue*nWeight)>>8;
				pCurrentDest->nWeight+=(nSourceAlpha*nWeight)>>8;

			}


		}

		nSourceX+=1;
		pCurrentSource+=1;

	}

	SPete_Radiant_AveragePixel* pCurrentAverage=pAverageBuffer;

	U32* pOutputEnd=(pOutput+nNumPixels);
	U32* pCurrentOutput=pOutput;
	while (pCurrentOutput!=pOutputEnd) {

		const unsigned int nWeight=(pCurrentAverage->nWeight);
		if (nWeight==0) {
			*pCurrentOutput=0x00000000;
		} else {

			const unsigned int nAverageRed=(pCurrentAverage->nRed<<8);
			const unsigned int nAverageGreen=(pCurrentAverage->nGreen<<8);
			const unsigned int nAverageBlue=(pCurrentAverage->nBlue<<8);

			const unsigned int nOutputRed=(nAverageRed/nWeight);
			const unsigned int nOutputGreen=(nAverageGreen/nWeight);
			const unsigned int nOutputBlue=(nAverageBlue/nWeight);
			const unsigned int nOutputAlpha=0xff;

			const U32 OutputColour=
				(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE)|
				(nOutputAlpha<<SHIFT_ALPHA);

			*pCurrentOutput=OutputColour;

		}

		pCurrentOutput+=1;
		pCurrentAverage+=1;

	}

}

