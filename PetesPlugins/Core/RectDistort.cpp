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

#include "RectDistort.h"
#include "PeteHelpers.h"

void Pete_RectDistort_SetCoordMapIdentity(SPete_RectDistort_Data* pInstanceData);
void Pete_RectDistort_SetOffsetRectangle(
	SPete_RectDistort_Data* pInstanceData,
	int nLeftX,
	int nTopY,
	int nRightX,
	int nBottomY,
	const float XDelta,
	const float YDelta);
void Pete_RectDistort_DrawRectanglesToMap(SPete_RectDistort_Data* pInstanceData,SPete_RectDistort_Settings* pSettings);
void Pete_RectDistort_CoordMapDecay(SPete_RectDistort_Data* pInstanceData,float Persistence);

static SPete_Parameter g_Parameters[]={
	{
		"Enable Clear",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Enable Rectangles",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Rate",
		6.0f,
		0.0f,
		32.0f,
		PETE_PARAM_INT
	},
	{
		"Width",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Height",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"X Delta",
		8.0f,
		0.0f,
		32.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Y Delta",
		8.0f,
		0.0f,
		32.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Persistence",
		0.95f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},

};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_RectDistort_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_RectDistort_Init(SPete_RectDistort_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_RectDistort_DeInit(pInstanceData);

	const int nNumberOfPixels=nWidth*nHeight;
	const int nNumberOfBytes=
		nNumberOfPixels*sizeof(SPete_RectDistort_Coord);
	pInstanceData->hCoordMap=
		Pete_NewHandle(nNumberOfBytes);

	if (pInstanceData->hCoordMap==NULL) {
		Pete_RectDistort_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	Pete_RectDistort_SetCoordMapIdentity(pInstanceData);

	return 1;

}

void Pete_RectDistort_DeInit(SPete_RectDistort_Data* pInstanceData) {

	if (pInstanceData->hCoordMap!=NULL) {
		Pete_FreeHandle(pInstanceData->hCoordMap);
		pInstanceData->hCoordMap=NULL;
	}

}

void Pete_RectDistort_Render(SPete_RectDistort_Data* pInstanceData,SPete_RectDistort_Settings* pSettings,U32* pSource,U32* pOutput) {

	if (pSettings->m_DoEnableRectangles>0.0f) {
		Pete_RectDistort_DrawRectanglesToMap(pInstanceData,pSettings);
	}

	if (pSettings->m_DoEnableClear>0.0f) {
		Pete_RectDistort_SetCoordMapIdentity(pInstanceData);
	}

	if (pSettings->m_Persistence<1.0f) {
		Pete_RectDistort_CoordMapDecay(pInstanceData,pSettings->m_Persistence);
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const float Width=(float)(nWidth);
	const float Height=(float)(nHeight);

	const int nNumPixels = nWidth*nHeight;

	U32*const pSourceStart=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	SPete_RectDistort_Coord* pMapStart=(SPete_RectDistort_Coord*)Pete_LockHandle(pInstanceData->hCoordMap);
	if (pMapStart==NULL) {
		return;
	}

	SPete_RectDistort_Coord* pCurrentMap=pMapStart;

	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		while (pCurrentOutput!=pOutputLineEnd) {

			int nSourceX=static_cast<int>(pCurrentMap->x);
			int nSourceY=static_cast<int>(pCurrentMap->y);

			nSourceX=GetTiled(nSourceX,nWidth);
			nSourceY=GetTiled(nSourceY,nHeight);

			U32* pCurrentSource=
				pSourceStart+
				(nSourceY*nWidth)+
				nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentMap+=1;
			pCurrentOutput+=1;

		}

	}

}

void Pete_RectDistort_SetCoordMapIdentity(SPete_RectDistort_Data* pInstanceData) {

	SPete_RectDistort_Coord* pMapStart=(SPete_RectDistort_Coord*)Pete_LockHandle(pInstanceData->hCoordMap);
	if (pMapStart==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nNumPixels=(nWidth*nHeight);

	SPete_RectDistort_Coord* pCurrent=pMapStart;
	const SPete_RectDistort_Coord* pMapEnd=(pMapStart+nNumPixels);

	float YPos=0.0f;
	while (pCurrent!=pMapEnd) {

		const SPete_RectDistort_Coord* pLineEnd=pCurrent+nWidth;

		float XPos=0.0f;
		while (pCurrent!=pLineEnd) {

			pCurrent->x=XPos;
			pCurrent->y=YPos;

			XPos+=1.0f;
			pCurrent+=1;

		}

		YPos+=1.0f;

	}

}

void Pete_RectDistort_SetOffsetRectangle(
	SPete_RectDistort_Data* pInstanceData,
	int nLeftX,
	int nTopY,
	int nRightX,
	int nBottomY,
	const float XDelta,
	const float YDelta) {

	SPete_RectDistort_Coord* pMapStart=(SPete_RectDistort_Coord*)Pete_LockHandle(pInstanceData->hCoordMap);
	if (pMapStart==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nRectWidth=nRightX-nLeftX;
	const int nRectHeight=nBottomY-nTopY;

	const int nStartX=GetTiled(nLeftX,nWidth);
	const int nStartY=GetTiled(nTopY,nHeight);

	const int nEndX=nStartX+nRectWidth;
	const int nEndY=nStartY+nRectHeight;

	int nY;
	for (nY=nStartY; nY<nEndY; nY+=1) {

		const int nGatedY=GetTiled(nY,nHeight);
		SPete_RectDistort_Coord* pLineStart=pMapStart+(nGatedY*nWidth);

		int nX;
		for (nX=nStartX; nX<nEndX; nX+=1) {

			const int nGatedX=GetTiled(nX,nWidth);

			SPete_RectDistort_Coord* pCurrent=pLineStart+nGatedX;

			pCurrent->x+=XDelta;
			pCurrent->y+=YDelta;

		}

	}

}

void Pete_RectDistort_DrawRectanglesToMap(SPete_RectDistort_Data* pInstanceData,SPete_RectDistort_Settings* pSettings) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nRectCount=static_cast<int>(pSettings->m_RectangleRate);

	const float XDeltaScale=pSettings->m_RectangleXDeltaScale;
	const float YDeltaScale=pSettings->m_RectangleYDeltaScale;

	const float XDeltaOffset=XDeltaScale/2.0f;
	const float YDeltaOffset=YDeltaScale/2.0f;

	int nCount;
	for (nCount=0; nCount<nRectCount; ++nCount) {

		Pete_RectDistort_SetOffsetRectangle(
			pInstanceData,
			GetRandomFloat()*nWidth,
			GetRandomFloat()*nHeight,
			GetRandomFloat()*nWidth,
			GetRandomFloat()*nHeight,
			(GetRandomFloat()*XDeltaScale)-XDeltaOffset,
			(GetRandomFloat()*YDeltaScale)-YDeltaOffset);


	}

}


void Pete_RectDistort_CoordMapDecay(SPete_RectDistort_Data* pInstanceData,float Persistence) {

	SPete_RectDistort_Coord* pMapStart=(SPete_RectDistort_Coord*)Pete_LockHandle(pInstanceData->hCoordMap);
	if (pMapStart==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nNumPixels=(nWidth*nHeight);

	SPete_RectDistort_Coord* pCurrent=pMapStart;
	const SPete_RectDistort_Coord* pMapEnd=(pMapStart+nNumPixels);

	float YPos=0.0f;
	while (pCurrent!=pMapEnd) {

		const SPete_RectDistort_Coord* pLineEnd=pCurrent+nWidth;

		float XPos=0.0f;
		while (pCurrent!=pLineEnd) {

			const float OriginalX=pCurrent->x;
			const float OriginalY=pCurrent->y;

			const float DeltaX=(OriginalX-XPos);
			const float DeltaY=(OriginalY-YPos);

			const float DecayedDeltaX=(DeltaX*Persistence);
			const float DecayedDeltaY=(DeltaY*Persistence);

			const float ResultX=XPos+DecayedDeltaX;
			const float ResultY=YPos+DecayedDeltaY;

			pCurrent->x=ResultX;
			pCurrent->y=ResultY;

			XPos+=1.0f;
			pCurrent+=1;

		}

		YPos+=1.0f;

	}

}
