#ifndef INCLUDE_CHROMIUM_H
#define INCLUDE_CHROMIUM_H

#include "PeteHelpers.h"

struct SPete_Chromium_Settings {
	float m_Edge1;
	float m_Edge2;
	float m_RedTint;
	float m_GreenTint;
	float m_BlueTint;
};

struct SPete_Chromium_Data {
	int nWidth;
	int nHeight;
};

void Pete_Chromium_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Chromium_Init(SPete_Chromium_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Chromium_DeInit(SPete_Chromium_Data* pInstanceData);
void Pete_Chromium_Render(SPete_Chromium_Data* pInstanceData,SPete_Chromium_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_CHROMIUM_H