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

#include "Tile.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Skew Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Cell Width",
		0.16f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Cell Height",
		0.12f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Tile_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Tile_Init(SPete_Tile_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Tile_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Tile_DeInit(SPete_Tile_Data* pInstanceData) {
	// do nothing
}

void Pete_Tile_Render(SPete_Tile_Data* pInstanceData,SPete_Tile_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const float HalfWidth=(Width/2.0f);
	const float HalfHeight=(Height/2.0f);

	const float UAngleRadians=pSettings->m_Angle;

	const float VAngleRadians=(UAngleRadians+pSettings->m_SkewAngle+Pete_HalfPi);

	const float UNormalX=cos(UAngleRadians);
	const float UNormalY=sin(UAngleRadians);

	const float VNormalX=cos(VAngleRadians);
	const float VNormalY=sin(VAngleRadians);

	const float CellWidth=(pSettings->m_CellWidth*nWidth);
	const float CellHeight=(pSettings->m_CellHeight*nHeight);

	const float TwoCellWidth=(CellWidth*2.0f);
	const float TwoCellHeight=(CellHeight*2.0f);

	const int nPixelsCount=(nWidth*nHeight);

	U32* pOutputStart=pOutput;
	U32* pOutputEnd=(pOutput+nPixelsCount);

	U32* pCurrentOutput=pOutputStart;
	U32* pCurrentSource=pSource;

	float CurrentY=-HalfHeight;
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=(pOutputLineStart+nWidth);
		
		float CurrentX=-HalfWidth;

		while (pCurrentOutput<pOutputLineEnd) {

			float PosDotU=
				(CurrentX*UNormalX)+
				(CurrentY*UNormalY);

			PosDotU=fmodf(PosDotU,TwoCellWidth);
			if (PosDotU<0.0f) {
				PosDotU=TwoCellWidth+PosDotU;
			}
			if (PosDotU>CellWidth) {
				PosDotU=TwoCellWidth-PosDotU;
			}

			float PosDotV=
				(CurrentX*VNormalX)+
				(CurrentY*VNormalY);

			PosDotV=fmodf(PosDotV,TwoCellHeight);
			if (PosDotV<0.0f) {
				PosDotV=TwoCellHeight+PosDotV;
			}
			if (PosDotV>CellHeight) {
				PosDotV=TwoCellHeight-PosDotV;
			}


			const float SourceX=PosDotU+HalfWidth;
			const float SourceY=PosDotV+HalfHeight;
	
			int nSourceX=static_cast<int>(SourceX);
			int nSourceY=static_cast<int>(SourceY);

			nSourceX=GetMirrored(nSourceX,nWidth);
			nSourceY=GetMirrored(nSourceY,nHeight);

			U32* pTileSource=
				pSource+
				(nSourceY*nWidth)+
				nSourceX;

			*pCurrentOutput=*pTileSource;
			
			CurrentX+=1.0f;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}
		
		CurrentY+=1.0f;

	}
}
