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

#include "LumaCycle.h"
#include "PeteHelpers.h"

#include "ColourConversion.h"

#include "math.h"

const int nPaletteShift=10;
const int cnPaletteEntries=(1<<10);

void Pete_LumaCycle_SetupPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings);
void Pete_LumaCycle_SetupGradientPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings);
void Pete_LumaCycle_SetupSpectrumPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings);

static SPete_Parameter g_Parameters[]={
	{
		"Offset",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Start Colour Pos",
		0.25f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"End Colour Pos",
		0.75f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Start Colour Hue",
		0.17f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Start Colour Saturation",
		0.441f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Start Colour Value",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"End Colour Hue",
		0.68f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"End Colour Saturation",
		0.55f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"End Colour Value",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Spectrum Gradient",
		0.0f,
		1.0f,
		0.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_LumaCycle_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_LumaCycle_Init(SPete_LumaCycle_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_LumaCycle_DeInit(pInstanceData);

	const int nPaletteByteCount=
		(cnPaletteEntries*sizeof(U32));

	pInstanceData->hPalette=
		Pete_NewHandle(nPaletteByteCount);
	if (pInstanceData->hPalette==NULL) {
		Pete_LumaCycle_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_LumaCycle_DeInit(SPete_LumaCycle_Data* pInstanceData) {

	if (pInstanceData->hPalette!=NULL) {
		Pete_FreeHandle(pInstanceData->hPalette);
		pInstanceData->hPalette=NULL;
	}

}

void Pete_LumaCycle_Render(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings,U32* pSource,U32* pOutput) {

	Pete_LumaCycle_SetupPalette(pInstanceData,pSettings);

	U32* pPalette=(U32*)Pete_LockHandle(pInstanceData->hPalette);

	if (pPalette==NULL) {
		return;
	}

	const int nLuminanceShift=16;

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelsCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelsCount);

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;

	while (pCurrentSource<pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;

		const int nLuminance=GetLuminance(SourceColour);

		const int nPaletteIndex=
			(nLuminance>>(nLuminanceShift-nPaletteShift));

		const U32 OutputColour=pPalette[nPaletteIndex];

		*pCurrentOutput=OutputColour;

		pCurrentSource+=1;
		pCurrentOutput+=1;

	}

}

void Pete_LumaCycle_SetupPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings) {

	const bool bDoSpectrumGradient=(pSettings->m_DoSpectrumGradient>0.0f);

	if (bDoSpectrumGradient) {

		Pete_LumaCycle_SetupSpectrumPalette(pInstanceData,pSettings);

	} else {

		Pete_LumaCycle_SetupGradientPalette(pInstanceData,pSettings);

	}

}

void Pete_LumaCycle_SetupGradientPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings) {

	U32* pPalette=(U32*)Pete_LockHandle(pInstanceData->hPalette);

	if (pPalette==NULL) {
		return;
	}

	const float Offset=pSettings->m_Offset;

	const float StartPos=pSettings->m_StartColourPos;
	const float EndPos=pSettings->m_EndColourPos;

	const float StartHue=pSettings->m_StartColourHue;
	const float StartSaturation=pSettings->m_StartColourSaturation;
	const float StartValue=pSettings->m_StartColourValue;

	const float EndHue=pSettings->m_EndColourHue;
	const float EndSaturation=pSettings->m_EndColourSaturation;
	const float EndValue=pSettings->m_EndColourValue;

	float LowPos;
	float LowHue;
	float LowSaturation;
	float LowValue;

	float HighPos;
	float HighHue;
	float HighSaturation;
	float HighValue;

	if (StartPos<EndPos) {

		LowPos=StartPos;
		LowHue=StartHue;
		LowSaturation=StartSaturation;
		LowValue=StartValue;

		HighPos=EndPos;
		HighHue=EndHue;
		HighSaturation=EndSaturation;
		HighValue=EndValue;

	} else {

		LowPos=EndPos;
		LowHue=EndHue;
		LowSaturation=EndSaturation;
		LowValue=EndValue;

		HighPos=StartPos;
		HighHue=StartHue;
		HighSaturation=StartSaturation;
		HighValue=StartValue;

	}

	int nCount;
	for (nCount=0; nCount<cnPaletteEntries; nCount+=1) {

		float CurrentPos=
			(nCount/(float)cnPaletteEntries);
		CurrentPos+=Offset;
		CurrentPos=fmodf(CurrentPos,1.0f);

		float LowLerpValue;
		float HighLerpValue;

		if (CurrentPos<LowPos) {

			CurrentPos+=1.0f;

			float TotalDist=
				(LowPos+1.0f)-HighPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				(LowPos+1.0f)-CurrentPos;

			const float HighDist=
				CurrentPos-HighPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		} else if (CurrentPos>HighPos) {

			float TotalDist=
				(LowPos+1.0f)-HighPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				(LowPos+1.0f)-CurrentPos;

			const float HighDist=
				CurrentPos-HighPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		} else {

			float TotalDist=
				HighPos-LowPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				CurrentPos-LowPos;

			const float HighDist=
				HighPos-CurrentPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		}

		const float CurrentHue=
			(LowHue*LowLerpValue)+
			(HighHue*HighLerpValue);

		const float CurrentSaturation=
			(LowSaturation*LowLerpValue)+
			(HighSaturation*HighLerpValue);

		const float CurrentValue=
			(LowValue*LowLerpValue)+
			(HighValue*HighLerpValue);

		const int HueIntegral=(int)(CurrentHue*255.0f);
		const int SaturationIntegral=(int)(CurrentSaturation*255.0f);
		const int ValueIntegral=(int)(CurrentValue*255.0f);

		const U32 PackedHSV=
			(HueIntegral<<SHIFT_RED)|
			(SaturationIntegral<<SHIFT_GREEN)|
			(ValueIntegral<<SHIFT_BLUE);

		const U32 ResultColour=Pete_ConvertHSVToRGB(PackedHSV);

		pPalette[nCount]=ResultColour;

	}

}

void Pete_LumaCycle_SetupSpectrumPalette(SPete_LumaCycle_Data* pInstanceData,SPete_LumaCycle_Settings* pSettings) {

	U32* pPalette=(U32*)Pete_LockHandle(pInstanceData->hPalette);

	if (pPalette==NULL) {
		return;
	}

	const float Offset=pSettings->m_Offset;

	const float StartPos=pSettings->m_StartColourPos;
	const float EndPos=pSettings->m_EndColourPos;

	const float StartHue=0.0f;
	const float StartSaturation=pSettings->m_StartColourSaturation;
	const float StartValue=pSettings->m_StartColourValue;

	const float EndHue=1.0f;
	const float EndSaturation=pSettings->m_EndColourSaturation;
	const float EndValue=pSettings->m_EndColourValue;

	float LowPos;
	float LowHue;
	float LowSaturation;
	float LowValue;

	float HighPos;
	float HighHue;
	float HighSaturation;
	float HighValue;

	if (StartPos<EndPos) {

		LowPos=StartPos;
		LowHue=StartHue;
		LowSaturation=StartSaturation;
		LowValue=StartValue;

		HighPos=EndPos;
		HighHue=EndHue;
		HighSaturation=EndSaturation;
		HighValue=EndValue;

	} else {

		LowPos=EndPos;
		LowHue=EndHue;
		LowSaturation=EndSaturation;
		LowValue=EndValue;

		HighPos=StartPos;
		HighHue=StartHue;
		HighSaturation=StartSaturation;
		HighValue=StartValue;

	}

	int nCount;
	for (nCount=0; nCount<cnPaletteEntries; nCount+=1) {

		float CurrentPos=
			(nCount/(float)cnPaletteEntries);
		CurrentPos+=Offset;
		CurrentPos=fmodf(CurrentPos,1.0f);

		float LowLerpValue;
		float HighLerpValue;

		if (CurrentPos<LowPos) {

			CurrentPos+=1.0f;

			float TotalDist=
				(LowPos+1.0f)-HighPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				(LowPos+1.0f)-CurrentPos;

			const float HighDist=
				CurrentPos-HighPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		} else if (CurrentPos>HighPos) {

			float TotalDist=
				(LowPos+1.0f)-HighPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				(LowPos+1.0f)-CurrentPos;

			const float HighDist=
				CurrentPos-HighPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		} else {

			float TotalDist=
				HighPos-LowPos;

			if (TotalDist==0.0f) {
				TotalDist=0.0001f;
			}

			const float LowDist=
				CurrentPos-LowPos;

			const float HighDist=
				HighPos-CurrentPos;

			LowLerpValue=LowDist/TotalDist;
			HighLerpValue=HighDist/TotalDist;

		}

		const float CurrentHue=
			(LowHue*LowLerpValue)+
			(HighHue*HighLerpValue);

		const float CurrentSaturation=
			(LowSaturation*LowLerpValue)+
			(HighSaturation*HighLerpValue);

		const float CurrentValue=
			(LowValue*LowLerpValue)+
			(HighValue*HighLerpValue);

		const int HueIntegral=(int)(CurrentHue*255.0f);
		const int SaturationIntegral=(int)(CurrentSaturation*255.0f);
		const int ValueIntegral=(int)(CurrentValue*255.0f);

		const U32 PackedHSV=
			(HueIntegral<<SHIFT_RED)|
			(SaturationIntegral<<SHIFT_GREEN)|
			(ValueIntegral<<SHIFT_BLUE);

		const U32 ResultColour=Pete_ConvertHSVToRGB(PackedHSV);

		pPalette[nCount]=ResultColour;

	}

}




















