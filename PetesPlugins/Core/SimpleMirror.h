#ifndef INCLUDE_SIMPLEMIRROR_H
#define INCLUDE_SIMPLEMIRROR_H

#include "PeteHelpers.h"

struct SPete_SimpleMirror_Settings {
	float m_Angle;
	float m_PlaneD;
	float m_DoSimpleMirrorAll;
};

struct SPete_SimpleMirror_Data {
	int nWidth;
	int nHeight;
};

void Pete_SimpleMirror_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_SimpleMirror_Init(SPete_SimpleMirror_Data* pInstanceData,int nWidth, int nHeight);
void Pete_SimpleMirror_DeInit(SPete_SimpleMirror_Data* pInstanceData);
void Pete_SimpleMirror_Render(SPete_SimpleMirror_Data* pInstanceData,SPete_SimpleMirror_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_SIMPLEMIRROR_H