#ifndef INCLUDE_TIMEWARP_H
#define INCLUDE_TIMEWARP_H

#include "PeteHelpers.h"

struct SPete_TimeWarp_Settings {
	float m_SineSlurPeriod;
	float m_SineSlurAmplitude;
	float m_StutterAmplitude;
	float m_CustomPlaybackOffset;
	float m_Style;
	float m_PolariBPM;
	float m_PolariDoLoop;
};

struct SPete_TimeWarp_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hStoredFrames;
	int nCurrentSlice;
	int nMaxStoredFramesCount;
	int nCurrentStoredFramesCount;
	int nCurrentStorageFrame;
	float PlaybackOffset;
	float SineSlurAngle;
	int nLastPolariTime;
};

void Pete_TimeWarp_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_TimeWarp_Init(SPete_TimeWarp_Data* pInstanceData,int nWidth, int nHeight,int nFramesToStore=50);
void Pete_TimeWarp_DeInit(SPete_TimeWarp_Data* pInstanceData);
void Pete_TimeWarp_Render(SPete_TimeWarp_Data* pInstanceData,SPete_TimeWarp_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TIMEWARP_H