#ifndef INCLUDE_BURN_H
#define INCLUDE_BURN_H

#include "PeteHelpers.h"

struct SPete_Burn_Settings {
};

struct SPete_Burn_Data {
	int nWidth;
	int nHeight;
};

void Pete_Burn_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Burn_Init(SPete_Burn_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Burn_DeInit(SPete_Burn_Data* pInstanceData);
void Pete_Burn_Render(SPete_Burn_Data* pInstanceData,SPete_Burn_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_BURN_H