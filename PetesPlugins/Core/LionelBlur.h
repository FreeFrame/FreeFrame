#ifndef INCLUDE_LIONELBLUR_H
#define INCLUDE_LIONELBLUR_H

#include "PeteHelpers.h"

struct SPete_LionelBlur_Settings {
	float m_BlurRadius;
	float m_Angle;
};

struct SPete_LionelBlur_Data {
	int nWidth;
	int nHeight;
};

void Pete_LionelBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LionelBlur_Init(SPete_LionelBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_LionelBlur_DeInit(SPete_LionelBlur_Data* pInstanceData);
void Pete_LionelBlur_Render(SPete_LionelBlur_Data* pInstanceData,SPete_LionelBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_LIONELBLUR_H