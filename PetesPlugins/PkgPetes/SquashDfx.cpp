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

#include "Squash.h"

class CSquashDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CSquashDfx();
	CSquashDfx();
	
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

	bool	m_bEnable;
	int		m_nEnableID;

	bool	m_bIsHorizontal;
	int		m_nIsHorizontalID;

	float	m_Size;
	int		m_nSizeID;

	SPete_Squash_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CSquashDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CSquashDfx)

BEGIN_OUTPUTSCREENS_DESC(CSquashDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CSquashDfx)

BEGIN_EFFECT_DESC(CSquashDfx, 10203216861479972, PETE_VJO_VERSION , "Petes\\Squash", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CSquashDfx) 

REGISTER_DFXFACTORY(CSquashDfx)

CSquashDfx::CSquashDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bEnable=TRUE;
	m_bIsHorizontal=TRUE;
	m_Size=1.0f;

	m_nMemUsage = sizeof(CSquashDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CSquashDfx::~CSquashDfx()
{
	Pete_Squash_DeInit(&m_InstanceData);
}

HBITMAP	CSquashDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CSquashDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CSquashDfx::SetConfigData(CConfigData *p)
{
	ASSERT(p);
	p->SetInt("!", 1);
}


BOOL	CSquashDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CSquashDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nEnableID=RegisterBool(
		m_pEngine,&m_bEnable,"Enable");

	m_nIsHorizontalID=RegisterBool(
		m_pEngine,&m_bIsHorizontal,"Horizontal");

	m_nSizeID=RegisterFloat(
		m_pEngine,&m_Size,"Size",-8.0f,8.0f);

	return TRUE;
}

BOOL	CSquashDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Squash_Init(&m_InstanceData,m_nXRes,m_nYRes);
	
	return TRUE;
}

BOOL	CSquashDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Squash_Settings Settings;
	Settings.m_DoHorizontal=(m_bIsHorizontal?100.0f:0.0f);
	Settings.m_Size=m_Size;

	if (!m_bEnable) {
		Settings.m_Size=1.0f;
	}

	Pete_Squash_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;
}

DWORD	CSquashDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CSquashDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CSquashDfx::GetInputState()
{
	return 0xffffffff;
}

