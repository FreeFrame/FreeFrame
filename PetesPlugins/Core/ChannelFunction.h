#ifndef INCLUDE_CHANNELFUNCTION_H
#define INCLUDE_CHANNELFUNCTION_H

#include "PeteHelpers.h"

const int nChannelFunction_Levels=256;

typedef struct SPete_ChannelFunction_Settings {
	int m_nWidth;
	int m_nHeight;
	int m_nRedTable[nChannelFunction_Levels];
	int m_nGreenTable[nChannelFunction_Levels];
	int m_nBlueTable[nChannelFunction_Levels];
} SPete_ChannelFunction_Settings;

void Pete_ChannelFunction_Render(SPete_ChannelFunction_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_CHANNELFUNCTION_H