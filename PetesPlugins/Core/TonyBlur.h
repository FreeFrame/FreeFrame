#ifndef INCLUDE_TONYBLUR_H
#define INCLUDE_TONYBLUR_H

#include "PeteHelpers.h"

struct SPete_TonyBlur_Settings {
	float m_BlurRadius;
	float m_CentreX;
	float m_CentreY;
	float m_ClearRadius;
	float m_Passes;
};

struct SPete_TonyBlur_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hSummedAreaTable;
	SPete_MemHandle hDistanceTable;
};

void Pete_TonyBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_TonyBlur_Init(SPete_TonyBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_TonyBlur_DeInit(SPete_TonyBlur_Data* pInstanceData);
void Pete_TonyBlur_Render(SPete_TonyBlur_Data* pInstanceData,SPete_TonyBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TONYBLUR_H