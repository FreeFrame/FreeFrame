#ifndef INCLUDE_RADIALBLUR_H
#define INCLUDE_RADIALBLUR_H

#include "PeteHelpers.h"

struct SPete_RadialBlur_Settings {
	float m_LayerScale;
	enum {eLowQuality, eMediumQuality, eHighQuality};
	float m_eQuality;
};

struct SPete_RadialBlur_Data {
	int nWidth;
	int nHeight;
};

void Pete_RadialBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_RadialBlur_Init(SPete_RadialBlur_Data* pInstanceData,int nWidth,int nHeight);
void Pete_RadialBlur_DeInit(SPete_RadialBlur_Data* pInstanceData);
void Pete_RadialBlur_Render(SPete_RadialBlur_Data* pInstanceData,SPete_RadialBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_RADIALBLUR_H