#ifndef INCLUDE_GAUSSIANBLUR_H
#define INCLUDE_GAUSSIANBLUR_H

#include "PeteHelpers.h"

struct SPete_GaussianBlur_Settings {
	float m_Radius;
	float m_EdgeFactor;
	float m_DoFakeIt;
	float m_FakeItPasses;
	float m_DoUnSharpMask;
	float m_AspectRatio;
	float m_UnSharpMaskAmount;
	float m_DoOddButCool;
};

struct SPete_GaussianBlur_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hIntermediateBuffer;
	SPete_MemHandle hKernel;
	int nMaxKernelSize;
};

void Pete_GaussianBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_GaussianBlur_Init(SPete_GaussianBlur_Data* pInstanceData,int nWidth, int nHeight);
void Pete_GaussianBlur_DeInit(SPete_GaussianBlur_Data* pInstanceData);
void Pete_GaussianBlur_Render(SPete_GaussianBlur_Data* pInstanceData,SPete_GaussianBlur_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_CHANNELFUNCTION_H