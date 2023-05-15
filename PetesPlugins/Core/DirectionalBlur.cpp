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

#include "DirectionalBlur.h"

#include "math.h"

#include "PeteHelpers.h"

void Pete_DoDirectionalBoxFilterBlur(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSize,float Angle,int nWidth,int nHeight,int nPassCount);

static SPete_Parameter g_Parameters[]={
	{
		"Radius",
		5.0f,
		1.0f,
		63.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);


void Pete_DirectionalBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_DirectionalBlur_Init(SPete_DirectionalBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_DirectionalBlur_DeInit(pInstanceData);

	const int nChannels=3;
	const int nSizeOfIntermediateBuffer=
		nWidth*nHeight*nChannels*sizeof(float);

	pInstanceData->hIntermediateBuffer=
		Pete_NewHandle(nSizeOfIntermediateBuffer);

	if (pInstanceData->hIntermediateBuffer==NULL) {
		Pete_DirectionalBlur_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_DirectionalBlur_DeInit(SPete_DirectionalBlur_Data* pInstanceData) {

	if (pInstanceData->hIntermediateBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hIntermediateBuffer);
		pInstanceData->hIntermediateBuffer=NULL;
	}


}

void Pete_DirectionalBlur_Render(SPete_DirectionalBlur_Data* pInstanceData,SPete_DirectionalBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	U32* pIntermediateBuffer=
		(U32*)Pete_LockHandle(pInstanceData->hIntermediateBuffer);
	if (pIntermediateBuffer==NULL) {
		return;
	}

	const int nMaxIterations=3;

	const float Radius=pSettings->m_Radius;

	const int nBoxFilterSize=
		(int)(ceilf(Radius));

	const float Angle=pSettings->m_Angle;

	Pete_DoDirectionalBoxFilterBlur(
		pSource,
		pIntermediateBuffer,
		pOutput,
		nBoxFilterSize,
		Angle,
		pInstanceData->nWidth,
		pInstanceData->nHeight,
		nMaxIterations);


	Pete_UnLockHandle(pInstanceData->hIntermediateBuffer);

}

void Pete_DoDirectionalBoxFilterBlur(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSize,float Angle,int nWidth,int nHeight,int nPassCount) {

	if ((pSourceBuffer==NULL)||
		(pIntermediateBuffer==NULL)||
		(pOutputBuffer==NULL)) {
		return;
	}

	nKernelSize=GateInt(nKernelSize,1,((nWidth/2)-1));

	const int nKernelWidth=(nKernelSize*2)+1;

	const int nKernelSizeMult=
		(1<<16)/nKernelWidth;

	const float DirX=cos(Angle);
	const float DirY=sin(Angle);

	const float AbsDirX=fabsf(DirX);
	const float AbsDirY=fabsf(DirY);

	float DirA;
	float DirB;

	int nSideA;
	int nSideB;

	int nAddressIncA;
	int nAddressIncB;

	if (AbsDirY>AbsDirX) {

		DirA=DirX;
		DirB=DirY;

		nSideA=nWidth;
		nSideB=nHeight;

		nAddressIncA=1;
		nAddressIncB=nWidth;

	} else {

		DirA=DirY;
		DirB=DirX;

		nSideA=nHeight;
		nSideB=nWidth;

		nAddressIncA=nWidth;
		nAddressIncB=1;

	}

	int nIncB;
	int nLineOriginB;
	int nLineEndB;
	if (DirB>0.0f) {
		nIncB=1;
		nLineOriginB=0;
		nLineEndB=nSideB;
	} else {
		nIncB=-1;
		nLineOriginB=(nSideB-1);
		nLineEndB=-1;
	}
	const int nDeltaB=(nIncB*nSideB);
	const int nAbsDeltaB=AbsInt(nDeltaB);
	const int nDeltaA=(int)((nDeltaB/DirB)*DirA);
	const int nAbsDeltaA=AbsInt(nDeltaA);

	int nStartA;
	int nEndA;
	int nIncA;
	if (DirA>0.0f) {
		nStartA=-nAbsDeltaA;
		nEndA=nSideA;
		nIncA=1;
	} else {
		nStartA=0;
		nEndA=(nSideA+nAbsDeltaA);
		nIncA=-1;
	}

	int nLineOriginA;
	for (nLineOriginA=nStartA; nLineOriginA<nEndA; nLineOriginA+=1) {

		int nCurrentPass;
		for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

			U32* pSource;
			U32* pOutput;

			if (nCurrentPass==0) {
				pSource=pSourceBuffer;
				pOutput=pIntermediateBuffer;
			} else if ((nCurrentPass&0x1)==1) {
				pSource=pIntermediateBuffer;
				pOutput=pOutputBuffer;
			} else {
				pSource=pOutputBuffer;
				pOutput=pIntermediateBuffer;
			}

			int nCounter;
			int nClippedLineOriginA;
			int nClippedLineOriginB;

			if (nLineOriginA<0) {

				const int nClipDistA=(0-nLineOriginA);
				const int nClipTimesDeltaB=(nClipDistA*nDeltaB);
				int nClipDistB;
				if (nClipTimesDeltaB>0) {
					nClipDistB=(nClipTimesDeltaB+(nDeltaA-1))/nDeltaA;
					nCounter=((nClipDistB*nDeltaA)-nClipTimesDeltaB);
				} else {
					nClipDistB=(nClipTimesDeltaB-(nDeltaA-1))/nDeltaA;
					nCounter=-((nClipDistB*nDeltaA)-nClipTimesDeltaB);
				}
				nClippedLineOriginB=(nLineOriginB+nClipDistB);
				nClippedLineOriginA=0;

			} else if (nLineOriginA>=nSideA) {

				const int nClipDistA=(nLineOriginA-(nSideA-1));
				const int nClipTimesDeltaB=(nClipDistA*nDeltaB);
				int nClipDistB;
				if (nClipTimesDeltaB>0) {
					nClipDistB=(nClipTimesDeltaB+(nAbsDeltaA-1))/nAbsDeltaA;
					nCounter=((nClipDistB*nAbsDeltaA)-nClipTimesDeltaB);
				} else {
					nClipDistB=(nClipTimesDeltaB-(nAbsDeltaA-1))/nAbsDeltaA;
					nCounter=-((nClipDistB*nAbsDeltaA)-nClipTimesDeltaB);
				}

				nClippedLineOriginB=(nLineOriginB+nClipDistB);
				nClippedLineOriginA=(nSideA-1);

			} else {

				nClippedLineOriginA=nLineOriginA;
				nClippedLineOriginB=nLineOriginB;
				nCounter=0;

			}

			const int nLineEndA=(nLineOriginA+nDeltaA);

			int nClippedLineEndB;
			if (nLineEndA<0) {

				const int nDistA=(nClippedLineOriginA+1);
				const int nDistB=((nAbsDeltaB*nDistA)+(nAbsDeltaA-1)-nCounter)/nAbsDeltaA;
				nClippedLineEndB=(nClippedLineOriginB+(nDistB*nIncB));

			} else if (nLineEndA>=nSideA) {

				const int nDistA=(nSideA-nClippedLineOriginA);
				const int nDistB=((nAbsDeltaB*nDistA)+(nAbsDeltaA-1)-nCounter)/nAbsDeltaA;
				nClippedLineEndB=(nClippedLineOriginB+(nDistB*nIncB));

			} else {

				nClippedLineEndB=nLineEndB;

			}

			int nCurrentA=nClippedLineOriginA;
			int nCurrentB=nClippedLineOriginB;

			int nCurrentAddressOffset=((nCurrentB*nAddressIncB)+(nCurrentA*nAddressIncA));
			int nLeftAddressOffset=nCurrentAddressOffset;
			int nRightAddressOffset=nCurrentAddressOffset;

			int nLeftCounter=nCounter;
			int nRightCounter=nCounter;

			const int nSignedAddressIncA=(nIncA*nAddressIncA);
			const int nSignedAddressIncB=(nIncB*nAddressIncB);

			U32 FirstPixelColour;
			if (nCurrentB!=nClippedLineEndB) {
				FirstPixelColour=*(pSource+nCurrentAddressOffset);
			} else {
				FirstPixelColour=0;
			}
			const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
			const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
			const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;
			const int nFirstPixelAlpha=(FirstPixelColour>>SHIFT_ALPHA)&0xff;

			int nRedTotal=(nFirstPixelRed*(nKernelSize-1));
			int nGreenTotal=(nFirstPixelGreen*(nKernelSize-1));
			int nBlueTotal=(nFirstPixelBlue*(nKernelSize-1));
			int nAlphaTotal=(nFirstPixelAlpha*(nKernelSize-1));

			int nRightRed=nFirstPixelRed;
			int nRightGreen=nFirstPixelGreen;
			int nRightBlue=nFirstPixelBlue;
			int nRightAlpha=nFirstPixelAlpha;

			int nK;
			int nRightB;
			int nPixelRed=0;
			int nPixelGreen=0;
			int nPixelBlue=0;
			int nPixelAlpha=0;
			for (nK=1,nRightB=nCurrentB; (nK<=nKernelSize)&&(nRightB!=nClippedLineEndB); nK+=1,nRightB+=nIncB) {

				U32* pRightSample=(pSource+nRightAddressOffset);

				U32 PixelColour=*pRightSample;
				nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
				nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
				nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
				nPixelAlpha=(PixelColour>>SHIFT_ALPHA)&0xff;

				nRedTotal+=nPixelRed;
				nGreenTotal+=nPixelGreen;
				nBlueTotal+=nPixelBlue;
				nAlphaTotal+=nPixelAlpha;

				nRightCounter+=nAbsDeltaA;
				nRightAddressOffset+=nSignedAddressIncB;
				if (nRightCounter>=nAbsDeltaB) {
					nRightCounter-=nAbsDeltaB;
					nRightAddressOffset+=nSignedAddressIncA;
				}

			}

			const int nRemainingIterations=((nKernelSize+1)-nK);
			nRedTotal+=(nPixelRed*nRemainingIterations);
			nGreenTotal+=(nPixelGreen*nRemainingIterations);
			nBlueTotal+=(nPixelBlue*nRemainingIterations);
			nAlphaTotal+=(nPixelAlpha*nRemainingIterations);


			int nLeftStart=nKernelSize;
			const int nUnsignedDistance=((nClippedLineEndB-nCurrentB)*nIncB);
			int nRightEnd=(nUnsignedDistance-nKernelSize);

			int nClipPos=0;

			for (; nCurrentB!=nClippedLineEndB; nCurrentB+=nIncB) {

				U32* pCurrentSource=(pSource+nCurrentAddressOffset);
				U32* pCurrentOutput=(pOutput+nCurrentAddressOffset);

				const int nOutputRed=(nRedTotal*nKernelSizeMult)>>16;
				const int nOutputGreen=(nGreenTotal*nKernelSizeMult)>>16;
				const int nOutputBlue=(nBlueTotal*nKernelSizeMult)>>16;
				const int nOutputAlpha=(nAlphaTotal*nKernelSizeMult)>>16;

				const U32 OutputColour=
					(nOutputRed<<SHIFT_RED)|
					(nOutputGreen<<SHIFT_GREEN)|
					(nOutputBlue<<SHIFT_BLUE)|
					(nOutputAlpha<<SHIFT_ALPHA);

				*pCurrentOutput=OutputColour;

				int nLeftRed;
				int nLeftGreen;
				int nLeftBlue;
				int nLeftAlpha;

				if (nClipPos<nLeftStart) {

					nLeftRed=nFirstPixelRed;
					nLeftGreen=nFirstPixelGreen;
					nLeftBlue=nFirstPixelBlue;
					nLeftAlpha=nFirstPixelAlpha;

				} else {

					U32* pLeftSample=pSource+nLeftAddressOffset;
					U32 LeftColour=*pLeftSample;
					nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
					nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
					nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;
					nLeftAlpha=(LeftColour>>SHIFT_ALPHA)&0xff;

					nLeftCounter+=nAbsDeltaA;
					nLeftAddressOffset+=nSignedAddressIncB;
					if (nLeftCounter>=nAbsDeltaB) {
						nLeftCounter-=nAbsDeltaB;
						nLeftAddressOffset+=nSignedAddressIncA;
					}

				}

				if (nClipPos<nRightEnd) {

					U32* pRightSample=pSource+nRightAddressOffset;
					U32 RightColour=*pRightSample;
					nRightRed=(RightColour>>SHIFT_RED)&0xff;
					nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
					nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;
					nRightAlpha=(RightColour>>SHIFT_ALPHA)&0xff;

					nRightCounter+=nAbsDeltaA;
					nRightAddressOffset+=nSignedAddressIncB;
					if (nRightCounter>=nAbsDeltaB) {
						nRightCounter-=nAbsDeltaB;
						nRightAddressOffset+=nSignedAddressIncA;
					}

				}

				nRedTotal-=nLeftRed;
				nRedTotal+=nRightRed;

				nGreenTotal-=nLeftGreen;
				nGreenTotal+=nRightGreen;

				nBlueTotal-=nLeftBlue;
				nBlueTotal+=nRightBlue;

				nAlphaTotal-=nLeftAlpha;
				nAlphaTotal+=nRightAlpha;

				nClipPos+=1;

				nCounter+=nAbsDeltaA;
				nCurrentAddressOffset+=nSignedAddressIncB;
				if (nCounter>=nAbsDeltaB) {
					nCounter-=nAbsDeltaB;
					nCurrentAddressOffset+=nSignedAddressIncA;
				}

			}

		}

	}

}

