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

#include "MetaImage.h"
#include "PeteHelpers.h"

U32 Pete_MetaImage_CreateSubImage(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pInput,U32* pSubImage,float SubWidth,float SubHeight);
void Pete_MetaImage_DrawSubImages(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pSource,U32* pOutput,U32* pSubImage,U32 AverageColour,float SubWidth,float SubHeight);
void Pete_MetaImage_DrawSubImage(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pShrunkBuffer,U32* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY,U32 WholeImageAverage,int nClippedLeftX,int nClippedTopY,int nClippedRightX,int nClippedBottomY,U32 SubImageAverage);
U32	Pete_MetaImage_GetAreaAverage(SPete_MetaImage_Data* pInstanceData,U32* pImage,int nLeftX,int nTopY,int nRightX,int nBottomY,int nStride);
U32	Pete_MetaImage_ShrinkSourceImage(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pOutput,float SubWidth,float SubHeight);
U32	Pete_MetaImage_ShrinkSourceImageFast(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pOutput,float SubWidth,float SubHeight);

static SPete_Parameter g_Parameters[]={
	{
		"Size",
		0.2f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Do Distance Based",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Cheap And Nasty",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_MetaImage_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_MetaImage_Init(SPete_MetaImage_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_MetaImage_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*sizeof(U32));
	pInstanceData->hSubImage=
		Pete_NewHandle(nNumBytes);
	if (pInstanceData->hSubImage==NULL) {
		Pete_MetaImage_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_MetaImage_DeInit(SPete_MetaImage_Data* pInstanceData) {

	if (pInstanceData->hSubImage!=NULL) {
		Pete_FreeHandle(pInstanceData->hSubImage);
		pInstanceData->hSubImage=NULL;
	}

}

void Pete_MetaImage_Render(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	float SubWidth;
	float SubHeight;

	if (pSettings->m_DoDistanceBased>0.0f) {

		const float Distance=1.0f+(pSettings->m_Size*(nHeight-1.0f));

		SubWidth=nWidth/Distance;
		SubHeight=nHeight/Distance;

	} else {

		SubWidth=1+(pSettings->m_Size*(nWidth-1));
		SubHeight=1+(pSettings->m_Size*(nHeight-1));
	}

	U32* pSubImageData=(U32*)Pete_LockHandle(pInstanceData->hSubImage);
	if (pSubImageData==NULL) {
		return;
	} 

	U32 AverageColour=Pete_MetaImage_CreateSubImage(pInstanceData,pSettings,pSource,pSubImageData,SubWidth,SubHeight);

	Pete_MetaImage_DrawSubImages(pInstanceData,pSettings,pSource,pOutput,pSubImageData,AverageColour,SubWidth,SubHeight);

}

U32 Pete_MetaImage_CreateSubImage(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pInput,U32* pSubImage,float SubWidth,float SubHeight) {

	U32 AverageColour;

	if (pSettings->m_DoCheapAndNasty>0.0f) {
		AverageColour=
			Pete_MetaImage_ShrinkSourceImageFast(pInstanceData,pInput,pSubImage,SubWidth,SubHeight);
	} else {
		AverageColour=
			Pete_MetaImage_ShrinkSourceImage(pInstanceData,pInput,pSubImage,SubWidth,SubHeight);
	}

	return AverageColour;

}

void Pete_MetaImage_DrawSubImages(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pSource,U32* pOutput,U32* pSubImage,U32 AverageColour,float SubWidth,float SubHeight) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	
	const int nHalfWidth=nWidth/2;
	const int nHalfHeight=nHeight/2;

	float CentreX=nHalfWidth+(SubWidth/2);
	float CentreY=nHalfHeight+(SubHeight/2);

	int nSubImageCountX=static_cast<int>(CentreX/SubWidth);
	int nSubImageCountY=static_cast<int>(CentreY/SubHeight);

	const float StartX=(nHalfWidth-(SubWidth/2))-(nSubImageCountX*SubWidth);
	const float EndX=(nHalfWidth+(SubWidth/2))+(nSubImageCountX*SubWidth);

	const float StartY=(nHalfHeight-(SubHeight/2))-(nSubImageCountY*SubHeight);
	const float EndY=(nHalfHeight+(SubHeight/2))+(nSubImageCountY*SubHeight);

	float CurrentY;
	for (CurrentY=StartY; CurrentY<EndY; CurrentY+=SubHeight) {

		float CurrentX;
		for (CurrentX=StartX; CurrentX<EndX; CurrentX+=SubWidth) {

			const int nLeftX=static_cast<int>(CurrentX);
			const int nTopY=static_cast<int>(CurrentY);
			const int nRightX=static_cast<int>(CurrentX+SubWidth);
			const int nBottomY=static_cast<int>(CurrentY+SubHeight);

			const int nClippedLeftX=GateInt(nLeftX,0,(nWidth-1));
			const int nClippedTopY=GateInt(nTopY,0,(nHeight-1));
			const int nClippedRightX=GateInt(nRightX,0,(nWidth-1));
			const int nClippedBottomY=GateInt(nBottomY,0,(nHeight-1));

			U32 SubImageAverage=Pete_MetaImage_GetAreaAverage(
				pInstanceData,
				pSource,
				nClippedLeftX,
				nClippedTopY,
				nClippedRightX,
				nClippedBottomY,
				4);

			Pete_MetaImage_DrawSubImage(
				pInstanceData,
				pSource,
				pSubImage,
				pOutput,
				nLeftX,
				nTopY,
				nRightX,
				nBottomY,
				AverageColour,
				nClippedLeftX,
				nClippedTopY,
				nClippedRightX,
				nClippedBottomY,
				SubImageAverage);

		}

	}

}

void Pete_MetaImage_DrawSubImage(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pShrunkBuffer,U32* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY,U32 WholeImageAverage,int nClippedLeftX,int nClippedTopY,int nClippedRightX,int nClippedBottomY,U32 SubImageAverage) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nSubRed=(SubImageAverage>>SHIFT_RED)&0xff;
	const int nSubGreen=(SubImageAverage>>SHIFT_GREEN)&0xff;
	const int nSubBlue=(SubImageAverage>>SHIFT_BLUE)&0xff;

	const int nWholeRed=(WholeImageAverage>>SHIFT_RED)&0xff;
	const int nWholeGreen=(WholeImageAverage>>SHIFT_GREEN)&0xff;
	const int nWholeBlue=(WholeImageAverage>>SHIFT_BLUE)&0xff;

	const int nRedDelta=(nSubRed-nWholeRed);
	const int nGreenDelta=(nSubGreen-nWholeGreen);
	const int nBlueDelta=(nSubBlue-nWholeBlue);

	const int nXDelta=nClippedRightX-nClippedLeftX;
	const int nYDelta=nClippedBottomY-nClippedTopY;

	if ((nXDelta<=0)||
		(nYDelta<=0)) {
		return;
	}


	U32* pCurrentSource=pShrunkBuffer;

	pCurrentSource+=((nClippedTopY-nTopY)*nWidth);
	pCurrentSource+=(nClippedLeftX-nLeftX);

	U32* pCurrentOutput=pOutput+(nClippedTopY*nWidth)+nClippedLeftX;
	U32*const pOutputEnd=(pCurrentOutput+(nYDelta*nWidth)+nXDelta);

	while (pCurrentOutput<pOutputEnd) {
		
		U32*const pOutputLineStart=pCurrentOutput;
		U32*const pOutputLineEnd=pCurrentOutput+nXDelta;

		U32* pSourceLineStart=pCurrentSource;

		while (pCurrentOutput<pOutputLineEnd) {

			const U32 SourceColour=*pCurrentSource;

			const U32 nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const U32 nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const U32 nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;

			const U32 nOutputRed=GateInt(nSourceRed+nRedDelta,0,255);
			const U32 nOutputGreen=GateInt(nSourceGreen+nGreenDelta,0,255);
			const U32 nOutputBlue=GateInt(nSourceBlue+nBlueDelta,0,255);

			const U32 OutputColour=
				((nOutputRed&0xff)<<SHIFT_RED)|
				((nOutputGreen&0xff)<<SHIFT_GREEN)|
				((nOutputBlue&0xff)<<SHIFT_BLUE);

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;
		pCurrentSource=pSourceLineStart+nWidth;

	}

}

U32	Pete_MetaImage_GetAreaAverage(SPete_MetaImage_Data* pInstanceData,U32* pImage,int nLeftX,int nTopY,int nRightX,int nBottomY,int nStride) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nXDelta=nRightX-nLeftX;
	const int nYDelta=nBottomY-nTopY;

	if ((nXDelta<=0)||
		(nYDelta<=0)) {
		return 0x00000000;
	}
	
	U32* pCurrentImage=pImage+(nTopY*nWidth)+nLeftX;
	U32*const pImageEnd=(pCurrentImage+(nYDelta*nWidth)+nXDelta);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	while (pCurrentImage<pImageEnd) {
		
		U32*const pImageLineStart=pCurrentImage;
		U32*const pImageLineEnd=pCurrentImage+nXDelta;

		while (pCurrentImage<pImageLineEnd) {

			const U32 ImageColour=*pCurrentImage;

			const U32 nImageRed=(ImageColour>>SHIFT_RED)&0xff;
			const U32 nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
			const U32 nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nImageRed;
			nGreenTotal+=nImageGreen;
			nBlueTotal+=nImageBlue;

			nSampleCount+=1;
			
			pCurrentImage+=nStride;

		}

		pCurrentImage=pImageLineStart+(nStride*nWidth);

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	U32 Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;
	
}

U32	Pete_MetaImage_ShrinkSourceImage(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pOutput,float SubWidth,float SubHeight) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	if (SubWidth>(float)(nWidth)) {
		SubWidth=(float)(nWidth);
	}

	if (SubHeight>(float)(nHeight)) {
		SubHeight=(float)(nHeight);
	}

	const float SourceYInc=(nHeight/SubHeight);
	const float SourceXInc=(nWidth/SubWidth);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	U32* pCurrentOutput=pOutput;
	
	float SourceY;
	for (SourceY=0.0f; SourceY<nHeight; SourceY+=SourceYInc) {

		U32* pOutputLineStart=pCurrentOutput;
		const int nTopY=static_cast<int>(SourceY);
		int nBottomY=static_cast<int>(SourceY+SourceYInc);
		nBottomY=GateInt(nBottomY,0,(nHeight-1));

		float SourceX;
		for (SourceX=0.0f; SourceX<nWidth; SourceX+=SourceXInc) {

			const int nLeftX=static_cast<int>(SourceX);
			int nRightX=static_cast<int>(SourceX+SourceXInc);
			nRightX=GateInt(nRightX,0,(nWidth-1));

			const U32 OutputColour=
				Pete_MetaImage_GetAreaAverage(pInstanceData,pSource,nLeftX,nTopY,nRightX,nBottomY,1);

			const U32 nOutputRed=(OutputColour>>SHIFT_RED)&0xff;
			const U32 nOutputGreen=(OutputColour>>SHIFT_GREEN)&0xff;
			const U32 nOutputBlue=(OutputColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nOutputRed;
			nGreenTotal+=nOutputGreen;
			nBlueTotal+=nOutputBlue;

			nSampleCount+=1;

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	U32 Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;

}

U32	Pete_MetaImage_ShrinkSourceImageFast(SPete_MetaImage_Data* pInstanceData,U32* pSource,U32* pOutput,float SubWidth,float SubHeight) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	if (SubWidth>(float)(nWidth)) {
		SubWidth=(float)(nWidth);
	}

	if (SubHeight>(float)(nHeight)) {
		SubHeight=(float)(nHeight);
	}

	const float SourceYInc=(nHeight/SubHeight);
	const float SourceXInc=(nWidth/SubWidth);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	U32* pCurrentOutput=pOutput;
	
	float SourceY;
	for (SourceY=0.0f; SourceY<nHeight; SourceY+=SourceYInc) {

		U32* pOutputLineStart=pCurrentOutput;
		const int nTopY=static_cast<int>(SourceY);

		U32* pSourceLineStart=pSource+(nTopY*nWidth);

		float SourceX;
		for (SourceX=0.0f; SourceX<nWidth; SourceX+=SourceXInc) {

			const int nLeftX=static_cast<int>(SourceX);

			const U32 OutputColour=*(pSourceLineStart+nLeftX);

			const U32 nOutputRed=(OutputColour>>SHIFT_RED)&0xff;
			const U32 nOutputGreen=(OutputColour>>SHIFT_GREEN)&0xff;
			const U32 nOutputBlue=(OutputColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nOutputRed;
			nGreenTotal+=nOutputGreen;
			nBlueTotal+=nOutputBlue;

			nSampleCount+=1;

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	U32 Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;

}
