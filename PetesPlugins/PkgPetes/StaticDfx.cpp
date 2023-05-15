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

#include "Static.h"

class CStaticDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CStaticDfx();
	CStaticDfx();

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

	void	WriteOutputFrame(DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	bool	m_bIsGreyScale;
	int		m_nIsGreyScaleID;

	bool	m_bIsTwoTone;
	int		m_nIsTwoToneID;

	DWORD   m_nMemUsage;

	SPete_Static_Data m_InstanceData;
};

_screen_info CStaticDfx::m_InputScreensInfo[1];
INT CStaticDfx::m_InputScreensCount =0;

BEGIN_OUTPUTSCREENS_DESC(CStaticDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CStaticDfx)

BEGIN_EFFECT_DESC(CStaticDfx, 10203216861479964, PETE_VJO_VERSION , "Petes\\Static", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CStaticDfx)

REGISTER_DFXFACTORY(CStaticDfx)


CStaticDfx::CStaticDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bIsGreyScale=true;
	m_bIsTwoTone=true;

	m_nMemUsage = sizeof(CStaticDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CStaticDfx::~CStaticDfx()
{
	Pete_Static_DeInit(&m_InstanceData);
}

HBITMAP	CStaticDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CStaticDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CStaticDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CStaticDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CStaticDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nIsGreyScaleID=RegisterBool(m_pEngine,&m_bIsGreyScale,"Grey Scale");
	m_nIsTwoToneID=RegisterBool(m_pEngine,&m_bIsTwoTone,"Two Tone");

	return TRUE;
}

BOOL	CStaticDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Static_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CStaticDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();

	SPete_Static_Settings Settings;
	Settings.m_DoGreyScale=(m_bIsGreyScale?100.0f:0.0f);
	Settings.m_DoTwoTone=(m_bIsTwoTone?100.0f:0.0f);

	Pete_Static_Render(&m_InstanceData,&Settings,NULL,pOutputMem);

	return TRUE;

}

DWORD	CStaticDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CStaticDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CStaticDfx::GetInputState()
{
	return 0xffffffff;
}

