#ifndef INCLUDE_DILATE_H
#define INCLUDE_DILATE_H

#include "PeteHelpers.h"

struct SPete_Dilate_Settings {
	float m_DoErode;
};

struct SPete_Dilate_Data {
	int nWidth;
	int nHeight;
};

void Pete_Dilate_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Dilate_Init(SPete_Dilate_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Dilate_DeInit(SPete_Dilate_Data* pInstanceData);
void Pete_Dilate_Render(SPete_Dilate_Data* pInstanceData,SPete_Dilate_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_DILATE_H