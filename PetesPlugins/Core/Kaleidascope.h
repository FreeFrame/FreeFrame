#ifndef INCLUDE_KALEIDASCOPE_H
#define INCLUDE_KALEIDASCOPE_H

#include "PeteHelpers.h"

struct SPete_Kaleidascope_Settings {
	float m_Divisions;
	float m_OutputAnglePreIncrement;
	float m_SourceAnglePreIncrement;
	float m_SourceCentreX;
	float m_SourceCentreY;
	float m_OutputCentreX;
	float m_OutputCentreY;
	float m_ReflectionLineProportion;
	float m_SourceAngleProportion;
};

struct SPete_AngleTable_Entry {
	int nAngleFA;
	int nDist;
};

struct SPete_Kaleidascope_Line {
	float X;
	float Y;
	U32 Flags;
};

struct SPete_Kaleidascope_Data {
	int nWidth;
	int nHeight;
	SPete_MemHandle hAngleTable;
	SPete_MemHandle hCosTable;
	SPete_MemHandle hLines;
	int nMaxLines;
};

void Pete_Kaleidascope_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_Kaleidascope_Init(SPete_Kaleidascope_Data* pInstanceData,int nWidth, int nHeight);
void Pete_Kaleidascope_DeInit(SPete_Kaleidascope_Data* pInstanceData);
void Pete_Kaleidascope_Render(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_KALEIDASCOPE_H