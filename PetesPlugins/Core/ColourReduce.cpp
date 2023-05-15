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


#include "ColourReduce.h"
#include "PeteHelpers.h"

const int cnGridSizeShift=3;
const int cnGridSize=(1<<cnGridSizeShift);
const int cnGridSizeMask=(cnGridSize-1);
const int cnGridCellCount=(cnGridSize*cnGridSize*cnGridSize);
const int cnColourToIndexShift=(8-cnGridSizeShift);
const int cnGridCellWidth=(1<<cnColourToIndexShift);
const int cnGridCellHalfWidth=(cnGridCellWidth/2);

void Pete_ColourReduce_CalcHistogram(
	U32* pSource,
	int nWidth,
	int nHeight,
	int nSampleSpacing,
	int* pHistogram,
	float PalettePersistence);
void Pete_ColourReduce_SortColours(int* pHistogram,int** ppSortedColours);
void Pete_ColourReduce_SetupInverseColourMap(int** ppSortedColours,int nColours,SPete_ColourReduce_InverseMapEntry* pInverseColorMap,int* pHistogram);
extern "C" int Pete_ColourReduce_HistogramSortFunction(const void* pElem1,const void* pElem2);

static SPete_Parameter g_Parameters[]={
	{
		"Colour Count",
		8.0f,
		1.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Palette Persistence",
		0.95f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Boundary Smoothing",
		0.10f,
		0.0f,
		1.0f
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_ColourReduce_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_ColourReduce_Init(SPete_ColourReduce_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_ColourReduce_DeInit(pInstanceData);

	pInstanceData->hRGBHistogram=
		Pete_NewHandle(cnGridCellCount*sizeof(int));
	if (pInstanceData->hRGBHistogram==NULL) {
		Pete_ColourReduce_DeInit(pInstanceData);
		return 0;
	}
	Pete_ZeroMemory((char*)Pete_LockHandle(pInstanceData->hRGBHistogram),cnGridCellCount*sizeof(int));

	pInstanceData->hSortedColours=
		Pete_NewHandle(cnGridCellCount*sizeof(int*));
	if (pInstanceData->hSortedColours==NULL) {
		Pete_ColourReduce_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->hInverseColourMap=
		Pete_NewHandle(cnGridCellCount*sizeof(SPete_ColourReduce_InverseMapEntry));

	if (pInstanceData->hInverseColourMap==NULL) {
		Pete_ColourReduce_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_ColourReduce_DeInit(SPete_ColourReduce_Data* pInstanceData) {

	if (pInstanceData->hRGBHistogram!=NULL) {
		Pete_FreeHandle(pInstanceData->hRGBHistogram);
		pInstanceData->hRGBHistogram=NULL;
	}

	if (pInstanceData->hSortedColours!=NULL) {
		Pete_FreeHandle(pInstanceData->hSortedColours);
		pInstanceData->hSortedColours=NULL;
	}

	if (pInstanceData->hInverseColourMap!=NULL) {
		Pete_FreeHandle(pInstanceData->hInverseColourMap);
		pInstanceData->hInverseColourMap=NULL;
	}

}

inline U32 Pete_ColourReduce_GetClosestColour(U32 Colour,SPete_ColourReduce_InverseMapEntry* pInverseColourMap,float BoundarySmoothing) {

	const int nRed=(Colour>>SHIFT_RED)&0xff;
	const int nGreen=(Colour>>SHIFT_GREEN)&0xff;
	const int nBlue=(Colour>>SHIFT_BLUE)&0xff;

	const int nRedIndex=(nRed>>cnColourToIndexShift);
	const int nGreenIndex=(nGreen>>cnColourToIndexShift);
	const int nBlueIndex=(nBlue>>cnColourToIndexShift);

	SPete_ColourReduce_InverseMapEntry* pInvColourMapEntry=
		pInverseColourMap+
		(nBlueIndex<<(cnGridSizeShift*2))+
		(nGreenIndex<<(cnGridSizeShift*1))+
		(nRedIndex<<(cnGridSizeShift*0));

	U32 ClosestColour=(pInvColourMapEntry->ClosestColour);

	if (BoundarySmoothing==0.0f) {
		return ClosestColour;
	}

	U32 NextClosestColour=(pInvColourMapEntry->NextClosestColour);

	const int nClosestRed=(ClosestColour>>SHIFT_RED)&0xff;
	const int nClosestGreen=(ClosestColour>>SHIFT_GREEN)&0xff;
	const int nClosestBlue=(ClosestColour>>SHIFT_BLUE)&0xff;

	const int nNextClosestRed=(NextClosestColour>>SHIFT_RED)&0xff;
	const int nNextClosestGreen=(NextClosestColour>>SHIFT_GREEN)&0xff;
	const int nNextClosestBlue=(NextClosestColour>>SHIFT_BLUE)&0xff;

	const int nClosestDeltaRed=(nClosestRed-nRed);
	const int nClosestDeltaGreen=(nClosestGreen-nGreen);
	const int nClosestDeltaBlue=(nClosestBlue-nBlue);

	const int nNextClosestDeltaRed=(nNextClosestRed-nRed);
	const int nNextClosestDeltaGreen=(nNextClosestGreen-nGreen);
	const int nNextClosestDeltaBlue=(nNextClosestBlue-nBlue);

	const int nClosestDistSqrd=
		(nClosestDeltaRed*nClosestDeltaRed)+
		(nClosestDeltaGreen*nClosestDeltaGreen)+
		(nClosestDeltaBlue*nClosestDeltaBlue);

	const int nNextClosestDistSqrd=
		(nNextClosestDeltaRed*nNextClosestDeltaRed)+
		(nNextClosestDeltaGreen*nNextClosestDeltaGreen)+
		(nNextClosestDeltaBlue*nNextClosestDeltaBlue);

	const float ClosestDist=(float)sqrt(nClosestDistSqrd);
	const float NextClosestDist=(float)sqrt(nNextClosestDistSqrd);

	const float TotalDist=(ClosestDist+NextClosestDist);

	if (TotalDist==0.0f) {
		return ClosestColour;
	}

	const float UnWeightedLerpValue=(NextClosestDist/TotalDist);

	float WeightedLerpValue=(UnWeightedLerpValue-0.5f)/BoundarySmoothing;
	WeightedLerpValue+=0.5f;
	if (WeightedLerpValue>1.0f) {
		return ClosestColour;
	} else if (WeightedLerpValue<0.0f) {
		return NextClosestColour;
	} else {

		const float OneMinusLerpValue=(1.0f-WeightedLerpValue);

		const int nSmoothedRed=(int)(
			(nClosestRed*WeightedLerpValue)+
			(nNextClosestRed*OneMinusLerpValue));

		const int nSmoothedGreen=(int)(
			(nClosestGreen*WeightedLerpValue)+
			(nNextClosestGreen*OneMinusLerpValue));

		const int nSmoothedBlue=(int)(
			(nClosestBlue*WeightedLerpValue)+
			(nNextClosestBlue*OneMinusLerpValue));

		const U32 SmoothedColour=
			(nSmoothedRed<<SHIFT_RED)|
			(nSmoothedGreen<<SHIFT_GREEN)|
			(nSmoothedBlue<<SHIFT_BLUE);

		return SmoothedColour;

	}
}


void Pete_ColourReduce_Render(SPete_ColourReduce_Data* pInstanceData,SPete_ColourReduce_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nColours=pSettings->m_TargetColourCount;
	const float PalettePersistence=pSettings->m_PalettePersistence;
	const float BoundarySmoothing=pSettings->m_BoundarySmoothing;

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	int* pHistogram=(int*)Pete_LockHandle(pInstanceData->hRGBHistogram);
	if (pHistogram==NULL) {
		return;
	}
	int** ppSortedColours=(int**)Pete_LockHandle(pInstanceData->hSortedColours);
	if (ppSortedColours==NULL) {
		return;
	}
	SPete_ColourReduce_InverseMapEntry* pInverseColourMap=
		(SPete_ColourReduce_InverseMapEntry*)Pete_LockHandle(pInstanceData->hInverseColourMap);
	if (pInverseColourMap==NULL) {
		return;
	}

	const int nSampleSpacing=4;

	Pete_ColourReduce_CalcHistogram(pSource,nWidth,nHeight,nSampleSpacing,pHistogram,PalettePersistence);

	Pete_ColourReduce_SortColours(pHistogram,ppSortedColours);

	Pete_ColourReduce_SetupInverseColourMap(ppSortedColours,nColours,pInverseColourMap,pHistogram);

	const int nNumPixels=(nWidth*nHeight);

	U32* pCurrentSource=pSource;
	U32* pSourceEnd=pSource+nNumPixels;

	U32* pCurrentOutput=pOutput;

	while (pCurrentSource<pSourceEnd) {

		*pCurrentOutput=
			Pete_ColourReduce_GetClosestColour(*pCurrentSource,pInverseColourMap,BoundarySmoothing);

		pCurrentSource+=1;
		pCurrentOutput+=1;

	}

	Pete_UnLockHandle(pInstanceData->hRGBHistogram);
	Pete_UnLockHandle(pInstanceData->hSortedColours);
	Pete_UnLockHandle(pInstanceData->hInverseColourMap);

}

void Pete_ColourReduce_CalcHistogram(
	U32* pSource,
	int nWidth,
	int nHeight,
	int nSampleSpacing,
	int* pHistogram,
	float PalettePersistence) {

	const int nHistogramMult=
		static_cast<int>((1<<8)*PalettePersistence);

	int nCount;
	for (nCount=0; nCount<cnGridCellCount; nCount+=1) {

		int nEntry=pHistogram[nCount];

		nEntry*=nHistogramMult;
		nEntry>>=8;

		pHistogram[nCount]=nEntry;

	}

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

			const int nRedIndex=(nSourceRed>>cnColourToIndexShift);
			const int nGreenIndex=(nSourceGreen>>cnColourToIndexShift);
			const int nBlueIndex=(nSourceBlue>>cnColourToIndexShift);

			int* pResultCell=
				(pHistogram+
				(nBlueIndex*cnGridSize*cnGridSize)+
				(nGreenIndex*cnGridSize)+
				(nRedIndex));

			(*pResultCell)+=1;

			pCurrentSource+=nSampleSpacing;

		}

		pCurrentSource=
			pSourceLineStart+(nSampleSpacing*nWidth);

	}

}

extern "C" int Pete_ColourReduce_HistogramSortFunction(const void* pElem1,const void* pElem2) {

	int** ppFirstElement=(int**)pElem1;
	int** ppSecondElement=(int**)pElem2;

	const int nFirstValue=(**ppFirstElement);
	const int nSecondValue=(**ppSecondElement);

	if (nSecondValue<nFirstValue) {
		return -1;
	} else if (nSecondValue>nFirstValue) {
		return 1;
	} else {
		return 0;
	}

}

void Pete_ColourReduce_SortColours(int* pHistogram,int** ppSortedColours) {

	int nCount;
	for (nCount=0; nCount<cnGridCellCount; nCount+=1) {

		ppSortedColours[nCount]=&(pHistogram[nCount]);

	}

	qsort((void*)ppSortedColours,cnGridCellCount,sizeof(int*),&Pete_ColourReduce_HistogramSortFunction);

}

void Pete_ColourReduce_SetupInverseColourMap(int** ppSortedColours,int nColours,SPete_ColourReduce_InverseMapEntry* pInverseColorMap,int* pHistogram) {

	int nSortedRed[cnGridCellCount];
	int nSortedGreen[cnGridCellCount];
	int nSortedBlue[cnGridCellCount];

	int nCount;
	for (nCount=0; nCount<nColours; nCount+=1) {

		int nOffset=(ppSortedColours[nCount]-pHistogram);

		nSortedRed[nCount]=(nOffset>>(cnGridSizeShift*0))&cnGridSizeMask;
		nSortedRed[nCount]*=cnGridCellWidth;

		nSortedGreen[nCount]=(nOffset>>(cnGridSizeShift*1))&cnGridSizeMask;
		nSortedGreen[nCount]*=cnGridCellWidth;

		nSortedBlue[nCount]=(nOffset>>(cnGridSizeShift*2))&cnGridSizeMask;
		nSortedBlue[nCount]*=cnGridCellWidth;

	}

	int nBlueIndex;
	for (nBlueIndex=0; nBlueIndex<cnGridSize; nBlueIndex+=1) {

		int nBlue=(nBlueIndex*cnGridCellWidth)+(cnGridCellHalfWidth);

		int nGreenIndex;
		for (nGreenIndex=0; nGreenIndex<cnGridSize; nGreenIndex+=1) {

			int nGreen=(nGreenIndex*cnGridCellWidth)+(cnGridCellHalfWidth);

			int nRedIndex;
			for (nRedIndex=0; nRedIndex<cnGridSize; nRedIndex+=1) {

				int nRed=(nRedIndex*cnGridCellWidth)+(cnGridCellHalfWidth);

				int nClosestDistance=cnBiggestSignedInt;
				int nNextClosestDistance=cnBiggestSignedInt;
				U32 ResultColour;
				U32 NextClosestColour;

				int nCount;
				for (nCount=0; nCount<nColours; nCount+=1) {

					const int nCandRed=nSortedRed[nCount];
					const int nCandGreen=nSortedGreen[nCount];
					const int nCandBlue=nSortedBlue[nCount];

					const int nRedDist=nRed-nCandRed;
					const int nRedDistSquared=(nRedDist*nRedDist);

					const int nGreenDist=nGreen-nCandGreen;
					const int nGreenDistSquared=(nGreenDist*nGreenDist);

					const int nBlueDist=nBlue-nCandBlue;
					const int nBlueDistSquared=(nBlueDist*nBlueDist);

					const int nTotalDistSquared=
						nRedDistSquared+
						nGreenDistSquared+
						nBlueDistSquared;

					if (nTotalDistSquared<nClosestDistance) {

						nNextClosestDistance=nClosestDistance;
						NextClosestColour=ResultColour;

						nClosestDistance=nTotalDistSquared;
						ResultColour=
							(nCandRed<<SHIFT_RED)|
							(nCandGreen<<SHIFT_GREEN)|
							(nCandBlue<<SHIFT_BLUE);

					} else if (nTotalDistSquared<nNextClosestDistance) {

						nNextClosestDistance=nTotalDistSquared;
						NextClosestColour=
							(nCandRed<<SHIFT_RED)|
							(nCandGreen<<SHIFT_GREEN)|
							(nCandBlue<<SHIFT_BLUE);

					}

				}

				SPete_ColourReduce_InverseMapEntry* pInvColourMapEntry=
					pInverseColorMap+
					(nBlueIndex<<(cnGridSizeShift*2))+
					(nGreenIndex<<(cnGridSizeShift*1))+
					(nRedIndex<<(cnGridSizeShift*0));

				pInvColourMapEntry->ClosestColour=ResultColour;
				pInvColourMapEntry->NextClosestColour=NextClosestColour;

			}

		}

	}


}
