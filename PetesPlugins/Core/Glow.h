#ifndef INCLUDE_GLOW_H
#define INCLUDE_GLOW_H

#include "PeteHelpers.h"
#include "Levels.h"

struct SPete_Glow_Settings {
	float m_InnerRadius;
	float m_OuterRadius;
	float m_FakeItPasses;
	float m_Floor;
	float m_Ceiling;
	float m_AspectRatio;
};

struct SPete_Glow_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hIntermediateBuffer;
	SPete_MemHandle hOuterBuffer;
	SPete_Levels_Data m_LevelsInstanceData;
};

void Pete_Glow_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Glow_Init(SPete_Glow_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Glow_DeInit(SPete_Glow_Data* pInstanceData);
void Pete_Glow_Render(SPete_Glow_Data* pInstanceData,SPete_Glow_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_GLOW_H