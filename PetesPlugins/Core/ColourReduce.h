#ifndef INCLUDE_COLOURREDUCE_H
#define INCLUDE_COLOURREDUCE_H

#include "PeteHelpers.h"

struct SPete_ColourReduce_Settings {
	float m_TargetColourCount;
	float m_PalettePersistence;
	float m_BoundarySmoothing;
};

struct SPete_ColourReduce_InverseMapEntry {
	U32 ClosestColour;
	U32 NextClosestColour;
};

struct SPete_ColourReduce_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hRGBHistogram;
	SPete_MemHandle hSortedColours;
	SPete_MemHandle hInverseColourMap;
};

void Pete_ColourReduce_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);int Pete_ColourReduce_Init(SPete_ColourReduce_Data* pInstanceData,int nWidth, int nHeight);
void Pete_ColourReduce_DeInit(SPete_ColourReduce_Data* pInstanceData);
void Pete_ColourReduce_Render(SPete_ColourReduce_Data* pInstanceData,SPete_ColourReduce_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_COLOURMATCH_H