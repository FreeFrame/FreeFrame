#ifndef INCLUDE_EXCLUSION_H
#define INCLUDE_EXCLUSION_H

#include "PeteHelpers.h"

struct SPete_Exclusion_Settings {
};

struct SPete_Exclusion_Data {
	int nWidth;
	int nHeight;
};

void Pete_Exclusion_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Exclusion_Init(SPete_Exclusion_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Exclusion_DeInit(SPete_Exclusion_Data* pInstanceData);
void Pete_Exclusion_Render(SPete_Exclusion_Data* pInstanceData,SPete_Exclusion_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_EXCLUSION_H