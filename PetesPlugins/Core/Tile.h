#ifndef INCLUDE_TILE_H
#define INCLUDE_TILE_H

#include "PeteHelpers.h"

struct SPete_Tile_Settings {
	float m_Angle;
	float m_SkewAngle;
	float m_CellWidth;
	float m_CellHeight;
};

struct SPete_Tile_Data {
	int nWidth;
	int nHeight;
};

void Pete_Tile_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Tile_Init(SPete_Tile_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Tile_DeInit(SPete_Tile_Data* pInstanceData);
void Pete_Tile_Render(SPete_Tile_Data* pInstanceData,SPete_Tile_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TILE_H