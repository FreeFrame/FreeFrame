#ifndef INCLUDE_POSTERIZE_H
#define INCLUDE_POSTERIZE_H

#include "PeteHelpers.h"

struct SPete_Posterize_Settings {
	float m_PosterizeLevels;
};

struct SPete_Posterize_Data {
	int nWidth;
	int nHeight;
};

void Pete_Posterize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Posterize_Init(SPete_Posterize_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Posterize_DeInit(SPete_Posterize_Data* pInstanceData);
void Pete_Posterize_Render(SPete_Posterize_Data* pInstanceData,SPete_Posterize_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_POSTERIZE_H