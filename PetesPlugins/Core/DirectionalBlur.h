#ifndef INCLUDE_DIRECTIONALBLUR_H
#define INCLUDE_DIRECTIONALBLUR_H

#include "PeteHelpers.h"

struct SPete_DirectionalBlur_Settings {
	float m_Radius;
	float m_Angle;
};

struct SPete_DirectionalBlur_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hIntermediateBuffer;
};

void Pete_DirectionalBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_DirectionalBlur_Init(SPete_DirectionalBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_DirectionalBlur_DeInit(SPete_DirectionalBlur_Data* pInstanceData);
void Pete_DirectionalBlur_Render(SPete_DirectionalBlur_Data* pInstanceData,SPete_DirectionalBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_CHANNELFUNCTION_H