#ifndef INCLUDE_RECTDISTORT_H
#define INCLUDE_RECTDISTORT_H

#include "PeteHelpers.h"

struct SPete_RectDistort_Settings {
	float m_DoEnableClear;
	float m_DoEnableRectangles;
	float m_RectangleRate;
	float m_RectangleWidth;
	float m_RectangleHeight;
	float m_RectangleXDeltaScale;
	float m_RectangleYDeltaScale;
	float m_Persistence;
};

struct SPete_RectDistort_Coord {
	float x;
	float y;
};

struct SPete_RectDistort_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hCoordMap;
};

void Pete_RectDistort_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_RectDistort_Init(SPete_RectDistort_Data* pInstanceData,int nWidth, int nHeight);
void Pete_RectDistort_DeInit(SPete_RectDistort_Data* pInstanceData);
void Pete_RectDistort_Render(SPete_RectDistort_Data* pInstanceData,SPete_RectDistort_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_RECTDISTORT_H