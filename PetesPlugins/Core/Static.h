#ifndef INCLUDE_STATIC_H
#define INCLUDE_STATIC_H

#include "PeteHelpers.h"

struct SPete_Static_Settings {
	float m_DoGreyScale;
	float m_DoTwoTone;
};

struct SPete_Static_Data {
	int nWidth;
	int nHeight;
};

void Pete_Static_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Static_Init(SPete_Static_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Static_DeInit(SPete_Static_Data* pInstanceData);
void Pete_Static_Render(SPete_Static_Data* pInstanceData,SPete_Static_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_STATIC_H