#ifndef INCLUDE_HALFTONE_H
#define INCLUDE_HALFTONE_H

#include "PeteHelpers.h"

struct SPete_HalfTone_Settings {
	float m_CellSize;
	float m_Style;
	float m_Angle;
	float m_Smoothing;
};

struct SPete_HalfTone_Data {
	int nWidth;
	int nHeight;
};

void Pete_HalfTone_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_HalfTone_Init(SPete_HalfTone_Data* pInstanceData,int nWidth, int nHeight);
void Pete_HalfTone_DeInit(SPete_HalfTone_Data* pInstanceData);
void Pete_HalfTone_Render(SPete_HalfTone_Data* pInstanceData,SPete_HalfTone_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_HALFTONE_H