#ifndef INCLUDE_SQUASH_H
#define INCLUDE_SQUASH_H

#include "PeteHelpers.h"

struct SPete_Squash_Settings {
	float m_DoHorizontal;
	float m_Size;
};

struct SPete_Squash_Data {
	int nWidth;
	int nHeight;
};

void Pete_Squash_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Squash_Init(SPete_Squash_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Squash_DeInit(SPete_Squash_Data* pInstanceData);
void Pete_Squash_Render(SPete_Squash_Data* pInstanceData,SPete_Squash_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_SQUASH_H