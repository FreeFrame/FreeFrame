#ifndef INCLUDE_MIXER_H
#define INCLUDE_MIXER_H

#include "PeteHelpers.h"

struct SPete_Mixer_Settings {
	float m_MixValue;
};

struct SPete_Mixer_Data {
	int nWidth;
	int nHeight;
};

void Pete_Mixer_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Mixer_Init(SPete_Mixer_Data* pInstance,int nWidth, int nHeight);
void Pete_Mixer_DeInit(SPete_Mixer_Data* pInstanceData);
void Pete_Mixer_Render(SPete_Mixer_Data* pInstanceData,SPete_Mixer_Settings* pSettings,U32** ppSources, U32* pOutput);

#endif // INCLUDE_MIXER_H