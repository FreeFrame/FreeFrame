#ifndef INCLUDE_SMEAR_H
#define INCLUDE_SMEAR_H

#include "PeteHelpers.h"

struct SPete_Smear_Settings {
	float m_Angle;
	float m_PosX;
	float m_PosY;
	float m_DoSmearAll;
	float m_BevelSize;
	float m_BevelScale;
};

struct SPete_Smear_Data {
	int nWidth;
	int nHeight;
};

void Pete_Smear_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Smear_Init(SPete_Smear_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Smear_DeInit(SPete_Smear_Data* pInstanceData);
void Pete_Smear_Render(SPete_Smear_Data* pInstanceData,SPete_Smear_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_SMEAR_H