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

#include "stdafx.h"

#include <math.h>

#include "pkgSAMPLES.h"
#include "ColourWarpDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "ColourWarp.h"
#include "PeteHelpers.h"

class CColourWarpDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CColourWarpDfx();
	CColourWarpDfx();

	BOOL	UpdateConfig(void);
	BOOL	Initialize(void);
	BOOL	SetupDialog(void);
	BOOL	Render(CScreen **ppInput, CScreen *pOutput);
	HBITMAP	GetLabelBitmap(void);
	DWORD	GetMemoryUsage(void);
	BOOL	ConfirmRenderSource(int i);
	DWORD	GetInputState(void);
	void	SetupTables(void);
	DWORD	ConvertRGBToHSV(DWORD ColourRGB);
	DWORD	ConvertHSVToRGB(DWORD ColourHSV);

protected:
	BOOL	GetConfigData(CConfigData *p);
	void	SetConfigData(CConfigData *p);

	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);
	void	WriteOutputFrameUsingTable(DWORD* pSource,DWORD* pOutput);
	void	WriteOutputFrameRGBToHSV(DWORD* pSource, DWORD* pOutput);
	void	WriteOutputFrameHSVToRGB(DWORD* pSource, DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	float	m_HueOffset;
	int		m_nHueOffsetID;

	float	m_HueScale;
	int		m_nHueScaleID;

	float	m_SaturationOffset;
	int		m_nSaturationOffsetID;

	float	m_SaturationScale;
	int		m_nSaturationScaleID;

	float	m_ValueOffset;
	int		m_nValueOffsetID;

	float	m_ValueScale;
	int		m_nValueScaleID;

	bool	m_bCheapAndNasty;
	int		m_nCheapAndNastyID;

	bool	m_bHSVToRGB;
	int		m_nHSVToRGBID;

	bool	m_bRGBToHSV;
	int		m_nRGBToHSVID;

	SPete_ColourWarp_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CColourWarpDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CColourWarpDfx)

BEGIN_OUTPUTSCREENS_DESC(CColourWarpDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CColourWarpDfx)

BEGIN_EFFECT_DESC(CColourWarpDfx, 10203216861479949, PETE_VJO_VERSION , "Petes\\ColourWarp", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CColourWarpDfx)

REGISTER_DFXFACTORY(CColourWarpDfx)

CColourWarpDfx::CColourWarpDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_HueOffset=0.0f;
	m_HueScale=1.0f;
	m_SaturationOffset=0.0f;
	m_SaturationScale=1.0f;
	m_ValueOffset=0.0f;
	m_ValueScale=1.0f;
	m_bCheapAndNasty=false;
	m_bRGBToHSV=false;
	m_bHSVToRGB=false;

	m_nMemUsage = sizeof(CColourWarpDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CColourWarpDfx::~CColourWarpDfx()
{
	Pete_ColourWarp_DeInit(&m_InstanceData);
}

HBITMAP	CColourWarpDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CColourWarpDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;
}

void	CColourWarpDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CColourWarpDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CColourWarpDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nHueOffsetID=
		RegisterFloat(m_pEngine,&m_HueOffset,"Hue Offset",0.0f,1.0f);

	m_nHueScaleID=
		RegisterFloat(m_pEngine,&m_HueScale,"Hue Scale",0.0f,1.0f);

	m_nSaturationOffsetID=
		RegisterFloat(m_pEngine,&m_SaturationOffset,"Saturation Offset",-1.0f,1.0f);

	m_nSaturationScaleID=
		RegisterFloat(m_pEngine,&m_SaturationScale,"Saturation Scale",-10.0f,10.0f);

	m_nValueOffsetID=
		RegisterFloat(m_pEngine,&m_ValueOffset,"Value Offset",-1.0f,1.0f);

	m_nValueScaleID=
		RegisterFloat(m_pEngine,&m_ValueScale,"Value Scale",-10.0f,10.0f);

	m_nCheapAndNastyID=
		RegisterBool(m_pEngine,&m_bCheapAndNasty,"Cheap And Nasty");

	m_nHSVToRGBID=
		RegisterBool(m_pEngine,&m_bHSVToRGB,"HSV to RGB");

	m_nRGBToHSVID=
		RegisterBool(m_pEngine,&m_bRGBToHSV,"RGB to HSV");

	return TRUE;
}

BOOL	CColourWarpDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_ColourWarp_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CColourWarpDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_ColourWarp_Settings Settings;
	Settings.m_HueOffset=m_HueOffset;
	Settings.m_HueScale=m_HueScale;
	Settings.m_SaturationOffset=m_SaturationOffset;
	Settings.m_SaturationScale=m_SaturationScale;
	Settings.m_ValueOffset=m_ValueOffset;
	Settings.m_ValueScale=m_ValueScale;
	Settings.m_DoCheapAndNasty=(m_bCheapAndNasty?100.0f:0.0f);
	Settings.m_DoHSVToRGB=(m_bHSVToRGB?100.0f:0.0f);
	Settings.m_DoRGBToHSV=(m_bRGBToHSV?100.0f:0.0f);

	Pete_ColourWarp_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CColourWarpDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CColourWarpDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CColourWarpDfx::GetInputState()
{
	return 0xffffffff;
}

