#ifndef INCLUDE_TVPIXEL_H
#define INCLUDE_TVPIXEL_H

#include "PeteHelpers.h"

struct SPete_TVPixel_Settings {
	float m_Height;
};

struct SPete_TVPixel_Data {
	int nWidth;
	int nHeight;
};

void Pete_TVPixel_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_TVPixel_Init(SPete_TVPixel_Data* pInstance,int nWidth, int nHeight);
void Pete_TVPixel_DeInit(SPete_TVPixel_Data* pInstanceData);
void Pete_TVPixel_Render(SPete_TVPixel_Data* pInstanceData,SPete_TVPixel_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TVPIXEL_H