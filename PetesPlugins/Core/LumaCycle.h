#ifndef INCLUDE_LUMACYCLE_H
#define INCLUDE_LUMACYCLE_H

#include "PeteHelpers.h"

struct SPete_LumaCycle_Settings {
	float m_Offset;
	float m_StartColourPos;
	float m_EndColourPos;
	float m_StartColourHue;
	float m_StartColourSaturation;
	float m_StartColourValue;
	float m_EndColourHue;
	float m_EndColourSaturation;
	float m_EndColourValue;
	float m_DoSpectrumGradient;
};

struct SPete_LumaCycle_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hPalette;
};

void Pete_LumaCycle_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LumaCycle_Init(SPete_LumaCycle_Data* pInstanceData,int nWidth, int nHeight);
void Pete_LumaCycle_DeInit(SPete_LumaCycle_Data* pInstanceData);
void Pete_LumaCycle_Render(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_LUMACYCLE_H