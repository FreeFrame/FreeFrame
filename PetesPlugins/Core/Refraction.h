#ifndef INCLUDE_REFRACTION_H
#define INCLUDE_REFRACTION_H

#include "PeteHelpers.h"

struct SPete_Refraction_Settings {
	float m_Refraction;
	float m_CellWidth;
	float m_CellHeight;
	float m_DoAllowMagnification;
};

struct SPete_Refraction_Data {
	int nWidth;
	int nHeight;
};

void Pete_Refraction_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Refraction_Init(SPete_Refraction_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Refraction_DeInit(SPete_Refraction_Data* pInstanceData);
void Pete_Refraction_Render(SPete_Refraction_Data* pInstanceData,SPete_Refraction_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_REFRACTION_H