#ifndef INCLUDE_RADIANT_H
#define INCLUDE_RADIANT_H

#include "PeteHelpers.h"

struct SPete_Radiant_Settings {
	float m_SpikeScale;
	float m_SourcePointX;
	float m_SourcePointY;
	float m_BoostLight;
	float m_DoFade;
};

struct SPete_Radiant_AveragePixel {
	unsigned short nBlue;
	unsigned short nGreen;
	unsigned short nRed;
	unsigned short nWeight;
};

struct SPete_Radiant_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hAverageBuffer;
};

void Pete_Radiant_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Radiant_Init(SPete_Radiant_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Radiant_DeInit(SPete_Radiant_Data* pInstanceData);
void Pete_Radiant_Render(SPete_Radiant_Data* pInstanceData,SPete_Radiant_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_RADIANT_H