#ifndef INCLUDE_COLOURSUPPRESS_H
#define INCLUDE_COLOURSUPPRESS_H

#include "PeteHelpers.h"

struct SPete_ColourSuppress_Settings {
	float m_SuppressionRed;
	float m_BalanceRed;
	float m_SuppressionGreen;
	float m_BalanceGreen;
	float m_SuppressionBlue;
	float m_BalanceBlue;
	float m_DoUseMinimum;
};

struct SPete_ColourSuppress_Data {
	int nWidth;
	int nHeight;
};

void Pete_ColourSuppress_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_ColourSuppress_Init(SPete_ColourSuppress_Data* pInstanceData,int nWidth, int nHeight);
void Pete_ColourSuppress_DeInit(SPete_ColourSuppress_Data* pInstanceData);
void Pete_ColourSuppress_Render(SPete_ColourSuppress_Data* pInstanceData,SPete_ColourSuppress_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_COLOURSUPPRESS_H
