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

#include "Exclusion.h"
#include "PeteHelpers.h"

class CExclusionDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CExclusionDfx();
	CExclusionDfx();
	
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

	void	SetupTables(void);
	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	SPete_Exclusion_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CExclusionDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CExclusionDfx)

BEGIN_OUTPUTSCREENS_DESC(CExclusionDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CExclusionDfx)

BEGIN_EFFECT_DESC(CExclusionDfx, 10203216861479953, PETE_VJO_VERSION , "Petes\\Exclusion", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CExclusionDfx) 

REGISTER_DFXFACTORY(CExclusionDfx)

CExclusionDfx::CExclusionDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CExclusionDfx);
}

CExclusionDfx::~CExclusionDfx()
{
	Pete_Exclusion_DeInit(&m_InstanceData);
}

HBITMAP	CExclusionDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CExclusionDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CExclusionDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CExclusionDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CExclusionDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CExclusionDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Exclusion_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CExclusionDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Exclusion_Settings Settings;

	Pete_Exclusion_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CExclusionDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CExclusionDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CExclusionDfx::GetInputState()
{
	return 0xffffffff;
}

