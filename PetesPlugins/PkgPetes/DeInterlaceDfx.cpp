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

#include "DeInterlace.h"

class CDeInterlaceDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CDeInterlaceDfx();
	CDeInterlaceDfx();
	
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

	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	BOOL	m_bPalStretchFix;
	int		m_nPalStretchFixID;

	SPete_DeInterlace_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CDeInterlaceDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CDeInterlaceDfx)

BEGIN_OUTPUTSCREENS_DESC(CDeInterlaceDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CDeInterlaceDfx)

BEGIN_EFFECT_DESC(CDeInterlaceDfx, 10203216861479923, PETE_VJO_VERSION , "Petes\\DeInterlace", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CDeInterlaceDfx) 

REGISTER_DFXFACTORY(CDeInterlaceDfx)

CDeInterlaceDfx::CDeInterlaceDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CDeInterlaceDfx);

	m_bPalStretchFix=false;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CDeInterlaceDfx::~CDeInterlaceDfx()
{
	Pete_DeInterlace_DeInit(&m_InstanceData);
}

HBITMAP	CDeInterlaceDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CDeInterlaceDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CDeInterlaceDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CDeInterlaceDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CDeInterlaceDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nPalStretchFixID=
		RegisterBool(m_pEngine,&m_bPalStretchFix,"PAL Stretch Fix");
	
	return TRUE;
}

BOOL	CDeInterlaceDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_DeInterlace_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CDeInterlaceDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_DeInterlace_Settings Settings;
	Settings.m_DoPalStretchFix=(m_bPalStretchFix?100.0f:0.0f);

	Pete_DeInterlace_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CDeInterlaceDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CDeInterlaceDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CDeInterlaceDfx::GetInputState()
{
	return 0xffffffff;
}

