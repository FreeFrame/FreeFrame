#ifndef INCLUDE_SOLARIZE_H
#define INCLUDE_SOLARIZE_H

#include "PeteHelpers.h"

struct SPete_Solarize_Settings {
	float m_Threshold;
	float m_Start;
	float m_End;
	float m_DoLimit;
	float m_Floor;
	float m_Ceiling;
};

struct SPete_Solarize_Data {
	int nWidth;
	int nHeight;
};

void Pete_Solarize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Solarize_Init(SPete_Solarize_Data* pInstanceData,int nWidth,int nHeight);
void Pete_Solarize_DeInit(SPete_Solarize_Data* pInstanceData);
void Pete_Solarize_Render(SPete_Solarize_Data* pInstanceData,SPete_Solarize_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_CHANNELFUNCTION_H