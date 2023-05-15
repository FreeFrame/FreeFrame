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

#include "LumaOffset.h"

class CLumaOffsetDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CLumaOffsetDfx();
	CLumaOffsetDfx();

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

	DWORD   m_nMemUsage;

	float	m_OffsetScale;
	int		m_nOffsetScaleID;

	float	m_LineGap;
	int		m_nLineGapID;

	SPete_LumaOffset_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CLumaOffsetDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CLumaOffsetDfx)

BEGIN_OUTPUTSCREENS_DESC(CLumaOffsetDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLumaOffsetDfx)

BEGIN_EFFECT_DESC(CLumaOffsetDfx, 10203216861479907, PETE_VJO_VERSION , "Petes\\LumaOffset", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLumaOffsetDfx)

REGISTER_DFXFACTORY(CLumaOffsetDfx)

CLumaOffsetDfx::CLumaOffsetDfx(void)
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_OffsetScale=12.0f;
	m_LineGap=1.0f;

	m_nMemUsage = sizeof(CLumaOffsetDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CLumaOffsetDfx::~CLumaOffsetDfx(void)
{
	Pete_LumaOffset_DeInit(&m_InstanceData);
}

HBITMAP	CLumaOffsetDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLumaOffsetDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;
}

void	CLumaOffsetDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CLumaOffsetDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CLumaOffsetDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nOffsetScaleID=RegisterFloat(
		m_pEngine,&m_OffsetScale,"Offset Scale",-127.0f,127.0f);

	m_nLineGapID=RegisterFloat(
		m_pEngine,&m_LineGap,"Line Gap",0.0f,255.0f);

	return TRUE;
}

BOOL	CLumaOffsetDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_LumaOffset_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CLumaOffsetDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_LumaOffset_Settings Settings;
	Settings.m_OffsetScale=m_OffsetScale;
	Settings.m_LineGap=m_LineGap;

	Pete_LumaOffset_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}


DWORD	CLumaOffsetDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CLumaOffsetDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CLumaOffsetDfx::GetInputState()
{
	return 0xffffffff;
}

