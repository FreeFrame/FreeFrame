#ifndef INCLUDE_BLOOM_H
#define INCLUDE_BLOOM_H

#include "PeteHelpers.h"
#include "Levels.h"

struct SPete_Bloom_Settings {
	float m_Radius;
	float m_FakeItPasses;
	float m_Floor;
	float m_Ceiling;
	float m_AspectRatio;
	float m_BoostLight;
};

struct SPete_Bloom_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hIntermediateBuffer;
	SPete_Levels_Data m_LevelsInstanceData;
};

void Pete_Bloom_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Bloom_Init(SPete_Bloom_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Bloom_DeInit(SPete_Bloom_Data* pInstanceData);
void Pete_Bloom_Render(SPete_Bloom_Data* pInstanceData,SPete_Bloom_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_BLOOM_H