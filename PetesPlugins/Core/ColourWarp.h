#ifndef INCLUDE_COLOURWARP_H
#define INCLUDE_COLOURWARP_H

#include "PeteHelpers.h"

struct SPete_ColourWarp_Settings {
	float m_HueOffset;
	float m_HueScale;
	float m_SaturationOffset;
	float m_SaturationScale;
	float m_ValueOffset;
	float m_ValueScale;
	float m_DoCheapAndNasty;
	float m_DoHSVToRGB;
	float m_DoRGBToHSV;
};

struct SPete_ColourWarp_Data {
	int nWidth;
	int nHeight;
};

void Pete_ColourWarp_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_ColourWarp_Init(SPete_ColourWarp_Data* pInstanceData,int nWidth, int nHeight);
void Pete_ColourWarp_DeInit(SPete_ColourWarp_Data* pInstanceData);
void Pete_ColourWarp_Render(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_COLOURWARP_H