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

#include "Bloom.h"

class CBloomDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CBloomDfx();
	CBloomDfx();

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

	float	m_Radius;
	int		m_nRadiusID;

	float	m_FakeItPasses;
	int		m_nFakeItPassesID;

	float	m_Floor;
	int		m_nFloorID;

	float	m_Ceiling;
	int		m_nCeilingID;

	float	m_AspectRatio;
	int		m_nAspectRatioID;

	int		m_nMemUsage;

	SPete_Bloom_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CBloomDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CBloomDfx)

BEGIN_OUTPUTSCREENS_DESC(CBloomDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CBloomDfx)

BEGIN_EFFECT_DESC(CBloomDfx, 10203216861478919, PETE_VJO_VERSION , "Petes\\Bloom", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CBloomDfx)

REGISTER_DFXFACTORY(CBloomDfx)

CBloomDfx::CBloomDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Radius = 15.0f;
	m_FakeItPasses = 2.0f;
	m_Floor = 240.0f;
	m_Ceiling = 248.0f;
	m_AspectRatio = 1.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CBloomDfx);
}

CBloomDfx::~CBloomDfx()
{

	Pete_Bloom_DeInit(&m_InstanceData);

}

HBITMAP	CBloomDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CBloomDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CBloomDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CBloomDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CBloomDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nRadiusID=RegisterFloat(m_pEngine,&m_Radius,"Radius",1.0f,255.0f);
	m_nFakeItPassesID=RegisterFloat(m_pEngine,&m_FakeItPasses,"Number of passes when faking",1.0f,10.0f);
	m_nFloorID=RegisterFloat(m_pEngine,&m_Floor,"Floor",0.0f,255.0f);
	m_nCeilingID=RegisterFloat(m_pEngine,&m_Ceiling,"Ceiling",0.0f,255.0f);
	m_nAspectRatioID=RegisterFloat(m_pEngine,&m_AspectRatio,"Aspect Ratio",0.0f,2.0f);

	return TRUE;
}

BOOL	CBloomDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Bloom_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CBloomDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Bloom_Settings Settings;

	Settings.m_Radius=m_Radius;
	Settings.m_FakeItPasses=m_FakeItPasses;
	Settings.m_Floor=m_Floor;
	Settings.m_Ceiling=m_Ceiling;
	Settings.m_AspectRatio=m_AspectRatio;

	Pete_Bloom_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CBloomDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CBloomDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CBloomDfx::GetInputState()
{
	return 0xffffffff;
}

