#ifndef INCLUDE_STRETCH_H
#define INCLUDE_STRETCH_H

#include "PeteHelpers.h"

struct SPete_Stretch_Settings {
	float m_DoMirror;

	float m_TopLeftX;
	float m_TopLeftY;

	float m_TopRightX;
	float m_TopRightY;

	float m_BottomRightX;
	float m_BottomRightY;

	float m_BottomLeftX;
	float m_BottomLeftY;
};

struct SPete_Stretch_Data {
	int nWidth;
	int nHeight;
};

void Pete_Stretch_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Stretch_Init(SPete_Stretch_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Stretch_DeInit(SPete_Stretch_Data* pInstanceData);
void Pete_Stretch_Render(SPete_Stretch_Data* pInstanceData,SPete_Stretch_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_STRETCH_H