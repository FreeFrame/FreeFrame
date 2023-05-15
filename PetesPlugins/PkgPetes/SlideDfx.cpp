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

#include "Slide.h"

class CSlideDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CSlideDfx();
	CSlideDfx();

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

	bool	m_bHorizontal;
	int		m_nHorizontalID;

	bool	m_bMirror;
	int		m_nMirrorID;

	bool	m_bMirrorFlip;
	int		m_nMirrorFlipID;

	float	m_Proportion;
	int		m_nProportionID;

	float	m_Speed1;
	int		m_nSpeed1ID;

	float	m_Speed2;
	int		m_nSpeed2ID;

	SPete_Slide_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CSlideDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CSlideDfx)

BEGIN_OUTPUTSCREENS_DESC(CSlideDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CSlideDfx)

BEGIN_EFFECT_DESC(CSlideDfx, 10203216861479959, PETE_VJO_VERSION , "Petes\\Slide", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CSlideDfx)

REGISTER_DFXFACTORY(CSlideDfx)

CSlideDfx::CSlideDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bHorizontal=true;
	m_nHorizontalID=0;

	m_bMirror=false;
	m_nMirrorID=0;

	m_bMirrorFlip=false;
	m_nMirrorFlipID=0;

	m_Proportion=0.3141583f;
	m_nProportionID=0;

	m_Speed1=1.0f;
	m_nSpeed1ID=0;

	m_Speed2=-0.5f;
	m_nSpeed2ID=0;

	m_nMemUsage = sizeof(CSlideDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CSlideDfx::~CSlideDfx()
{
	Pete_Slide_DeInit(&m_InstanceData);
}

HBITMAP	CSlideDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CSlideDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CSlideDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CSlideDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CSlideDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nHorizontalID=RegisterBool(m_pEngine,&m_bHorizontal,"Horizontal, rather than vertical");
	m_nMirrorID=RegisterBool(m_pEngine,&m_bMirror,"Mirror");
	m_nMirrorFlipID=RegisterBool(m_pEngine,&m_bMirrorFlip,"Mirror Flip");
	m_nProportionID=RegisterFloat(m_pEngine,&m_Proportion,"Proportion",0.0f,1.0f);
	m_nSpeed1ID=RegisterFloat(m_pEngine,&m_Speed1,"Speed 1",-16.0f,16.0f);
	m_nSpeed2ID=RegisterFloat(m_pEngine,&m_Speed2,"Speed 2",-16.0f,16.0f);

	return TRUE;
}

BOOL	CSlideDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Slide_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CSlideDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Slide_Settings Settings;
	Settings.m_DoHorizontal=(m_bHorizontal?100.0f:0.0f);
	Settings.m_DoMirror=(m_bMirror?100.0f:0.0f);
	Settings.m_DoMirrorFlip=(m_bMirrorFlip?100.0f:0.0f);
	Settings.m_Proportion=m_Proportion;
	Settings.m_Speed1=m_Speed1;
	Settings.m_Speed2=m_Speed2;

	Pete_Slide_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CSlideDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CSlideDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CSlideDfx::GetInputState()
{
	return 0xffffffff;
}

