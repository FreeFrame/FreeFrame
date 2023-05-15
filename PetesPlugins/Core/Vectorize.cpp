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


#include "Vectorize.h"

#include "math.h"
//#include "mmintrin.h"

#include "PeteHelpers.h"
#include "BoxFilter.h"
#include "ColourReduce.h"

struct SVectorizeImage {
	int nWidth;
	int nHeight;
	U32* pInstanceData;
};

const int nMaxDiffThreshold=static_cast<int>(0.19f*255*3);

int Pete_Vectorize_SetupMipMapTable(SPete_Vectorize_Data* pInstanceData);
void Pete_Vectorize_CreateMipMaps(SPete_Vectorize_Data* pInstanceData,U32* pFullImage);
void Pete_HalveImageSize(U32* pSource,int nWidth,int nHeight,U32* pOutput);
void Pete_Vectorize_Quantize(SPete_Vectorize_Data* pInstanceData,SPete_Vectorize_Settings* pSettings,U32* pFullImage);
void Pete_AutoThresholdImage(U32* pFullImage,int nWidth,int nHeight);
void Pete_SampleLuminance(
	U32* pSource,
	int nWidth,
	int nHeight,
	int nSampleSpacing,
	float LowPercentile,
	float HighPercentile,
	int* poutLowLuminance,
	int* poutHighLuminance);
void Pete_Vectorize_DoCell(int nX,int nY,int nWidth,int nHeight,int nDiffThreshold,SVectorizeImage* pImage);
void Pete_Vectorize_GetEdgePosition(int nIndex,int nDeltaX,int nDeltaY,int* poutOffsetX,int* poutOffsetY);
void Pete_Vectorize_DrawSolidCell(
	int nLeftX,
	int nTopY,
	int nDeltaX,
	int nDeltaY,
	U32 StartingColour,
	SVectorizeImage* pImage);
void Pete_Vectorize_DrawBisectedCell(
	int nCornerX,
	int nCornerY,
	int nDeltaX,
	int nDeltaY,
	int nTransitionX0,
	int nTransitionY0,
	int nTransitionX1,
	int nTransitionY1,
	U32 StartingColour,
	U32 AlternateColour,
	SVectorizeImage* pImage);
inline void Pete_Vectorize_FillSpanLine(U32* pOutput,int nDeltaX,U32 Colour);
U32 Pete_Vectorize_GetCellAverage(
	int nCornerX,
	int nCornerY,
	int nCellWidth,
	int nCellHeight,
	int nTransitionX0,
	int nTransitionY0,
	int nTransitionX1,
	int nTransitionY1,
	U32 StartingColour,
	U32 AlternateColour,
	SVectorizeImage* pImage);
U32 Pete_Vectorize_GetImageAverage(int nLeftX,int nTopY,int nDeltaX,int nDeltaY,SVectorizeImage* pImage);

static SPete_Parameter g_Parameters[]={
	{
		"Pre-Filter Size",
		5.0f,
		1.0f,
		64.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Resolution",
		8.0f,
		1.0f,
		128.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Monochrome",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Colour Count",
		4.0f,
		1.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Palette Persistence",
		0.99f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Difference Threshold",
		0.1f,
		0.0f,
		0.2f,
		PETE_PARAM_FLOAT
	},

};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Vectorize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Vectorize_Init(SPete_Vectorize_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Vectorize_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	pInstanceData->hIntermediateData=
		Pete_NewHandle(nWidth*nHeight*sizeof(U32)*2);
	if (pInstanceData->hIntermediateData==NULL) {
		Pete_Vectorize_DeInit(pInstanceData);
		return 0;
	}

	const int nColourReduceInitResult=
		Pete_ColourReduce_Init(
		&(pInstanceData->ColourReduceInstanceData),
		nWidth,
		nHeight);

	if (nColourReduceInitResult!=1) {
		Pete_Vectorize_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_Vectorize_DeInit(SPete_Vectorize_Data* pInstanceData) {

	if (pInstanceData->hIntermediateData!=NULL) {
		Pete_FreeHandle(pInstanceData->hIntermediateData);
		pInstanceData->hIntermediateData=NULL;
	}

	Pete_ColourReduce_DeInit(&(pInstanceData->ColourReduceInstanceData));

}

void Pete_Vectorize_Render(SPete_Vectorize_Data* pInstanceData,SPete_Vectorize_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nBoxFilterSize=
		GateInt((int)(pSettings->m_PreFilterSize),1,255);

	U32* pIntermediateData=(U32*)Pete_LockHandle(pInstanceData->hIntermediateData);

	if (pIntermediateData==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	Pete_DoBoxFilterBlur(
		pSource,
		pIntermediateData,
		pOutput,
		nBoxFilterSize,
		nBoxFilterSize,
		nWidth,
		nHeight,
		1);

	Pete_Vectorize_Quantize(pInstanceData,pSettings,pOutput);

	const int nTotalCellCount=
		static_cast<int>(pSettings->m_CellCount);

	const int nCellCountX=(nTotalCellCount*3)/2;
	const int nCellCountY=nTotalCellCount;

	const int nCellWidth=nWidth/nCellCountX;
	const int nCellHeight=nHeight/nCellCountY;

	const int nDiffThreshold=
		static_cast<int>(pSettings->m_DifferenceThreshold*256*3);

	SVectorizeImage SourceImage;
	SourceImage.nWidth=nWidth;
	SourceImage.nHeight=nHeight;
	SourceImage.pInstanceData=pOutput;

	int nCellY;
	for (nCellY=0; nCellY<nCellCountY; nCellY+=1) {

		int nCellX;
		for (nCellX=0; nCellX<nCellCountX; nCellX+=1) {

			Pete_Vectorize_DoCell(
				(nCellX*nCellWidth),
				(nCellY*nCellHeight),
				nCellWidth,
				nCellHeight,
				nDiffThreshold,
				&SourceImage);

		}

	}

}

inline void Pete_Vectorize_FillSpanLine(U32* pOutput,int nDeltaX,U32 Colour) {

	U32* pOutputEnd=pOutput+nDeltaX;
	while (pOutput<pOutputEnd) {
		*pOutput=Colour;
		pOutput+=1;
	}

}

void Pete_Vectorize_Quantize(SPete_Vectorize_Data* pInstanceData,SPete_Vectorize_Settings* pSettings,U32* pFullImage) {

	if (pSettings->m_Monochrome>0.0f) {

		const int nWidth=pInstanceData->nWidth;
		const int nHeight=pInstanceData->nHeight;

		Pete_AutoThresholdImage(pFullImage,nWidth,nHeight);

	} else {

		SPete_ColourReduce_Settings ColourReduceSettings;
		ColourReduceSettings.m_TargetColourCount=pSettings->m_ColourReduceLevels;
		ColourReduceSettings.m_PalettePersistence=pSettings->m_ColourReducePalettePersistence;

		Pete_ColourReduce_Render(
			&(pInstanceData->ColourReduceInstanceData),
			&ColourReduceSettings,
			pFullImage,
			pFullImage);

	}

}

void Pete_AutoThresholdImage(U32* pFullImage,int nWidth,int nHeight) {

	int nLowLuminance;
	int nHighLuminance;
	const int nSampleSpacing=8;
	const float LowPercentile=45.0f;
	const float HighPercentile=55.0f;
	Pete_SampleLuminance(
		pFullImage,
		nWidth,
		nHeight,
		nSampleSpacing,
		LowPercentile,
		HighPercentile,
		&nLowLuminance,
		&nHighLuminance);

	const int nThreshold=
		((nLowLuminance+nHighLuminance)*256)/2;

	const int nNumPixels=(nWidth*nHeight);

	U32* pCurrentOutput=pFullImage;
	U32* pOutputEnd=pFullImage+nNumPixels;

	while (pCurrentOutput<pOutputEnd) {

		U32 CurrentColour=*pCurrentOutput;
		const int nLuminance=GetLuminance(CurrentColour);

		if (nLuminance<nThreshold) {
			*pCurrentOutput=0x00000000;
		} else {
			*pCurrentOutput=0xffffffff;
		}

		pCurrentOutput+=1;

	}

}

void Pete_SampleLuminance(
	U32* pSource,
	int nWidth,
	int nHeight,
	int nSampleSpacing,
	float LowPercentile,
	float HighPercentile,
	int* poutLowLuminance,
	int* poutHighLuminance) {


	int	nRedHistogram[256];
	int	nGreenHistogram[256];
	int	nBlueHistogram[256];

	Pete_ZeroMemory(&nRedHistogram[0],256*sizeof(int));
	Pete_ZeroMemory(&nGreenHistogram[0],256*sizeof(int));
	Pete_ZeroMemory(&nBlueHistogram[0],256*sizeof(int));

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentSource=pSource;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource<pSourceEnd) {

		U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		while (pCurrentSource<pSourceLineEnd) {

			U32 SourceColour=*pCurrentSource;

			const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;

			nRedHistogram[nSourceRed]+=1;
			nGreenHistogram[nSourceGreen]+=1;
			nBlueHistogram[nSourceBlue]+=1;

			pCurrentSource+=nSampleSpacing;

		}

		pCurrentSource=
			pSourceLineStart+(nSampleSpacing*nWidth);

	}

	const int nSampleCount=
		(nWidth/nSampleSpacing)*
		(nHeight/nSampleSpacing);

	const int nStartThreshold=static_cast<int>((LowPercentile*nSampleCount)/100.0f);
	const int nEndThreshold=static_cast<int>((HighPercentile*nSampleCount)/100.0f);

	int nCurrentRedTotal;
	int nCurrentGreenTotal;
	int nCurrentBlueTotal;
	int nCurrentSlot;

	nCurrentRedTotal=0;
	nCurrentSlot=0;
	while ((nCurrentRedTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentRedTotal+=nRedHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nRedLow=(nCurrentSlot-1);

	nCurrentRedTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentRedTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentRedTotal-=nRedHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nRedHigh=(nCurrentSlot+1);

	nCurrentGreenTotal=0;
	nCurrentSlot=0;
	while ((nCurrentGreenTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentGreenTotal+=nGreenHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nGreenLow=(nCurrentSlot-1);

	nCurrentGreenTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentGreenTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentGreenTotal-=nGreenHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nGreenHigh=(nCurrentSlot+1);

	nCurrentBlueTotal=0;
	nCurrentSlot=0;
	while ((nCurrentBlueTotal<nStartThreshold)&&(nCurrentSlot<256)) {
		nCurrentBlueTotal+=nBlueHistogram[nCurrentSlot];
		nCurrentSlot+=1;
	}

	const int nBlueLow=(nCurrentSlot-1);

	nCurrentBlueTotal=nSampleCount;
	nCurrentSlot=255;
	while ((nCurrentBlueTotal>nEndThreshold)&&(nCurrentSlot>=0)) {
		nCurrentBlueTotal-=nBlueHistogram[nCurrentSlot];
		nCurrentSlot-=1;
	}

	const int nBlueHigh=(nCurrentSlot+1);

	const int nLowLuminance =
		((90 * nRedLow)+
		(115 * nGreenLow)+
		(51 * nBlueLow))/256;

	const int nHighLuminance =
		((90 * nRedHigh)+
		(115 * nGreenHigh)+
		(51 * nBlueHigh))/256;

	*poutHighLuminance=nHighLuminance;
	*poutLowLuminance=nLowLuminance;

}

void Pete_Vectorize_GetEdgePosition(int nIndex,int nDeltaX,int nDeltaY,int* poutOffsetX,int* poutOffsetY) {

	const int nEdgePixelCount=(nDeltaX*2)+(nDeltaY*2);

	nIndex=(nIndex%nEdgePixelCount);

	int nResultX;
	int nResultY;

	if (nIndex<nDeltaX) {

		nResultX=nIndex;
		nResultY=0;

	} else {

		nIndex-=nDeltaX;

		if (nIndex<nDeltaY) {

			nResultX=nDeltaX;
			nResultY=nIndex;

		} else {

			nIndex-=nDeltaY;

			if (nIndex<nDeltaX) {

				nResultX=nDeltaX-nIndex;
				nResultY=nDeltaY;

			} else {

				nIndex-=nDeltaX;

				nResultX=0;
				nResultY=nDeltaY-nIndex;

			}


		}

	}

	*poutOffsetX=nResultX;
	*poutOffsetY=nResultY;

}

void Pete_Vectorize_DoCell(int nX,int nY,int nWidth,int nHeight,int nDiffThreshold,SVectorizeImage* pImage) {

	const int nFullWidth=pImage->nWidth;
	const int nFullHeight=pImage->nHeight;
	U32* pImageData=pImage->pInstanceData;

	int nLeftX=nX;
	nLeftX=GateInt(nLeftX,0,(nFullWidth-1));

	int nTopY=nY;
	nTopY=GateInt(nTopY,0,(nFullHeight-1));

	int nRightX=nX+nWidth;
	nRightX=GateInt(nRightX,0,(nFullWidth-1));

	int nBottomY=nY+nHeight;
	nBottomY=GateInt(nBottomY,0,(nFullHeight-1));

	const int nDeltaX=nRightX-nLeftX;
	const int nDeltaY=nBottomY-nTopY;

	int nTransitionCount=0;
	int nTransitionX[2];
	int nTransitionY[2];

	U32* pStartingSource=
		pImageData+(nTopY*nFullWidth)+nLeftX;

	U32 StartingColour=*pStartingSource;
	U32 PreviousColour=StartingColour;
	U32 AlternateColour;

	bool bFoundTooManyTransitions=false;

	const int nEdgePixelCount=(nDeltaX*2)+(nDeltaY*2);

	int nCount;
	for (nCount=1;(nCount<=nEdgePixelCount)&&(!bFoundTooManyTransitions);nCount+=1) {

		int nOffsetX;
		int nOffsetY;
		Pete_Vectorize_GetEdgePosition(nCount,nDeltaX,nDeltaY,&nOffsetX,&nOffsetY);

		int nCurrentX=nLeftX+nOffsetX;
		int nCurrentY=nTopY+nOffsetY;

		U32* pCurrentSource=
			pImageData+(nCurrentY*nFullWidth)+nCurrentX;

		U32 CurrentColour=*pCurrentSource;

		if (CurrentColour!=PreviousColour) {

			if (nTransitionCount==0) {

				AlternateColour=CurrentColour;

				nTransitionX[0]=nCurrentX;
				nTransitionY[0]=nCurrentY;

				nTransitionCount=1;

			} else if ((nTransitionCount==1)&&(CurrentColour==StartingColour)) {

				nTransitionX[1]=nCurrentX;
				nTransitionY[1]=nCurrentY;

				nTransitionCount=1;

			} else {

				bFoundTooManyTransitions=true;

			}

		}

		PreviousColour=CurrentColour;

	}

	const bool bCheckDifference=(nDiffThreshold<nMaxDiffThreshold);
	bool bDifferenceTolerable;

	if (!bFoundTooManyTransitions&&bCheckDifference&&(nDeltaX>0)&&(nDeltaY>0)) {

		U32 CellAverageColour=
			Pete_Vectorize_GetCellAverage(
			nLeftX,
			nTopY,
			nDeltaX,
			nDeltaY,
			nTransitionX[0],
			nTransitionY[0],
			nTransitionX[1],
			nTransitionY[1],
			StartingColour,
			AlternateColour,
			pImage);

		const int nCellAverageRed=(CellAverageColour>>SHIFT_RED)&0xff;
		const int nCellAverageGreen=(CellAverageColour>>SHIFT_GREEN)&0xff;
		const int nCellAverageBlue=(CellAverageColour>>SHIFT_BLUE)&0xff;

		U32 ImageAverageColour=
			Pete_Vectorize_GetImageAverage(
			nLeftX,
			nTopY,
			nDeltaX,
			nDeltaY,
			pImage);

		const int nImageAverageRed=(ImageAverageColour>>SHIFT_RED)&0xff;
		const int nImageAverageGreen=(ImageAverageColour>>SHIFT_GREEN)&0xff;
		const int nImageAverageBlue=(ImageAverageColour>>SHIFT_BLUE)&0xff;

		const int nRedDiff=AbsInt(nCellAverageRed-nImageAverageRed);
		const int nGreenDiff=AbsInt(nCellAverageGreen-nImageAverageGreen);
		const int nBlueDiff=AbsInt(nCellAverageBlue-nImageAverageBlue);

		const int nTotalDiff=(nRedDiff+nGreenDiff+nBlueDiff);

		if (nTotalDiff<nDiffThreshold) {
			bDifferenceTolerable=true;
		} else {
			bDifferenceTolerable=false;
		}

	}

	if (!bCheckDifference) {
		bDifferenceTolerable=true;
	}

	if (((!bFoundTooManyTransitions)&&(bDifferenceTolerable))||
		(nWidth<=1)||
		(nHeight<=1)) {

		if (nTransitionCount==0) {

			Pete_Vectorize_DrawSolidCell(
				nLeftX,
				nTopY,
				nDeltaX,
				nDeltaY,
				StartingColour,
				pImage);

		} else {

			Pete_Vectorize_DrawBisectedCell(
				nLeftX,
				nTopY,
				nDeltaX,
				nDeltaY,
				nTransitionX[0],
				nTransitionY[0],
				nTransitionX[1],
				nTransitionY[1],
				StartingColour,
				AlternateColour,
				pImage);

		}

	} else {

		const int nHalfWidth=nWidth/2;
		const int nHalfHeight=nHeight/2;

		Pete_Vectorize_DoCell(nX,nY,nHalfWidth,nHalfHeight,nDiffThreshold,pImage);
		Pete_Vectorize_DoCell(nX+nHalfWidth,nY,nHalfWidth,nHalfHeight,nDiffThreshold,pImage);
		Pete_Vectorize_DoCell(nX,nY+nHalfHeight,nHalfWidth,nHalfHeight,nDiffThreshold,pImage);
		Pete_Vectorize_DoCell(nX+nHalfWidth,nY+nHalfHeight,nHalfWidth,nHalfHeight,nDiffThreshold,pImage);

	}

}

void Pete_Vectorize_DrawSolidCell(
	int nLeftX,
	int nTopY,
	int nDeltaX,
	int nDeltaY,
	U32 StartingColour,
	SVectorizeImage* pImage) {

	const int nImageWidth=pImage->nWidth;
	const int nImageHeight=pImage->nHeight;

	U32* pOutputStart=
		pImage->pInstanceData+
		(nTopY*nImageWidth)+
		nLeftX;

	U32* pOutputEnd=
		pOutputStart+
		(nDeltaY*nImageWidth);

	U32* pCurrentOutput=pOutputStart;

	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=pCurrentOutput+nDeltaX;

		while (pCurrentOutput<pOutputLineEnd) {

			*pCurrentOutput=StartingColour;
			pCurrentOutput+=1;

		}

		pCurrentOutput=pOutputLineStart+nImageWidth;

	}
}

void Pete_Vectorize_DrawBisectedCell(
	int nCornerX,
	int nCornerY,
	int nCellWidth,
	int nCellHeight,
	int nTransitionX0,
	int nTransitionY0,
	int nTransitionX1,
	int nTransitionY1,
	U32 StartingColour,
	U32 AlternateColour,
	SVectorizeImage* pImage) {

	int nTopX;
	int nTopY;
	int nBottomX;
	int nBottomY;

	nTopX=nTransitionX0-nCornerX;
	nTopY=nTransitionY0-nCornerY;

	nBottomX=nTransitionX1-nCornerX;
	nBottomY=nTransitionY1-nCornerY;

	const int nImageWidth=pImage->nWidth;
	const int nImageHeight=pImage->nHeight;

	U32* pOutputStart=
		pImage->pInstanceData+
		(nCornerY*nImageWidth)+
		nCornerX;

	const int nDeltaX=(nBottomX-nTopX);
	int nDeltaY=(nBottomY-nTopY);
	if (nDeltaY==0) {
		nDeltaY=1;
	}

	const int nRemainderY=(nCellHeight-nBottomY);

	U32* pCurrentOutput=pOutputStart;

	U32 LeftColour;
	U32 RightColour;

	int nY;
	for (nY=0; nY<nCellHeight; nY+=1) {

		int nTransitionX=nTopX+(((nY-nTopY)*nDeltaX)/nDeltaY);

		U32* pOutputLineStart=pCurrentOutput;

		if (nY==0) {
			if (nTransitionX<=0) {
				RightColour=StartingColour;
				LeftColour=AlternateColour;
			} else {
				RightColour=AlternateColour;
				LeftColour=StartingColour;
			}
		}

		U32* pOutputTransition=pOutputLineStart+nTransitionX;
		U32* pOutputLineEnd=pOutputLineStart+nCellWidth;
		if (pOutputTransition>pOutputLineEnd) {
			pOutputTransition=pOutputLineEnd;
		}

		while (pCurrentOutput<pOutputTransition) {
			*pCurrentOutput=LeftColour;
			pCurrentOutput+=1;
		}

		while (pCurrentOutput<pOutputLineEnd) {
			*pCurrentOutput=RightColour;
			pCurrentOutput+=1;
		}

		pCurrentOutput=pOutputLineStart+nImageWidth;

	}


}

U32 Pete_Vectorize_GetCellAverage(
	int nCornerX,
	int nCornerY,
	int nCellWidth,
	int nCellHeight,
	int nTransitionX0,
	int nTransitionY0,
	int nTransitionX1,
	int nTransitionY1,
	U32 StartingColour,
	U32 AlternateColour,
	SVectorizeImage* pImage) {

	int nTopX;
	int nTopY;
	int nBottomX;
	int nBottomY;

	nTopX=nTransitionX0-nCornerX;
	nTopY=nTransitionY0-nCornerY;

	nBottomX=nTransitionX1-nCornerX;
	nBottomY=nTransitionY1-nCornerY;

	const int nImageWidth=pImage->nWidth;
	const int nImageHeight=pImage->nHeight;

	U32* pOutputStart=
		pImage->pInstanceData+
		(nCornerY*nImageWidth)+
		nCornerX;

	const int nDeltaX=(nBottomX-nTopX);
	int nDeltaY=(nBottomY-nTopY);
	if (nDeltaY==0) {
		nDeltaY=1;
	}

	const int nRemainderY=(nCellHeight-nBottomY);

	U32* pCurrentOutput=pOutputStart;

	U32 LeftColour;
	U32 RightColour;

	int nLeftCount=0;
	int nRightCount=0;

	int nY;
	for (nY=0; nY<nCellHeight; nY+=1) {

		int nTransitionX=nTopX+(((nY-nTopY)*nDeltaX)/nDeltaY);

		U32* pOutputLineStart=pCurrentOutput;

		if (nY==0) {
			if (nTransitionX<=0) {
				RightColour=StartingColour;
				LeftColour=AlternateColour;
			} else {
				RightColour=AlternateColour;
				LeftColour=StartingColour;
			}
		}

		if (nTransitionX<0) {
			nRightCount+=nCellWidth;
		} else if (nTransitionX>=nCellWidth) {
			nLeftCount+=nCellWidth;
		} else {
			nLeftCount+=nTransitionX;
			nRightCount+=(nCellWidth-nTransitionX);
		}

	}

	const int nTotalCount=(nLeftCount+nRightCount);

	const int nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
	const int nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
	const int nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;

	const int nRightRed=(RightColour>>SHIFT_RED)&0xff;
	const int nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
	const int nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;

	const int nAverageRed=
		((nLeftRed*nLeftCount)+
		(nRightRed*nRightCount))/nTotalCount;

	const int nAverageGreen=
		((nLeftGreen*nLeftCount)+
		(nRightGreen*nRightCount))/nTotalCount;

	const int nAverageBlue=
		((nLeftBlue*nLeftCount)+
		(nRightBlue*nRightCount))/nTotalCount;

	const U32 ResultColour=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return ResultColour;
}

U32 Pete_Vectorize_GetImageAverage(int nLeftX,int nTopY,int nDeltaX,int nDeltaY,SVectorizeImage* pImage) {

	const int nImageWidth=pImage->nWidth;
	const int nImageHeight=pImage->nHeight;

	U32* pSourceStart=
		pImage->pInstanceData+
		(nTopY*nImageWidth)+
		nLeftX;

	U32* pSourceEnd=
		pSourceStart+
		(nDeltaY*nImageWidth);

	U32* pCurrentSource=pSourceStart;

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;

	while (pCurrentSource<pSourceEnd) {

		U32* pSourceLineStart=pCurrentSource;
		U32* pSourceLineEnd=pCurrentSource+nDeltaX;

		while (pCurrentSource<pSourceLineEnd) {

			const U32 CurrentColour=*pCurrentSource;

			const int nCurrentRed=(CurrentColour>>SHIFT_RED)&0xff;
			const int nCurrentGreen=(CurrentColour>>SHIFT_GREEN)&0xff;
			const int nCurrentBlue=(CurrentColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nCurrentRed;
			nGreenTotal+=nCurrentGreen;
			nBlueTotal+=nCurrentBlue;

			pCurrentSource+=1;

		}

		pCurrentSource=pSourceLineStart+nImageWidth;

	}

	const int nTotalSamples=(nDeltaX*nDeltaY);

	const int nRedAverage=(nRedTotal/nTotalSamples);
	const int nGreenAverage=(nGreenTotal/nTotalSamples);
	const int nBlueAverage=(nBlueTotal/nTotalSamples);

	return (nRedAverage<<SHIFT_RED)|(nGreenAverage<<SHIFT_GREEN)|(nBlueAverage<<SHIFT_BLUE);

}
