#ifndef INCLUDE_LUMAMASK_H
#define INCLUDE_LUMAMASK_H

#include "PeteHelpers.h"

struct SPete_LumaMask_Settings {
	float m_ThresholdStart;
	float m_ThresholdEnd;
};

struct SPete_LumaMask_Data {
	int nWidth;
	int nHeight;
};

void Pete_LumaMask_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LumaMask_Init(SPete_LumaMask_Data* pInstance,int nWidth, int nHeight);
void Pete_LumaMask_DeInit(SPete_LumaMask_Data* pInstanceData);
void Pete_LumaMask_Render(SPete_LumaMask_Data* pInstanceData,SPete_LumaMask_Settings* pSettings,U32* pSourceA,U32* pSourceB,U32* pMask,U32* pOutput);

#endif // INCLUDE_LUMAMASK_H