#ifndef INCLUDE_FISHEYE_H
#define INCLUDE_FISHEYE_H

#include "PeteHelpers.h"

struct SPete_FishEye_Settings {
	float m_Distortion;
	float m_MaxRadius;
	float m_CentreX;
	float m_CentreY;
};

struct SPete_FishEye_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hRadiusTable;
	SPete_MemHandle hACosTable;
};

void Pete_FishEye_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_FishEye_Init(SPete_FishEye_Data* pInstanceData,int nWidth, int nHeight);
void Pete_FishEye_DeInit(SPete_FishEye_Data* pInstanceData);
void Pete_FishEye_Render(SPete_FishEye_Data* pInstanceData,SPete_FishEye_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_FISHEYE_H