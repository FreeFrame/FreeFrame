#ifndef INCLUDE_WAVE_H
#define INCLUDE_WAVE_H

#include "PeteHelpers.h"

struct SPete_Wave_Cell {
	float RedHeight;
	float GreenHeight;
	float BlueHeight;

	float RedVelocity;
	float GreenVelocity;
	float BlueVelocity;
};

struct SPete_Wave_Settings {
	float	m_Friction;
	float	m_Speed;
};

struct SPete_Wave_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hWaveBuffer;
};

void Pete_Wave_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Wave_Init(SPete_Wave_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Wave_DeInit(SPete_Wave_Data* pInstanceData);
void Pete_Wave_Render(SPete_Wave_Data* pInstanceData,SPete_Wave_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_WAVE_H