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
#include "DilateDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "Dilate.h"

class CDilateDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CDilateDfx();
	CDilateDfx();

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

	BOOL	m_bErode;
	int		m_nErodeID;

	SPete_Dilate_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CDilateDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CDilateDfx)

BEGIN_OUTPUTSCREENS_DESC(CDilateDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CDilateDfx)

BEGIN_EFFECT_DESC(CDilateDfx, 10203216861479950, PETE_VJO_VERSION , "Petes\\Dilate", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CDilateDfx)

REGISTER_DFXFACTORY(CDilateDfx)

CDilateDfx::CDilateDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bErode=FALSE;

	m_nMemUsage = sizeof(CDilateDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CDilateDfx::~CDilateDfx()
{
	Pete_Dilate_DeInit(&m_InstanceData);
}

HBITMAP	CDilateDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CDilateDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CDilateDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
	p->SetInt("E", m_bErode);

}


BOOL	CDilateDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_bErode=p->GetInt("E", m_bErode);

	return TRUE;
}

BOOL	CDilateDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nErodeID=
		RegisterBool(m_pEngine,&m_bErode,"Erode instead of Dilate");

	return TRUE;
}

BOOL	CDilateDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Dilate_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CDilateDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Dilate_Settings Settings;
	Settings.m_DoErode=(m_bErode?100.0f:0.0f);

	Pete_Dilate_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}


DWORD	CDilateDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CDilateDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CDilateDfx::GetInputState()
{
	return 0xffffffff;
}

