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

#include "Solarize.h"

class CSolarizeDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CSolarizeDfx();
	CSolarizeDfx();
	
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

	float	m_Threshold;
	int		m_nThresholdID;

	float	m_Start;
	int		m_nStartID;

	float	m_End;
	int		m_nEndID;

	bool	m_bLimit;
	int		m_nLimitID;

	float	m_Floor;
	int		m_nFloorID;

	float	m_Ceiling;
	int		m_nCeilingID;

	int		m_nMemUsage;

	SPete_Solarize_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CSolarizeDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CSolarizeDfx)

BEGIN_OUTPUTSCREENS_DESC(CSolarizeDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CSolarizeDfx)

BEGIN_EFFECT_DESC(CSolarizeDfx, 10203216861479915, PETE_VJO_VERSION , "Petes\\Solarize", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CSolarizeDfx) 

REGISTER_DFXFACTORY(CSolarizeDfx)

CSolarizeDfx::CSolarizeDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Threshold=128.0f;
	m_Start=0.0f;
	m_End=255.0f;
	m_bLimit=false;
	m_Floor=0.0f;
	m_Ceiling=255.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CSolarizeDfx);
}

CSolarizeDfx::~CSolarizeDfx()
{

	Pete_Solarize_DeInit(&m_InstanceData);

}

HBITMAP	CSolarizeDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CSolarizeDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CSolarizeDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CSolarizeDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CSolarizeDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nThresholdID=RegisterFloat(m_pEngine,&m_Threshold,"Threshold",0.0f,255.0f);
	m_nStartID=RegisterFloat(m_pEngine,&m_Start,"Start",0.0f,255.0f);
	m_nEndID=RegisterFloat(m_pEngine,&m_End,"End",0.0f,255.0f);
	m_nLimitID=RegisterBool(m_pEngine,&m_bLimit,"Limit");
	m_nFloorID=RegisterFloat(m_pEngine,&m_Floor,"Floor",0.0f,255.0f);
	m_nCeilingID=RegisterFloat(m_pEngine,&m_Ceiling,"Ceiling",0.0f,255.0f);

	return TRUE;
}

BOOL	CSolarizeDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Solarize_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CSolarizeDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Solarize_Settings Settings;

	Settings.m_Threshold=m_Threshold;
	Settings.m_Start=m_Start;
	Settings.m_End=m_End;
	if (m_bLimit) {
		Settings.m_DoLimit=1.0f;
	} else {
		Settings.m_DoLimit=0.0f;
	}
	Settings.m_Floor=m_Floor;
	Settings.m_Ceiling=m_Ceiling;

	Pete_Solarize_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CSolarizeDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CSolarizeDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CSolarizeDfx::GetInputState()
{
	return 0xffffffff;
}

