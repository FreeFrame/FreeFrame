#ifndef INCLUDE_METAIMAGE_H
#define INCLUDE_METAIMAGE_H

#include "PeteHelpers.h"

struct SPete_MetaImage_Settings {
	float m_Size;
	float m_DoDistanceBased;
	float m_DoCheapAndNasty;
};

struct SPete_MetaImage_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hSubImage;
};

void Pete_MetaImage_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_MetaImage_Init(SPete_MetaImage_Data* pInstanceData,int nWidth, int nHeight);
void Pete_MetaImage_DeInit(SPete_MetaImage_Data* pInstanceData);
void Pete_MetaImage_Render(SPete_MetaImage_Data* pInstanceData,SPete_MetaImage_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_METAIMAGE_H