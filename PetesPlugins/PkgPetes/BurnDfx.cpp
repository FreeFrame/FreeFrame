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

#include "Burn.h"
#include "PeteHelpers.h"

class CBurnDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CBurnDfx();
	CBurnDfx();
	
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

	SPete_Burn_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CBurnDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CBurnDfx)

BEGIN_OUTPUTSCREENS_DESC(CBurnDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CBurnDfx)

BEGIN_EFFECT_DESC(CBurnDfx, 10203216861479951, PETE_VJO_VERSION , "Petes\\Burn", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CBurnDfx) 

REGISTER_DFXFACTORY(CBurnDfx)

CBurnDfx::CBurnDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CBurnDfx);
}

CBurnDfx::~CBurnDfx()
{
	Pete_Burn_DeInit(&m_InstanceData);
}

HBITMAP	CBurnDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CBurnDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CBurnDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CBurnDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CBurnDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CBurnDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Burn_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CBurnDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Burn_Settings Settings;

	Pete_Burn_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CBurnDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CBurnDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CBurnDfx::GetInputState()
{
	return 0xffffffff;
}

