#ifndef INCLUDE_LEVELS_H
#define INCLUDE_LEVELS_H

#include "PeteHelpers.h"

struct SPete_Levels_Settings {
	float m_DoAuto;
	float m_DoUniform;
	float m_DoAllowInversion;

	float m_UniformInputFloor;
	float m_UniformInputCeiling;
	float m_UniformOutputFloor;
	float m_UniformOutputCeiling;

	float m_RedInputFloor;
	float m_RedInputCeiling;
	float m_RedOutputFloor;
	float m_RedOutputCeiling;

	float m_GreenInputFloor;
	float m_GreenInputCeiling;
	float m_GreenOutputFloor;
	float m_GreenOutputCeiling;

	float m_BlueInputFloor;
	float m_BlueInputCeiling;
	float m_BlueOutputFloor;
	float m_BlueOutputCeiling;

	float m_LowPercentile;
	float m_HighPercentile;
};

struct SPete_Levels_Data {
	int nWidth;
	int nHeight;
};

void Pete_Levels_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Levels_Init(SPete_Levels_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Levels_DeInit(SPete_Levels_Data* pInstanceData);
void Pete_Levels_Render(SPete_Levels_Data* pInstanceData,SPete_Levels_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_LEVELS_H