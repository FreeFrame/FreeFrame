#ifndef INCLUDE_BACKLIGHT_H
#define INCLUDE_BACKLIGHT_H

#include "PeteHelpers.h"

struct SPete_BackLight_Settings {
	float m_SpikeScale;
	float m_SpikeFloor;
	float m_SpikeCeiling;
};

struct SPete_BackLight_Data {
	int nWidth;
	int nHeight;
};

void Pete_BackLight_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_BackLight_Init(SPete_BackLight_Data* pInstanceData,int nWidth, int nHeight);
void Pete_BackLight_DeInit(SPete_BackLight_Data* pInstanceData);
void Pete_BackLight_Render(SPete_BackLight_Data* pInstanceData,SPete_BackLight_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_BACKLIGHT_H