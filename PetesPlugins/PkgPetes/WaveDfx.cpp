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

#include "Wave.h"
#include "PeteHelpers.h"

class CWaveDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CWaveDfx();
	CWaveDfx();
	
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

	void	UpdateWaveData(DWORD* pSource);

	
	void	SetupTables(void);
	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	float	m_Friction;
	int		m_nFrictionID;

	float	m_Speed;
	int		m_nSpeedID;

	SPete_Wave_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CWaveDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CWaveDfx)

BEGIN_OUTPUTSCREENS_DESC(CWaveDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CWaveDfx)

BEGIN_EFFECT_DESC(CWaveDfx, 10203216861479932, PETE_VJO_VERSION , "Petes\\Wave", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CWaveDfx) 

REGISTER_DFXFACTORY(CWaveDfx)

CWaveDfx::CWaveDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Friction = 0.95f;
	m_Speed = 0.2f;

	m_nMemUsage = sizeof(CWaveDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CWaveDfx::~CWaveDfx()
{
	Pete_Wave_DeInit(&m_InstanceData);
}

HBITMAP	CWaveDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CWaveDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;
}

void	CWaveDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
}


BOOL	CWaveDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CWaveDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nFrictionID=RegisterFloat(m_pEngine,&m_Friction,"Friction",0.0,1.0f);
	m_nSpeedID=RegisterFloat(m_pEngine,&m_Speed,"Speed",0.0,1.0f);

	return TRUE;
}

BOOL	CWaveDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Wave_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CWaveDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Wave_Settings Settings;
	Settings.m_Friction=m_Friction;
	Settings.m_Speed=m_Speed;

	Pete_Wave_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}


DWORD	CWaveDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CWaveDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CWaveDfx::GetInputState()
{
	return 0xffffffff;
}

