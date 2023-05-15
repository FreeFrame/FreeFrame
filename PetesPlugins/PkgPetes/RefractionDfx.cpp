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

#include "Refraction.h"

class CRefractionDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CRefractionDfx();
	CRefractionDfx();

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

	float	m_Refraction;
	int		m_nRefractionID;

	float	m_CellWidth;
	int		m_nCellWidthID;

	float	m_CellHeight;
	int		m_nCellHeightID;

	bool	m_bAllowMagnification;
	int		m_nAllowMagnificationID;

	DWORD   m_nMemUsage;

	SPete_Refraction_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CRefractionDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CRefractionDfx)

BEGIN_OUTPUTSCREENS_DESC(CRefractionDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CRefractionDfx)

BEGIN_EFFECT_DESC(CRefractionDfx, 10203216861479958, PETE_VJO_VERSION , "Petes\\Refraction", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CRefractionDfx)

REGISTER_DFXFACTORY(CRefractionDfx)

CRefractionDfx::CRefractionDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Refraction=2.0f;
	m_nRefractionID=0;

	m_CellWidth=16.0f;
	m_nCellWidthID=0;

	m_CellHeight=16.0f;
	m_nCellHeightID=0;

	m_bAllowMagnification=true;
	m_nAllowMagnificationID=0;

	m_nMemUsage = sizeof(CRefractionDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CRefractionDfx::~CRefractionDfx()
{
	Pete_Refraction_DeInit(&m_InstanceData);
}

HBITMAP	CRefractionDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CRefractionDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	return FALSE;
}

void	CRefractionDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CRefractionDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CRefractionDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nRefractionID=RegisterFloat(m_pEngine,&m_Refraction,"Refraction",0.0f,32.0f);
	m_nCellWidthID=RegisterFloat(m_pEngine,&m_CellWidth,"Cell Width",0.0f,255.0f);
	m_nCellHeightID=RegisterFloat(m_pEngine,&m_CellHeight,"Cell Height",0.0f,255.0f);
	m_nAllowMagnificationID=
		RegisterBool(m_pEngine,&m_bAllowMagnification,"Allow Magnification");

	return TRUE;
}

BOOL	CRefractionDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Refraction_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CRefractionDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Refraction_Settings Settings;
	Settings.m_Refraction=m_Refraction;
	Settings.m_CellWidth=m_CellWidth;
	Settings.m_CellHeight=m_CellHeight;
	Settings.m_DoAllowMagnification=(m_bAllowMagnification?100.0f:0.0f);

	Pete_Refraction_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CRefractionDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CRefractionDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CRefractionDfx::GetInputState()
{
	return 0xffffffff;
}
