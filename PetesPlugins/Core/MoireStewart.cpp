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

#include "MoireStewart.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Size 1",
		4.0f,
		1.0f,
		16.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Centre 1 X",
		0.0f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Centre 1 Y",
		0.0f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Size 2",
		4.0f,
		1.0f,
		16.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Centre 2 X",
		0.04f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Centre 2 Y",
		0.04f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

const int nFPShift=8;
const int nFPMult=(1<<nFPShift);
const int nHalfFPShift=(nFPShift/2);
const int nFPMask=(nFPMult-1);
const int nSqrtTablePrecisionShift=0;
const int nSqrtTablePrecision=(1<<0);

void Pete_MoireStewart_BuildSqrtTable(SPete_MoireStewart_Data* pInstanceData);

void Pete_MoireStewart_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_MoireStewart_Init(SPete_MoireStewart_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_MoireStewart_DeInit(pInstanceData);

	int nLargestDimension;
	if (nWidth>nHeight) {
		nLargestDimension=nWidth;
	} else {
		nLargestDimension=nHeight;
	}

	const int nSqrtTableSize=(nLargestDimension*nLargestDimension*nSqrtTablePrecision*2);
	const int nSqrtTableBytes=(nSqrtTableSize*sizeof(int));

	pInstanceData->pSqrtTable=(int*)malloc(nSqrtTableBytes);
	if (pInstanceData->pSqrtTable==NULL) {
		Pete_MoireStewart_DeInit(pInstanceData);
		return 0;
	}
	
	pInstanceData->nSqrtTableSize=nSqrtTableSize;	
	Pete_MoireStewart_BuildSqrtTable(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_MoireStewart_DeInit(SPete_MoireStewart_Data* pInstanceData) {

	if (pInstanceData->pSqrtTable!=NULL) {
		free(pInstanceData->pSqrtTable);
		pInstanceData->pSqrtTable=NULL;
	}

}

void Pete_MoireStewart_Render(SPete_MoireStewart_Data* pInstanceData,SPete_MoireStewart_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const float HalfWidth=(Width/2.0f);
	const float HalfHeight=(Height/2.0f);

	const int nPixelsCount=(nWidth*nHeight);

	const int* pSqrtTable=(pInstanceData->pSqrtTable);

	const int nCircleCentre1X=(pSettings->m_Centre1X*HalfWidth)*nFPMult;
	const int nCircleCentre1Y=(pSettings->m_Centre1Y*HalfHeight)*nFPMult;
	const int nCircleSize1=(1.0f/pSettings->m_Size1)*(nFPMult*nFPMult);

	const int nCircleCentre2X=(pSettings->m_Centre2X*HalfWidth)*nFPMult;
	const int nCircleCentre2Y=(pSettings->m_Centre2Y*HalfHeight)*nFPMult;
	const int nCircleSize2=(1.0f/pSettings->m_Size2)*(nFPMult*nFPMult);

	U32* pOutputStart=pOutput;
	U32* pOutputEnd=(pOutput+nPixelsCount);

	U32* pCurrentOutput=pOutputStart;
	U32* pCurrentSource=pSource;

	int nCurrentY=(-HalfHeight*nFPMult);
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=(pOutputLineStart+nWidth);
		
		const int nStartX=(-HalfWidth*nFPMult);

		int nCurrentX=nStartX;

		const int nDelta1Y=(nCurrentY-nCircleCentre1Y);
		const int nDeltaSquared1Y=
			(nDelta1Y>>nHalfFPShift)*
			(nDelta1Y>>nHalfFPShift);
		
		const int nDelta2Y=(nCurrentY-nCircleCentre2Y);
		const int nDeltaSquared2Y=
			(nDelta2Y>>nHalfFPShift)*
			(nDelta2Y>>nHalfFPShift);

		while (pCurrentOutput<pOutputLineEnd) {

			const int nDelta1X=(nCurrentX-nCircleCentre1X);
			const int nDeltaSquared1X=
				(nDelta1X>>nHalfFPShift)*
				(nDelta1X>>nHalfFPShift);

			const int nDelta2X=(nCurrentX-nCircleCentre2X);
			const int nDeltaSquared2X=
				(nDelta2X>>nHalfFPShift)*
				(nDelta2X>>nHalfFPShift);

			int nDistSquared1=(nDeltaSquared1X+nDeltaSquared1Y);
			int nDist1=pSqrtTable[nDistSquared1>>(nFPShift-nSqrtTablePrecisionShift)];
//			nDist1=sqrtf(nDistSquared1/(float)nFPMult)*nFPMult;

			int nDistSquared2=(nDeltaSquared2X+nDeltaSquared2Y);
			int nDist2=pSqrtTable[nDistSquared2>>(nFPShift-nSqrtTablePrecisionShift)];
//			nDist2=sqrtf(nDistSquared2/(float)nFPMult)*nFPMult;

			const int nCircleRing1=(nDist1*nCircleSize1)>>(nFPShift*2);
			const int nCircleRing1Modded=nCircleRing1&nFPMask;

			const int nCircleRing2=(nDist2*nCircleSize2)>>(nFPShift*2);
			const int nCircleRing2Modded=nCircleRing2&nFPMask;

			int nGreyValue1;
			if (nCircleRing1Modded<(nFPMult/2)) {
				nGreyValue1=(255*2*nCircleRing1Modded)>>nFPShift;
			} else {
				nGreyValue1=255-((255*2*(nCircleRing1Modded-(nFPMult/2)))>>nFPShift);
			}

			int nGreyValue2;
			if (nCircleRing2Modded<(nFPMult/2)) {
				nGreyValue2=(255*2*nCircleRing2Modded)>>nFPShift;
			} else {
				nGreyValue2=255-((255*2*(nCircleRing2Modded-(nFPMult/2)))>>nFPShift);
			}

			const int nGreyValue=AbsInt(nGreyValue1-nGreyValue2);

			const U32 OutputColour=
				(nGreyValue<<16)|
				(nGreyValue<<8)|
				(nGreyValue<<0);

			*pCurrentOutput=OutputColour;

			nCurrentX+=nFPMult;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}
		
		nCurrentY+=nFPMult;

	}

}

void Pete_MoireStewart_BuildSqrtTable(SPete_MoireStewart_Data* pInstanceData) {

	const int nSqrtTableEntryCount=pInstanceData->nSqrtTableSize;

	int* pSqrtTableStart=pInstanceData->pSqrtTable;
	int* pSqrtTableEnd=(pSqrtTableStart+nSqrtTableEntryCount);

	const float ValueIncrement=1.0f/nSqrtTablePrecision;

	float CurrentValue=0.0f;

	int* pCurrentEntry=pSqrtTableStart;

	while (pCurrentEntry<pSqrtTableEnd) {

		*pCurrentEntry=sqrtf(CurrentValue)*nFPMult;

		CurrentValue+=ValueIncrement;

		pCurrentEntry+=1;

	}

}
