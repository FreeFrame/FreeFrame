#ifndef INCLUDE_PANSPINZOOM_H
#define INCLUDE_PANSPINZOOM_H

#include "PeteHelpers.h"

struct SPete_PanSpinZoom_Settings {
	float OriginX;
	float OriginY;
	float Rotation;
	float Scale;
};

struct SPete_PanSpinZoom_Data {
	int nWidth;
	int nHeight;
};

void Pete_PanSpinZoom_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_PanSpinZoom_Init(SPete_PanSpinZoom_Data* pInstanceData,int nWidth, int nHeight);
void Pete_PanSpinZoom_DeInit(SPete_PanSpinZoom_Data* pInstanceData);
void Pete_PanSpinZoom_Render(SPete_PanSpinZoom_Data* pInstanceData,SPete_PanSpinZoom_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_PANSPINZOOM_H