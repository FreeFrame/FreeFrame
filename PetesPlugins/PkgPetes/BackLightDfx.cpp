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

#include "BackLight.h"
#include "PeteHelpers.h"

class CBackLightDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CBackLightDfx();
	CBackLightDfx();
	
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

	float	m_SpikeScale;
	int		m_nSpikeScaleID;

	DWORD   m_nMemUsage;

	SPete_BackLight_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CBackLightDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CBackLightDfx)

BEGIN_OUTPUTSCREENS_DESC(CBackLightDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CBackLightDfx)

BEGIN_EFFECT_DESC(CBackLightDfx, 10203216861479960, PETE_VJO_VERSION , "Petes\\BackLight", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CBackLightDfx) 

REGISTER_DFXFACTORY(CBackLightDfx)

CBackLightDfx::CBackLightDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_SpikeScale=127.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CBackLightDfx);
}

CBackLightDfx::~CBackLightDfx()
{

	Pete_BackLight_DeInit(&m_InstanceData);

}

HBITMAP	CBackLightDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CBackLightDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CBackLightDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CBackLightDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CBackLightDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nSpikeScaleID=
		RegisterFloat(m_pEngine,&m_SpikeScale,"Spike Scale",0.0f,255.0f);

	return TRUE;
}

BOOL	CBackLightDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_BackLight_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CBackLightDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_BackLight_Settings Settings;
	Settings.m_SpikeScale=m_SpikeScale;

	Pete_BackLight_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CBackLightDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CBackLightDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CBackLightDfx::GetInputState()
{
	return 0xffffffff;
}

