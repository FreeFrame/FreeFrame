#ifndef INCLUDE_VECTORIZE_H
#define INCLUDE_VECTORIZE_H

#include "PeteHelpers.h"
#include "ColourReduce.h"

struct SPete_Vectorize_Settings {
	float m_PreFilterSize;
	float m_CellCount;
	float m_Monochrome;
	float m_ColourReduceLevels;
	float m_ColourReducePalettePersistence;
	float m_DifferenceThreshold;
};

const int nMaxMipMapCount=12;

struct SPete_Vectorize_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hIntermediateData;
	SPete_ColourReduce_Data ColourReduceInstanceData;
};

void Pete_Vectorize_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Vectorize_Init(SPete_Vectorize_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Vectorize_DeInit(SPete_Vectorize_Data* pInstanceData);
void Pete_Vectorize_Render(SPete_Vectorize_Data* pInstanceData,SPete_Vectorize_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_VECTORIZE_H