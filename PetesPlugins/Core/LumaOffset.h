#ifndef INCLUDE_LUMAOFFSET_H
#define INCLUDE_LUMAOFFSET_H

#include "PeteHelpers.h"

struct SPete_LumaOffset_Settings {
	float	m_OffsetScale;
	float	m_LineGap;
	float	m_DoFilledLines;
	float	m_DoSmoothFill;
};

struct SPete_LumaOffset_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hPreviousLineHeights;
};

void Pete_LumaOffset_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LumaOffset_Init(SPete_LumaOffset_Data* pInstanceData,int nWidth, int nHeight);
void Pete_LumaOffset_DeInit(SPete_LumaOffset_Data* pInstanceData);
void Pete_LumaOffset_Render(SPete_LumaOffset_Data* pInstanceData,SPete_LumaOffset_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_LUMAOFFSET_H