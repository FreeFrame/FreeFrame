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

#include "SpiralBlur.h"
#include "PeteHelpers.h"

#include "math.h"
#ifndef PETE_MAC_OSX
#include "mmintrin.h"
#endif // PETE_MAC_OSX

const int nMaxLayerCount=32;

static SPete_Parameter g_Parameters[]={
	{
		"Angle",
		(10.0f*Pete_TwoPi)/360.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Scale",
		1.0f,
		0.5f,
		2.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Layer Count",
		8.0f,
		1.0f,
		31.0f,
		PETE_PARAM_INT
	},
	{	"Origin X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{	"Origin Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_SpiralBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

#ifdef PETE_MAC_OSX
#define PETE_USE_C
#endif // PETE_USE_C

int Pete_SpiralBlur_Init(SPete_SpiralBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_SpiralBlur_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_SpiralBlur_DeInit(SPete_SpiralBlur_Data* pInstanceData) {
	// do nothing
}

#ifndef PETE_USE_C

void Pete_SpiralBlur_Render(SPete_SpiralBlur_Data* pInstanceData,SPete_SpiralBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFPShift=16;
	const int nFPMult=(1<<nFPShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const int nHalfWidth=static_cast<int>((Width/2.0f)*nFPMult);
	const int nHalfHeight=static_cast<int>((Height/2.0f)*nFPMult);

	const float AngleRangeRadians=pSettings->m_Angle;

	const float StartAngleRadians=(-AngleRangeRadians/2.0f);

	const float EndScale=(pSettings->m_Scale-1.0f);

	const int nLayerCount=static_cast<int>(pSettings->m_LayerCount);

	const int nPixelsCount=(nWidth*nHeight);

	U32* pOutputStart=pOutput;
	U32* pOutputEnd=(pOutput+nPixelsCount);

	U32* pCurrentOutput=pOutputStart;

	int nCosLayerAngle[nMaxLayerCount];
	int nSinLayerAngle[nMaxLayerCount];
	int nLayerScale[nMaxLayerCount];

	int nCount;
	for (nCount=0; nCount<nLayerCount; nCount+=1) {

		const float CurrentAngle=StartAngleRadians+
			((AngleRangeRadians/nLayerCount)*nCount);

		nCosLayerAngle[nCount]=static_cast<int>(cosf(CurrentAngle)*nFPMult);
		nSinLayerAngle[nCount]=static_cast<int>(sinf(CurrentAngle)*nFPMult);

		nLayerScale[nCount]=static_cast<int>(
			(1.0f+((EndScale/nLayerCount)*nCount))*nFPMult);

	}

	const int nReciprocalLayerCount=
		((1<<16)/nLayerCount);

	const int nOriginX=(pSettings->m_OriginX*nWidth*nFPMult);
	const int nOriginY=(pSettings->m_OriginY*nHeight*nFPMult);

	const int nStartX=-nOriginX;
	const int nEndX=nStartX+(nWidth*nFPMult);

	const int nStartY=-nOriginY;
	const int nEndY=nStartY+(nHeight*nFPMult);

	const __m64 RecipReg=
		_mm_set_pi16(
		nReciprocalLayerCount,
		nReciprocalLayerCount,
		nReciprocalLayerCount,
		nReciprocalLayerCount);

	const __m64 WriteMask=
		_mm_set_pi32(
		0xffffffff,
		0x00000000);

	int nCurrentY=nStartY;
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=(pOutputLineStart+nWidth);
		
		int nSourceX[nMaxLayerCount];
		int nSourceY[nMaxLayerCount];
		int nSourceXInc[nMaxLayerCount];
		int nSourceYInc[nMaxLayerCount];

		int nCount;
		for (nCount=0; nCount<nLayerCount; nCount+=1) {

			const int nCos=nCosLayerAngle[nCount];
			const int nSin=nSinLayerAngle[nCount];
			const int nScale=nLayerScale[nCount];

			int nStartSourceX=
				((nCos>>8)*(nStartX>>8));

			nStartSourceX+=
				((nSin>>8)*(nCurrentY>>8));

			nStartSourceX=
				((nStartSourceX>>8)*(nScale>>8));

			nStartSourceX-=nStartX;

			int nStartSourceY=
				((nCos>>8)*(nCurrentY>>8));

			nStartSourceY-=
				((nSin>>8)*(nStartX>>8));

			nStartSourceY=
				((nStartSourceY>>8)*(nScale>>8));

			nStartSourceY-=nStartY;

			int nEndSourceX=
				((nCos>>8)*(nEndX>>8));

			nEndSourceX+=
				((nSin>>8)*(nCurrentY>>8));

			nEndSourceX=
				((nEndSourceX>>8)*(nScale>>8));

			nEndSourceX-=nStartX;

			int nEndSourceY=
				((nCos>>8)*(nCurrentY>>8));

			nEndSourceY-=
				((nSin>>8)*(nEndX>>8));

			nEndSourceY=
				((nEndSourceY>>8)*(nScale>>8));

			nEndSourceY-=nStartY;

			nSourceX[nCount]=nStartSourceX;
			nSourceY[nCount]=nStartSourceY;
			nSourceXInc[nCount]=((nEndSourceX-nStartSourceX)/nWidth);
			nSourceYInc[nCount]=((nEndSourceY-nStartSourceY)/nWidth);

		}

		__asm {

			pxor		mm0,mm0
			movq		mm3,RecipReg

		}

		while (pCurrentOutput<pOutputLineEnd) {

//			int nRedTotal=0;
//			int nGreenTotal=0;
//			int nBlueTotal=0;

			__asm {

				pxor		mm1,mm1

			}
			
			int nCount;
			for (nCount=0; nCount<nLayerCount; nCount+=1) {

				int nCurrentSourceX=(nSourceX[nCount])>>nFPShift;
				nCurrentSourceX=GateInt(nCurrentSourceX,0,(nWidth-1));

				int nCurrentSourceY=(nSourceY[nCount])>>nFPShift;
				nCurrentSourceY=GateInt(nCurrentSourceY,0,(nHeight-1));

				U32* pCurrentSource=
					pSource+
					(nCurrentSourceY*nWidth)+
					nCurrentSourceX;

				__asm {

					mov			esi, pCurrentSource

					movq		mm2,[esi]
					punpcklbw	mm2,mm0

					paddw		mm1,mm2

				}

//				const U32 SourceColour=*pCurrentSource;
//
//				const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
//				const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
//				const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
//
//				nRedTotal+=nSourceRed;
//				nGreenTotal+=nSourceGreen;
//				nBlueTotal+=nSourceBlue;

				nSourceX[nCount]+=nSourceXInc[nCount];
				nSourceY[nCount]+=nSourceYInc[nCount];

			}	

			__asm {

				movq		mm7,mm1
				pmulhw		mm7,mm3

				packuswb	mm7,mm0
				mov			esi,pCurrentOutput
				movq		mm6,[esi]
				pand		mm6,WriteMask
				por			mm7,mm6
				movq		[esi],mm7

			}

//			const int nAverageRed=(nRedTotal*nReciprocalLayerCount)>>16;
//			const int nAverageGreen=(nGreenTotal*nReciprocalLayerCount)>>16;
//			const int nAverageBlue=(nBlueTotal*nReciprocalLayerCount)>>16;
//
//			const U32 OutputColour=
//				(nAverageRed<<SHIFT_RED)|
//				(nAverageGreen<<SHIFT_GREEN)|
//				(nAverageBlue);
//
//			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		nCurrentY+=nFPMult;

	}

	_m_empty();

}

#else // PETE_USE_C

void Pete_SpiralBlur_Render(SPete_SpiralBlur_Data* pInstanceData,SPete_SpiralBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFPShift=16;
	const int nFPMult=(1<<nFPShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const int nHalfWidth=static_cast<int>((Width/2.0f)*nFPMult);
	const int nHalfHeight=static_cast<int>((Height/2.0f)*nFPMult);

	const int nOriginX=(pSettings->m_OriginX*nWidth*nFPMult);
	const int nOriginY=(pSettings->m_OriginY*nHeight*nFPMult);

	const float AngleRange=pSettings->m_Angle;
	const float AngleRangeRadians=(AngleRange/360.0f)*Pete_TwoPi;

	const float StartAngleRadians=(-AngleRangeRadians/2.0f);

	const float EndScale=(pSettings->m_Scale-1.0f);

	const int nLayerCount=static_cast<int>(pSettings->m_LayerCount);

	const int nPixelsCount=(nWidth*nHeight);

	U32* pOutputStart=pOutput;
	U32* pOutputEnd=(pOutput+nPixelsCount);

	U32* pCurrentOutput=pOutputStart;

	int nCosLayerAngle[nMaxLayerCount];
	int nSinLayerAngle[nMaxLayerCount];
	int nLayerScale[nMaxLayerCount];

	int nCount;
	for (nCount=0; nCount<nLayerCount; nCount+=1) {

		const float CurrentAngle=StartAngleRadians+
			((AngleRangeRadians/nLayerCount)*nCount);

		nCosLayerAngle[nCount]=static_cast<int>(cos(CurrentAngle)*nFPMult);
		nSinLayerAngle[nCount]=static_cast<int>(sin(CurrentAngle)*nFPMult);

		nLayerScale[nCount]=static_cast<int>(
			(1.0f+((EndScale/nLayerCount)*nCount))*nFPMult);

	}

	const int nReciprocalLayerCount=
		((1<<16)/nLayerCount);

	const int nStartX=-nOriginX;
	const int nEndX=nStartX+(nWidth*nFPMult);

	const int nStartY=-nOriginY;
	const int nEndY=nStartY+(nHeight*nFPMult);

	int nCurrentY=nStartY;
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=(pOutputLineStart+nWidth);
		
		int nSourceX[nMaxLayerCount];
		int nSourceY[nMaxLayerCount];
		int nSourceXInc[nMaxLayerCount];
		int nSourceYInc[nMaxLayerCount];

		int nCount;
		for (nCount=0; nCount<nLayerCount; nCount+=1) {

			const int nCos=nCosLayerAngle[nCount];
			const int nSin=nSinLayerAngle[nCount];
			const int nScale=nLayerScale[nCount];

			int nStartSourceX=
				((nCos>>8)*(nStartX>>8));

			nStartSourceX+=
				((nSin>>8)*(nCurrentY>>8));

			nStartSourceX=
				((nStartSourceX>>8)*(nScale>>8));

			nStartSourceX-=nStartX;

			int nStartSourceY=
				((nCos>>8)*(nCurrentY>>8));

			nStartSourceY-=
				((nSin>>8)*(nStartX>>8));

			nStartSourceY=
				((nStartSourceY>>8)*(nScale>>8));

			nStartSourceY-=nStartY;

			int nEndSourceX=
				((nCos>>8)*(nEndX>>8));

			nEndSourceX+=
				((nSin>>8)*(nCurrentY>>8));

			nEndSourceX=
				((nEndSourceX>>8)*(nScale>>8));

			nEndSourceX-=nStartX;

			int nEndSourceY=
				((nCos>>8)*(nCurrentY>>8));

			nEndSourceY-=
				((nSin>>8)*(nEndX>>8));

			nEndSourceY=
				((nEndSourceY>>8)*(nScale>>8));

			nEndSourceY-=nStartY;

			nSourceX[nCount]=nStartSourceX;
			nSourceY[nCount]=nStartSourceY;
			nSourceXInc[nCount]=((nEndSourceX-nStartSourceX)/nWidth);
			nSourceYInc[nCount]=((nEndSourceY-nStartSourceY)/nWidth);

		}

		while (pCurrentOutput<pOutputLineEnd) {

			int nTotalRed=0;
			int nTotalGreen=0;
			int nTotalBlue=0;
			int nTotalAlpha=0;

			int nCount;
			for (nCount=0; nCount<nLayerCount; nCount+=1) {

				int nCurrentSourceX=(nSourceX[nCount])>>nFPShift;
				nCurrentSourceX=GateInt(nCurrentSourceX,0,(nWidth-1));

				int nCurrentSourceY=(nSourceY[nCount])>>nFPShift;
				nCurrentSourceY=GateInt(nCurrentSourceY,0,(nHeight-1));

				U32* pCurrentSource=
					pSource+
					(nCurrentSourceY*nWidth)+
					nCurrentSourceX;

				const U32 SourceColour=*pCurrentSource;

				const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;
				
				nTotalRed+=nSourceRed;
				nTotalGreen+=nSourceGreen;
				nTotalBlue+=nSourceBlue;
				nTotalAlpha+=nSourceAlpha;
	
				nSourceX[nCount]+=nSourceXInc[nCount];
				nSourceY[nCount]+=nSourceYInc[nCount];

			}	

			const int nAverageRed=(nTotalRed*nReciprocalLayerCount)>>16;
			const int nAverageGreen=(nTotalGreen*nReciprocalLayerCount)>>16;
			const int nAverageBlue=(nTotalBlue*nReciprocalLayerCount)>>16;
			const int nAverageAlpha=(nTotalAlpha*nReciprocalLayerCount)>>16;

			const U32 OutputColour=
				(nAverageRed<<SHIFT_RED)|
				(nAverageGreen<<SHIFT_GREEN)|
				(nAverageBlue<<SHIFT_BLUE)|
				(nAverageAlpha<<SHIFT_ALPHA);

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		nCurrentY+=nFPMult;

	}

}

#endif // PETE_USE_C
