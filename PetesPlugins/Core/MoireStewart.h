#ifndef INCLUDE_MOIRESTEWART_H
#define INCLUDE_MOIRESTEWART_H

#include "PeteHelpers.h"

struct SPete_MoireStewart_Settings {
	float m_Size1;
	float m_Centre1X;
	float m_Centre1Y;
	float m_Size2;
	float m_Centre2X;
	float m_Centre2Y;
};

struct SPete_MoireStewart_Data {
	int nWidth;
	int nHeight;
	int* pSqrtTable;
	int nSqrtTableSize;
};

void Pete_MoireStewart_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_MoireStewart_Init(SPete_MoireStewart_Data* pInstanceData,int nWidth, int nHeight);
void Pete_MoireStewart_DeInit(SPete_MoireStewart_Data* pInstanceData);
void Pete_MoireStewart_Render(SPete_MoireStewart_Data* pInstanceData,SPete_MoireStewart_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_MOIRESTEWART_H