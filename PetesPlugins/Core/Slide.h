#ifndef INCLUDE_SLIDE_H
#define INCLUDE_SLIDE_H

#include "PeteHelpers.h"

struct SPete_Slide_Settings {
	float m_DoHorizontal;
	float m_DoMirror;
	float m_DoMirrorFlip;
	float m_Proportion;
	float m_Speed1;
	float m_Speed2;
};

struct SPete_Slide_Data {
	int nWidth;
	int nHeight;
	float m_Time;
};

void Pete_Slide_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Slide_Init(SPete_Slide_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Slide_DeInit(SPete_Slide_Data* pInstanceData);
void Pete_Slide_Render(SPete_Slide_Data* pInstanceData,SPete_Slide_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_SLIDE_H