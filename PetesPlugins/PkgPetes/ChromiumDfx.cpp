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

#include "Chromium.h"
#include "PeteHelpers.h"

class CChromiumDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CChromiumDfx();
	CChromiumDfx();

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

	float	m_Edge1;
	int		m_nEdge1ID;

	float	m_Edge2;
	int		m_nEdge2ID;

	float	m_RedTint;
	int		m_nRedTintID;

	float	m_GreenTint;
	int		m_nGreenTintID;

	float	m_BlueTint;
	int		m_nBlueTintID;

	DWORD   m_nMemUsage;

	SPete_Chromium_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CChromiumDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CChromiumDfx)

BEGIN_OUTPUTSCREENS_DESC(CChromiumDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CChromiumDfx)

BEGIN_EFFECT_DESC(CChromiumDfx, 10203216861479957, PETE_VJO_VERSION , "Petes\\Chromium", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CChromiumDfx)

REGISTER_DFXFACTORY(CChromiumDfx)

CChromiumDfx::CChromiumDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Edge1=0.0f;
	m_nEdge1ID=0;

	m_Edge2=128.0f;
	m_nEdge2ID=0;

	m_RedTint=7.0f;
	m_nRedTintID=0;

	m_GreenTint=22.0f;
	m_nGreenTintID=0;

	m_BlueTint=30.0f;
	m_nBlueTintID=0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CChromiumDfx);
}

CChromiumDfx::~CChromiumDfx()
{
	Pete_Chromium_DeInit(&m_InstanceData);
}

HBITMAP	CChromiumDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CChromiumDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CChromiumDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CChromiumDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CChromiumDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nEdge1ID=RegisterFloat(m_pEngine,&m_Edge1,"Edge 1",0.0f,255.0f);
	m_nEdge2ID=RegisterFloat(m_pEngine,&m_Edge2,"Edge 2",0.0f,255.0f);
	m_nRedTintID=RegisterFloat(m_pEngine,&m_RedTint,"Red Tint",0.0f,255.0f);
	m_nGreenTintID=RegisterFloat(m_pEngine,&m_GreenTint,"Green Tint",0.0f,255.0f);
	m_nBlueTintID=RegisterFloat(m_pEngine,&m_BlueTint,"Blue Tint",0.0f,255.0f);

	return TRUE;
}

BOOL	CChromiumDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Chromium_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CChromiumDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Chromium_Settings Settings;
	Settings.m_Edge1=m_Edge1;
	Settings.m_Edge2=m_Edge2;
	Settings.m_RedTint=m_RedTint;
	Settings.m_GreenTint=m_GreenTint;
	Settings.m_BlueTint=m_BlueTint;

	Pete_Chromium_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CChromiumDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CChromiumDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CChromiumDfx::GetInputState()
{
	return 0xffffffff;
}

