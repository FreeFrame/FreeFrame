#ifndef INCLUDE_SPIRALBLUR_H
#define INCLUDE_SPIRALBLUR_H

#include "PeteHelpers.h"

struct SPete_SpiralBlur_Settings {
	float m_Angle;
	float m_Scale;
	float m_LayerCount;
	float m_OriginX;
	float m_OriginY;
};

struct SPete_SpiralBlur_Data {
	int nWidth;
	int nHeight;
};

void Pete_SpiralBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_SpiralBlur_Init(SPete_SpiralBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_SpiralBlur_DeInit(SPete_SpiralBlur_Data* pInstanceData);
void Pete_SpiralBlur_Render(SPete_SpiralBlur_Data* pInstanceData,SPete_SpiralBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_SPIRALBLUR_H