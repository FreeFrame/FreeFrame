#ifndef INCLUDE_TIMEBLUR_H
#define INCLUDE_TIMEBLUR_H

#include "PeteHelpers.h"

struct SPete_TimeBlur_Settings {
	float m_Time;
};

struct SPete_TimeBlur_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hAverageFrame;
	SPete_MemHandle hDisplayFrame;
	SPete_MemHandle hFrameBuffer;
	int nCurrentFrameIndex;
	int nOldFrameIndex;
};

void Pete_TimeBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_TimeBlur_Init(SPete_TimeBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_TimeBlur_DeInit(SPete_TimeBlur_Data* pInstanceData);
void Pete_TimeBlur_Render(SPete_TimeBlur_Data* pInstanceData,SPete_TimeBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TIMEBLUR_H