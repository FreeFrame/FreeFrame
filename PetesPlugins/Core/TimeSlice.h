#ifndef INCLUDE_TIMESLICE_H
#define INCLUDE_TIMESLICE_H

#include "PeteHelpers.h"

struct SPete_TimeSlice_Settings {
	float m_SineSlurPeriod;
	float m_SineSlurAmplitude;
	float m_StutterAmplitude;
	float m_CustomPlaybackOffset;
	float m_DoHorizontal;
	float m_SliceSize;
	float m_Style;
};

struct SPete_TimeSlice_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hStoredFrames;
	int nCurrentSlice;
	int nMaxStoredFramesCount;
	int nCurrentStoredFramesCount;
	int nCurrentStorageFrame;
	float PlaybackOffset;
	float SineSlurAngle;
};

void Pete_TimeSlice_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_TimeSlice_Init(SPete_TimeSlice_Data* pInstanceData,int nWidth, int nHeight,int nFramesToStore=50);
void Pete_TimeSlice_DeInit(SPete_TimeSlice_Data* pInstanceData);
void Pete_TimeSlice_Render(SPete_TimeSlice_Data* pInstanceData,SPete_TimeSlice_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_TIMESLICE_H