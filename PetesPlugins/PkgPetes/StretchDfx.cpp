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

#include "Stretch.h"

class CStretchDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CStretchDfx();
	CStretchDfx();
	
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

	void	GetUVAtPos(float inX,float inY,float* poutU,float* poutV);
	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	bool	m_bMirror;
	int		m_nMirrorID;

	float	m_TopLeftX;
	int		m_nTopLeftXID;
	float	m_TopLeftY;
	int		m_nTopLeftYID;

	float	m_TopRightX;
	int		m_nTopRightXID;
	float	m_TopRightY;
	int		m_nTopRightYID;

	float	m_BottomRightX;
	int		m_nBottomRightXID;
	float	m_BottomRightY;
	int		m_nBottomRightYID;

	float	m_BottomLeftX;
	int		m_nBottomLeftXID;
	float	m_BottomLeftY;
	int		m_nBottomLeftYID;

	DWORD   m_nMemUsage;

	SPete_Stretch_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CStretchDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CStretchDfx)

BEGIN_OUTPUTSCREENS_DESC(CStretchDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CStretchDfx)

BEGIN_EFFECT_DESC(CStretchDfx, 10203216861479961, PETE_VJO_VERSION , "Petes\\Stretch", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CStretchDfx) 

REGISTER_DFXFACTORY(CStretchDfx)


CStretchDfx::CStretchDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bMirror=false;

	m_TopLeftX=0.0f;
	m_TopLeftY=0.0f;

	m_TopRightX=1.0f;
	m_TopRightY=0.0f;

	m_BottomRightX=1.0f;
	m_BottomRightY=1.0f;

	m_BottomLeftX=0.0f;
	m_BottomLeftY=1.0f;

	m_nMemUsage = sizeof(CStretchDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CStretchDfx::~CStretchDfx()
{
	Pete_Stretch_DeInit(&m_InstanceData);
}

HBITMAP	CStretchDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CStretchDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;
}

void	CStretchDfx::SetConfigData(CConfigData *p)
{
	ASSERT(p);
	p->SetInt("!", 1);
}


BOOL	CStretchDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CStretchDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nMirrorID=
		RegisterBool(m_pEngine,&m_bMirror,"Mirror instead of tiling");

	// Pete- Confusing descriptions because I messed up the direction of
	// the Y-axis, this gives the correct descriptions without altering
	// any code. Lazy, me?
	m_nBottomRightXID=
		RegisterFloat(m_pEngine,&m_BottomRightX,"Top Right X",-2.0f,2.0f);
	m_nBottomRightYID=
		RegisterFloat(m_pEngine,&m_BottomRightY,"Top Right Y",-2.0f,2.0f);

	m_nBottomLeftXID=
		RegisterFloat(m_pEngine,&m_BottomLeftX,"Top Left X",-2.0f,2.0f);
	m_nBottomLeftYID=
		RegisterFloat(m_pEngine,&m_BottomLeftY,"Top Left Y",-2.0f,2.0f);
	
	m_nTopLeftXID=
		RegisterFloat(m_pEngine,&m_TopLeftX,"Bottom Left X",-2.0f,2.0f);
	m_nTopLeftYID=
		RegisterFloat(m_pEngine,&m_TopLeftY,"Bottom Left Y",-2.0f,2.0f);

	m_nTopRightXID=
		RegisterFloat(m_pEngine,&m_TopRightX,"Bottom Right X",-2.0f,2.0f);
	m_nTopRightYID=
		RegisterFloat(m_pEngine,&m_TopRightY,"Bottom Right Y",-2.0f,2.0f);

	return TRUE;
}

BOOL	CStretchDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Stretch_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CStretchDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Stretch_Settings Settings;
	Settings.m_DoMirror=(m_bMirror?100.0f:0.0f);

	Settings.m_TopLeftX=m_TopLeftX;
	Settings.m_TopLeftY=m_TopLeftY;

	Settings.m_TopRightX=m_TopRightX;
	Settings.m_TopRightY=m_TopRightY;

	Settings.m_BottomLeftX=m_BottomLeftX;
	Settings.m_BottomLeftY=m_BottomLeftY;

	Settings.m_BottomRightX=m_BottomRightX;
	Settings.m_BottomRightY=m_BottomRightY;

	Pete_Stretch_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CStretchDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CStretchDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CStretchDfx::GetInputState()
{
	return 0xffffffff;
}

