#ifndef INCLUDE_DODGE_H
#define INCLUDE_DODGE_H

#include "PeteHelpers.h"

struct SPete_Dodge_Settings {
};

struct SPete_Dodge_Data {
	int nWidth;
	int nHeight;
};

void Pete_Dodge_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Dodge_Init(SPete_Dodge_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Dodge_DeInit(SPete_Dodge_Data* pInstanceData);
void Pete_Dodge_Render(SPete_Dodge_Data* pInstanceData,SPete_Dodge_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_DODGE_H