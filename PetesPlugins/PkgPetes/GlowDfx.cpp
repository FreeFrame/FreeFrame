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
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "Glow.h"

class CGlowDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CGlowDfx();
	CGlowDfx();
	
	BOOL	UpdateConfig();
	BOOL	Initialize();
	BOOL	SetupDialog();
	BOOL	Render(CScreen **ppInput, CScreen *pOutput);
	HBITMAP	GetLabelBitmap();
	DWORD	GetMemoryUsage();
	BOOL	ConfirmRenderSource(int i);
	DWORD	GetInputState();

protected:
	BOOL	GetConfigData(CConfigData *p);
	void	SetConfigData(CConfigData *p);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	float	m_InnerRadius;
	int		m_nInnerRadiusID;

	float	m_OuterRadius;
	int		m_nOuterRadiusID;

	float	m_FakeItPasses;
	int		m_nFakeItPassesID;

	float	m_Floor;
	int		m_nFloorID;

	float	m_Ceiling;
	int		m_nCeilingID;

	float	m_AspectRatio;
	int		m_nAspectRatioID;

	int		m_nMemUsage;

	SPete_Glow_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CGlowDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CGlowDfx)

BEGIN_OUTPUTSCREENS_DESC(CGlowDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CGlowDfx)

BEGIN_EFFECT_DESC(CGlowDfx, 10203216861471919, PETE_VJO_VERSION , "Petes\\Glow", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CGlowDfx) 

REGISTER_DFXFACTORY(CGlowDfx)

CGlowDfx::CGlowDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_InnerRadius = 5.0f;
	m_OuterRadius = 15.0f;
	m_FakeItPasses = 1.0f;
	m_Floor = 0.0f;
	m_Ceiling = 32.0f;
	m_AspectRatio = 1.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CGlowDfx);
}

CGlowDfx::~CGlowDfx()
{

	Pete_Glow_DeInit(&m_InstanceData);

}

HBITMAP	CGlowDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CGlowDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CGlowDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CGlowDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CGlowDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nInnerRadiusID=RegisterFloat(m_pEngine,&m_InnerRadius,"Inner Radius",0.0f,255.0f);
	m_nOuterRadiusID=RegisterFloat(m_pEngine,&m_OuterRadius,"Outer Radius",0.0f,255.0f);
	m_nFakeItPassesID=RegisterFloat(m_pEngine,&m_FakeItPasses,"Number of passes when faking",1.0f,10.0f);
	m_nFloorID=RegisterFloat(m_pEngine,&m_Floor,"Floor",0.0f,255.0f);
	m_nCeilingID=RegisterFloat(m_pEngine,&m_Ceiling,"Ceiling",0.0f,255.0f);
	m_nAspectRatioID=RegisterFloat(m_pEngine,&m_AspectRatio,"Aspect Ratio",0.0f,2.0f);

	return TRUE;
}

BOOL	CGlowDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Glow_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CGlowDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Glow_Settings Settings;

	Settings.m_InnerRadius=m_InnerRadius;
	Settings.m_OuterRadius=m_OuterRadius;
	Settings.m_FakeItPasses=m_FakeItPasses;
	Settings.m_Floor=m_Floor;
	Settings.m_Ceiling=m_Ceiling;
	Settings.m_AspectRatio=m_AspectRatio;

	Pete_Glow_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CGlowDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CGlowDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CGlowDfx::GetInputState()
{
	return 0xffffffff;
}

