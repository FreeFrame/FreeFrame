#ifndef INCLUDE_BACKDROPKEY_H
#define INCLUDE_BACKDROPKEY_H

#include "PeteHelpers.h"

struct SPete_BackDropKey_Settings {
	float m_UseThisFrameAsBackDrop;
	float m_Threshold;
};

struct SPete_BackDropKey_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hBackDropBuffer;
	bool bBackDropBufferFilled;
};

void Pete_BackDropKey_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_BackDropKey_Init(SPete_BackDropKey_Data* pInstance,int nWidth, int nHeight);
void Pete_BackDropKey_DeInit(SPete_BackDropKey_Data* pInstanceData);
void Pete_BackDropKey_Render(SPete_BackDropKey_Data* pInstanceData,SPete_BackDropKey_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_BACKDROPKEY_H