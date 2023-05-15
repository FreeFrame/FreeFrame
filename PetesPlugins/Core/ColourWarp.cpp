/* Copyright (C) 2002 Pete Warden

Effect plugin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


#include "ColourWarp.h"

#include "PeteHelpers.h"
#include "ColourConversion.h"

const int cnRGBToHSVTableCount=(1<<15);
const int cnHSVToRGBTableCount=(1<<15);

static U32 g_RGBToHSVTable[cnRGBToHSVTableCount];
static U32 g_HSVToRGBTable[cnHSVToRGBTableCount];
static bool g_bAreTablesSetup=false;

void Pete_ColourWarp_SetupTables(void);
void Pete_ColourWarp_RenderNormal(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput);
void Pete_ColourWarp_RenderRGBToHSV(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput);
void Pete_ColourWarp_RenderHSVToRGB(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput);
void Pete_ColourWarp_RenderUsingTable(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput);

static SPete_Parameter g_Parameters[]={
	{
		"Hue Offset",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Hue Scale",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Saturation Offset",
		0.0f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Saturation Scale",
		1.0f,
		-10.0f,
		10.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Value Offset",
		0.0f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Value Scale",
		1.0f,
		-10.0f,
		10.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Cheap And Nasty",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"RGB To HSV",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"HSV To RGB",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_ColourWarp_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_ColourWarp_Init(SPete_ColourWarp_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_ColourWarp_DeInit(pInstanceData);

	if (!g_bAreTablesSetup) {
		Pete_ColourWarp_SetupTables();
		g_bAreTablesSetup=true;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_ColourWarp_DeInit(SPete_ColourWarp_Data* pInstanceData) {

	// do nothing

}

void Pete_ColourWarp_Render(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource,U32* pOutput) {

	if (pSettings->m_DoCheapAndNasty>0.0f) {

		Pete_ColourWarp_RenderUsingTable(pInstanceData,pSettings,pSource,pOutput);

	} else if (pSettings->m_DoHSVToRGB>0.0f) {

		Pete_ColourWarp_RenderHSVToRGB(pInstanceData,pSettings,pSource,pOutput);

	} else if (pSettings->m_DoRGBToHSV>0.0f) {

		Pete_ColourWarp_RenderRGBToHSV(pInstanceData,pSettings,pSource,pOutput);

	} else {

		Pete_ColourWarp_RenderNormal(pInstanceData,pSettings,pSource,pOutput);

	}

}

void Pete_ColourWarp_SetupTables(void) {

	int nCount;

	for (nCount=0; nCount<cnRGBToHSVTableCount; nCount+=1) {

		int nRed=(((nCount>>10)&0x1f)<<3)+3;
		int nGreen=(((nCount>>5)&0x1f)<<3)+3;
		int nBlue=(((nCount>>SHIFT_BLUE)&0x1f)<<3)+3;

		U32 ColourRGB=(nRed<<SHIFT_RED)|(nGreen<<SHIFT_GREEN)|(nBlue<<SHIFT_BLUE);

		U32 ColourHSV=Pete_ConvertRGBToHSV(ColourRGB);

		g_RGBToHSVTable[nCount]=ColourHSV;

	}

	for (nCount=0; nCount<cnHSVToRGBTableCount; nCount+=1) {

		int nHue=(((nCount>>10)&0x1f)<<3)+3;
		int nSaturation=(((nCount>>5)&0x1f)<<3)+3;
		int nValue=(((nCount>>SHIFT_BLUE)&0x1f)<<3)+3;

		U32 ColourHSV=(nHue<<SHIFT_RED)|(nSaturation<<SHIFT_GREEN)|(nValue<<SHIFT_BLUE);

		U32 ColourRGB=Pete_ConvertHSVToRGB(ColourHSV);

		g_HSVToRGBTable[nCount]=ColourRGB;

	}

}

void Pete_ColourWarp_RenderNormal(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	const int nHueOffset = static_cast<int>(pSettings->m_HueOffset*(64*6));
	const int nHueScale = static_cast<int>(pSettings->m_HueScale*cnFixedPrec);

	const int nSaturationOffset = static_cast<int>(pSettings->m_SaturationOffset*255.0f);
	const int nSaturationScale = static_cast<int>(pSettings->m_SaturationScale*cnFixedPrec);

	const int nValueOffset = static_cast<int>(pSettings->m_ValueOffset*255.0f);
	const int nValueScale = static_cast<int>(pSettings->m_ValueScale*cnFixedPrec);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;
		pCurrentSource+=1;

		const int nSourceRed=(SourceColour&(0xff<<SHIFT_RED))>>SHIFT_RED;
		const int nSourceGreen=(SourceColour&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
		const int nSourceBlue=(SourceColour&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

		int nHue;
		int nSaturation;
		int nValue;

		int iMax, iMin;

		// Calculate the value component
		if (nSourceRed > nSourceGreen) {
			iMax = nSourceRed;
			iMin = nSourceGreen;
		} else {
			iMin = nSourceRed;
			iMax = nSourceGreen;
		}

		if (nSourceBlue > iMax)
			iMax = nSourceBlue;

		if (nSourceBlue < iMin)
			iMin = nSourceBlue;

		nValue = iMax;

		// Calculate the saturation component
		if (iMax != 0) {
			nSaturation = 256 * (iMax - iMin) / iMax;
		} else {
			nSaturation = 0;
		}

		// Calculate the hue
		if (nSaturation == 0) {
			nHue = 0;
		} else {
			int xHue;
			int nDelta;

			nDelta = (iMax - iMin);

			if (nSourceRed == iMax) {
				xHue = ((nSourceGreen - nSourceBlue)<<cnFixedShift) / nDelta;
			} else if (nSourceGreen == iMax) {
				xHue = (2<<cnFixedShift) + (((nSourceBlue - nSourceRed)<<cnFixedShift) / nDelta);
			} else {
				xHue = (4<<cnFixedShift) + (((nSourceRed - nSourceGreen)<<cnFixedShift) / nDelta);
			}

			xHue *= 64;

			if (xHue < 0) {
				xHue += ((64*6)<<cnFixedShift);
			}

			nHue = xHue>>cnFixedShift;

		}

		nHue=(((nHue*nHueScale)>>cnFixedShift)+nHueOffset)%(64*6);

		nSaturation=((nSaturation+nSaturationOffset)*nSaturationScale)>>cnFixedShift;

		if (nSaturation<0) {
			nSaturation=0;
		} else if (nSaturation>255) {
			nSaturation=255;
		}

		nValue=((nValue+nValueOffset)*nValueScale)>>cnFixedShift;

		if (nValue<0) {
			nValue=0;
		} else if (nValue&256) {
			nValue=255;
		}

		const int xHueSextant = (nHue*cnFixedPrec) / 64;
		const int nIntSextant = (xHueSextant>>cnFixedShift);
		const int xf = xHueSextant%cnFixedPrec;;
		const int xValue = (nValue<<(cnFixedShift-8));
		const int xSaturation = (nSaturation<<(cnFixedShift-8));

		const int xp = (xValue * (cnFixedPrec - xSaturation))>>cnFixedShift;
		const int xq = (xValue * (cnFixedPrec -
			((xSaturation * xf)>>cnFixedShift)))>>cnFixedShift;
		const int xt = (xValue * (cnFixedPrec -
			((xSaturation * (cnFixedPrec - xf))>>cnFixedShift) ))>>cnFixedShift;

		int nOutputRed;
		int nOutputGreen;
		int nOutputBlue;

		switch (nIntSextant) {
			case 0:
			nOutputRed = xValue>>(cnFixedShift-8);
			nOutputGreen = xt>>(cnFixedShift-8);
			nOutputBlue = xp>>(cnFixedShift-8);
			break;

			case 1:
			nOutputRed = xq>>(cnFixedShift-8);
			nOutputGreen = xValue>>(cnFixedShift-8);
			nOutputBlue = xp>>(cnFixedShift-8);
			break;

			case 2:
			nOutputRed = xp>>(cnFixedShift-8);
			nOutputGreen = xValue>>(cnFixedShift-8);
			nOutputBlue = xt>>(cnFixedShift-8);
			break;

			case 3:
			nOutputRed = xp>>(cnFixedShift-8);
			nOutputGreen = xq>>(cnFixedShift-8);
			nOutputBlue = xValue>>(cnFixedShift-8);
			break;

			case 4:
			nOutputRed = xt>>(cnFixedShift-8);
			nOutputGreen = xp>>(cnFixedShift-8);
			nOutputBlue = xValue>>(cnFixedShift-8);
			break;

			case 5:
			nOutputRed = xValue>>(cnFixedShift-8);
			nOutputGreen = xp>>(cnFixedShift-8);
			nOutputBlue = xq>>(cnFixedShift-8);
			break;
		}

		U32 OutputColour=(nOutputRed&0xff)<<SHIFT_RED;
		OutputColour|=(nOutputGreen&0xff)<<SHIFT_GREEN;
		OutputColour|=(nOutputBlue&0xff)<<SHIFT_BLUE;

		*pCurrentOutput=OutputColour;
		pCurrentOutput+=1;

	}

}

void Pete_ColourWarp_RenderRGBToHSV(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;
		pCurrentSource+=1;

		const int nSourceRed=(SourceColour&(0xff<<SHIFT_RED))>>SHIFT_RED;
		const int nSourceGreen=(SourceColour&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
		const int nSourceBlue=(SourceColour&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

		int nHue;
		int nSaturation;
		int nValue;

		int iMax, iMin;

		// Calculate the value component
		if (nSourceRed > nSourceGreen) {
			iMax = nSourceRed;
			iMin = nSourceGreen;
		} else {
			iMin = nSourceRed;
			iMax = nSourceGreen;
		}

		if (nSourceBlue > iMax)
			iMax = nSourceBlue;

		if (nSourceBlue < iMin)
			iMin = nSourceBlue;

		nValue = iMax;

		// Calculate the saturation component
		if (iMax != 0) {
			nSaturation = 256 * (iMax - iMin) / iMax;
		} else {
			nSaturation = 0;
		}

		// Calculate the hue
		if (nSaturation == 0) {
			nHue = 0;
		} else {
			int xHue;
			int nDelta;

			nDelta = (iMax - iMin);

			if (nSourceRed == iMax) {
				xHue = ((nSourceGreen - nSourceBlue)<<cnFixedShift) / nDelta;
			} else if (nSourceGreen == iMax) {
				xHue = (2<<cnFixedShift) + (((nSourceBlue - nSourceRed)<<cnFixedShift) / nDelta);
			} else {
				xHue = (4<<cnFixedShift) + (((nSourceRed - nSourceGreen)<<cnFixedShift) / nDelta);
			}

			xHue *= 64;

			if (xHue < 0) {
				xHue += ((64*6)<<cnFixedShift);
			}

			nHue = xHue>>cnFixedShift;

		}

		const int nOutputRed=nHue;
		const int nOutputGreen=nSaturation;
		const int nOutputBlue=nValue;

		U32 OutputColour=(nOutputRed&0xff)<<SHIFT_RED;
		OutputColour|=(nOutputGreen&0xff)<<SHIFT_GREEN;
		OutputColour|=(nOutputBlue&0xff)<<SHIFT_BLUE;

		*pCurrentOutput=OutputColour;
		pCurrentOutput+=1;

	}

}

void Pete_ColourWarp_RenderHSVToRGB(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	const int nHueOffset = static_cast<int>(pSettings->m_HueOffset*(64*6));
	const int nHueScale = static_cast<int>(pSettings->m_HueScale*cnFixedPrec);

	const int nSaturationOffset = static_cast<int>(pSettings->m_SaturationOffset*255.0f);
	const int nSaturationScale = static_cast<int>(pSettings->m_SaturationScale*cnFixedPrec);

	const int nValueOffset = static_cast<int>(pSettings->m_ValueOffset*255.0f);
	const int nValueScale = static_cast<int>(pSettings->m_ValueScale*cnFixedPrec);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;
		pCurrentSource+=1;

		const int nHue=(SourceColour&(0xff<<SHIFT_RED))>>SHIFT_RED;
		const int nSaturation=(SourceColour&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
		const int nValue=(SourceColour&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

		const int xHueSextant = (nHue*cnFixedPrec) / 64;
		const int nIntSextant = (xHueSextant>>cnFixedShift);
		const int xf = xHueSextant%cnFixedPrec;;
		const int xValue = (nValue<<(cnFixedShift-8));
		const int xSaturation = (nSaturation<<(cnFixedShift-8));

		const int xp = (xValue * (cnFixedPrec - xSaturation))>>cnFixedShift;
		const int xq = (xValue * (cnFixedPrec -
			((xSaturation * xf)>>cnFixedShift)))>>cnFixedShift;
		const int xt = (xValue * (cnFixedPrec -
			((xSaturation * (cnFixedPrec - xf))>>cnFixedShift) ))>>cnFixedShift;

		int nOutputRed;
		int nOutputGreen;
		int nOutputBlue;

		switch (nIntSextant) {
			case 0:
			nOutputRed = xValue>>(cnFixedShift-8);
			nOutputGreen = xt>>(cnFixedShift-8);
			nOutputBlue = xp>>(cnFixedShift-8);
			break;

			case 1:
			nOutputRed = xq>>(cnFixedShift-8);
			nOutputGreen = xValue>>(cnFixedShift-8);
			nOutputBlue = xp>>(cnFixedShift-8);
			break;

			case 2:
			nOutputRed = xp>>(cnFixedShift-8);
			nOutputGreen = xValue>>(cnFixedShift-8);
			nOutputBlue = xt>>(cnFixedShift-8);
			break;

			case 3:
			nOutputRed = xp>>(cnFixedShift-8);
			nOutputGreen = xq>>(cnFixedShift-8);
			nOutputBlue = xValue>>(cnFixedShift-8);
			break;

			case 4:
			nOutputRed = xt>>(cnFixedShift-8);
			nOutputGreen = xp>>(cnFixedShift-8);
			nOutputBlue = xValue>>(cnFixedShift-8);
			break;

			case 5:
			nOutputRed = xValue>>(cnFixedShift-8);
			nOutputGreen = xp>>(cnFixedShift-8);
			nOutputBlue = xq>>(cnFixedShift-8);
			break;
		}

		U32 OutputColour=(nOutputRed&0xff)<<SHIFT_RED;
		OutputColour|=(nOutputGreen&0xff)<<SHIFT_GREEN;
		OutputColour|=(nOutputBlue&0xff)<<SHIFT_BLUE;

		*pCurrentOutput=OutputColour;
		pCurrentOutput+=1;

	}

}

void Pete_ColourWarp_RenderUsingTable(SPete_ColourWarp_Data* pInstanceData,SPete_ColourWarp_Settings* pSettings,U32* pSource, U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	const int nHueOffset = static_cast<int>(pSettings->m_HueOffset*255.0f);

	const int nSaturationOffset = static_cast<int>(pSettings->m_SaturationOffset*255.0f);
	const int nSaturationScale = static_cast<int>(pSettings->m_SaturationScale*cnFixedPrec);

	const int nValueOffset = static_cast<int>(pSettings->m_ValueOffset*255.0f);
	const int nValueScale = static_cast<int>(pSettings->m_ValueScale*cnFixedPrec);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;
		pCurrentSource+=1;

		int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		nSourceRed>>=3;

		int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		nSourceGreen>>=3;

		int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
		nSourceBlue>>=3;

		const int nRGB16Bit=(nSourceRed<<10)|(nSourceGreen<<5)|(nSourceBlue<<SHIFT_BLUE);

		const U32 ColourHSV=g_RGBToHSVTable[nRGB16Bit];

		int nHue=(ColourHSV>>SHIFT_RED)&0xff;
		int nSaturation=(ColourHSV>>SHIFT_GREEN)&0xff;
		int nValue=(ColourHSV>>SHIFT_BLUE)&0xff;


		nHue=(nHue+nHueOffset)&0xff;

		nSaturation=((nSaturation+nSaturationOffset)*nSaturationScale)>>cnFixedShift;

		if (nSaturation<0) {
			nSaturation=0;
		} else if (nSaturation>255) {
			nSaturation=255;
		}

		nValue=((nValue+nValueOffset)*nValueScale)>>cnFixedShift;

		if (nValue<0) {
			nValue=0;
		} else if (nValue&256) {
			nValue=255;
		}

		nHue>>=3;
		nSaturation>>=3;
		nValue>>=3;

		const int nHSV16Bit=(nHue<<10)|(nSaturation<<5)|(nValue<<SHIFT_BLUE);

		const U32 OutputRGB=g_HSVToRGBTable[nHSV16Bit];

		*pCurrentOutput=OutputRGB;

		pCurrentOutput+=1;

	}

}
