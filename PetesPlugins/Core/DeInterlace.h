#ifndef INCLUDE_DEINTERLACE_H
#define INCLUDE_DEINTERLACE_H

#include "PeteHelpers.h"

struct SPete_DeInterlace_Settings {
	float m_DoPalStretchFix;
};

struct SPete_DeInterlace_Data {
	int nWidth;
	int nHeight;
};

void Pete_DeInterlace_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_DeInterlace_Init(SPete_DeInterlace_Data* pInstanceData,int nWidth, int nHeight);
void Pete_DeInterlace_DeInit(SPete_DeInterlace_Data* pInstanceData);
void Pete_DeInterlace_Render(SPete_DeInterlace_Data* pInstanceData,SPete_DeInterlace_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_DEINTERLACE_H