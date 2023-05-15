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


#include "TonyBlur.h"
#include "PeteHelpers.h"

#include "SummedAreaTables.h"

static SPete_Parameter g_Parameters[]={
	{
		"Blur Radius",
		15.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Centre X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Centre Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Clear Radius",
		0.0f,
		0.0f,
		1000.0f,
		PETE_PARAM_INT
	},
	{
		"Passes",
		1.0f,
		1.0f,
		4.0f,
		PETE_PARAM_INT
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_TonyBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_TonyBlur_Init(SPete_TonyBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_TonyBlur_DeInit(pInstanceData);

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*sizeof(SPete_SATEntry));
	pInstanceData->hSummedAreaTable=Pete_NewHandle(nNumBytes);
	if (pInstanceData->hSummedAreaTable==NULL) {
		Pete_TonyBlur_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->hDistanceTable=
		Pete_NewHandle(nNumPixels*sizeof(int));
	if (pInstanceData->hDistanceTable==NULL) {
		Pete_TonyBlur_DeInit(pInstanceData);
		return 0;
	}

	int* pDistanceTable=(int*)
		Pete_LockHandle(pInstanceData->hDistanceTable);
	if (pDistanceTable==NULL) {
		return 0;
	}

	int* pCurrentDistanceTable=pDistanceTable;

	int nY;
	for (nY=0; nY<nHeight; nY+=1) {

		int nX;
		for (nX=0; nX<nWidth; nX+=1) {

			*pCurrentDistanceTable=
				(int)sqrtf((nX*nX)+(nY*nY));

			pCurrentDistanceTable+=1;

		}

	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_TonyBlur_DeInit(SPete_TonyBlur_Data* pInstanceData) {

	if (pInstanceData->hSummedAreaTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hSummedAreaTable);
		pInstanceData->hSummedAreaTable=NULL;
	}

	if (pInstanceData->hDistanceTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hDistanceTable);
		pInstanceData->hDistanceTable=NULL;
	}

}

void Pete_TonyBlur_Render(SPete_TonyBlur_Data* pInstanceData,SPete_TonyBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=(nWidth*nHeight);

	SPete_SATEntry* pTable=(SPete_SATEntry*)
		Pete_LockHandle(pInstanceData->hSummedAreaTable);

	if (pTable==NULL) {
		return;
	}

	const int* pDistanceTable=(int*)
		Pete_LockHandle(pInstanceData->hDistanceTable);
	if (pDistanceTable==NULL) {
		return;
	}

	int nBaseRadius=pSettings->m_BlurRadius;
	if (nBaseRadius<=0) {
		nBaseRadius=1;
	}

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);
	const int nMaxDist=sqrtf(
		(nHalfWidth*nHalfWidth)+
		(nHalfHeight*nHalfHeight));

	const int nRadiusMult=
		(nBaseRadius*(1<<16))/nMaxDist;

	const float InputCentreX=pSettings->m_CentreX;
	const float InputCentreY=pSettings->m_CentreY;

	int nCentreX=(InputCentreX*nWidth);
	nCentreX=GateInt(nCentreX,0,(nWidth-1));

	int nCentreY=(InputCentreY*nHeight);
	nCentreY=GateInt(nCentreY,0,(nHeight-1));

	const int nClearRadius=pSettings->m_ClearRadius;

	const int nPasses=pSettings->m_Passes;

	int nRadiusErrorFP=0;

	int nCurrentPass;
	for (nCurrentPass=0; nCurrentPass<nPasses; nCurrentPass+=1) {

		U32* pCurrentSource;
		if (nCurrentPass==0) {
			pCurrentSource=pSource;
		} else {
			pCurrentSource=pOutput;
		}

		U32* pCurrentOutput=pOutput;

		Pete_SummedAreaTable_Construct(pCurrentSource,nWidth,nHeight,pTable);

		int nY;
		for (nY=0; nY<nHeight; nY+=1) {

			const int nDeltaY=(nY-nCentreY);
			const int nAbsDeltaY=AbsInt(nDeltaY);

			const int* pDistanceTableLine=
				pDistanceTable+(nAbsDeltaY*nWidth);

			int nX;
			for (nX=0; nX<nWidth; nX+=1) {

				const int nDeltaX=(nX-nCentreX);
				const int nAbsDeltaX=AbsInt(nDeltaX);
				int nCurrentDistance=
					*(pDistanceTableLine+nAbsDeltaX);

				nCurrentDistance-=nClearRadius;
				if (nCurrentDistance<0) {
					nCurrentDistance=0;
				}

				const int nCurrentRadiusFP=
					(nCurrentDistance*nRadiusMult);

				const int nCurrentRadius=
					(nCurrentRadiusFP+nRadiusErrorFP)>>16;

				const int nCurrentErrorFP=
					(nCurrentRadiusFP-(nCurrentRadius<<16));

				nRadiusErrorFP+=nCurrentErrorFP;

				if (nCurrentRadius==0) {
					*pCurrentOutput=*pCurrentSource;
				} else {

					const int nFloorRadius=(nCurrentRadius>>1);
					const int nCeilingRadius=(nCurrentRadius+1)>>1;

					int nLeft=(nX-nFloorRadius);
					nLeft=GateInt(nLeft,0,(nWidth-1));

					int nRight=(nX+nCeilingRadius);
					nRight=GateInt(nRight,0,(nWidth-1));

					int nTop=(nY-nFloorRadius);
					nTop=GateInt(nTop,0,(nHeight-1));

					int nBottom=(nY+nCeilingRadius);
					nBottom=GateInt(nBottom,0,(nHeight-1));

					const U32 OutputColour=
						Pete_SummedAreaTable_GetAverage(
							pTable,nWidth,nHeight,
							nLeft,nTop,
							nRight,nBottom);

					*pCurrentOutput=OutputColour;

				}

				pCurrentSource+=1;
				pCurrentOutput+=1;

			}

		}

	}

}