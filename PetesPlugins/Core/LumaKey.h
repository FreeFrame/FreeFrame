#ifndef INCLUDE_LUMAKEY_H
#define INCLUDE_LUMAKEY_H

#include "PeteHelpers.h"

struct SPete_LumaKey_Settings {
	float m_Threshold;
	float m_Fuzziness;
};

struct SPete_LumaKey_Data {
	int nWidth;
	int nHeight;
};

void Pete_LumaKey_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LumaKey_Init(SPete_LumaKey_Data* pInstance,int nWidth, int nHeight);
void Pete_LumaKey_DeInit(SPete_LumaKey_Data* pInstanceData);
void Pete_LumaKey_Render(SPete_LumaKey_Data* pInstanceData,SPete_LumaKey_Settings* pSettings,U32** ppSources, U32* pOutput);

#endif // INCLUDE_LUMAKEY_H