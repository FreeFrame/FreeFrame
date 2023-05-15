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

#include "PeteHelpers.h"

#include "BackDropKey.h"

class CBackDropKeyDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CBackDropKeyDfx();
	CBackDropKeyDfx();

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
	void	StoreBackDrop(DWORD* pInput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	bool	m_bCaptureBackDrop;
	int		m_nCaptureBackDropID;

	float	m_Threshold;
	int		m_nThresholdID;

	SPete_BackDropKey_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CBackDropKeyDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CBackDropKeyDfx)

BEGIN_OUTPUTSCREENS_DESC(CBackDropKeyDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CBackDropKeyDfx)

BEGIN_EFFECT_DESC(CBackDropKeyDfx, 10203216861479901, PETE_VJO_VERSION , "Petes\\BackDropKey", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CBackDropKeyDfx)

REGISTER_DFXFACTORY(CBackDropKeyDfx)

CBackDropKeyDfx::CBackDropKeyDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CBackDropKeyDfx);

	m_bCaptureBackDrop=false;
	m_Threshold=48.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CBackDropKeyDfx::~CBackDropKeyDfx()
{

	Pete_BackDropKey_DeInit(&m_InstanceData);

}

HBITMAP	CBackDropKeyDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CBackDropKeyDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return TRUE;

}

void	CBackDropKeyDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CBackDropKeyDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CBackDropKeyDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CBackDropKeyDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	Pete_BackDropKey_Init(&m_InstanceData,nWidth,nHeight);

	m_nCaptureBackDropID=RegisterBool(
		m_pEngine,&m_bCaptureBackDrop,"Capture Backdrop");

	m_nThresholdID=RegisterFloat(
		m_pEngine,&m_Threshold,"Threshold",0.0f,255.0f);

	return TRUE;
}

BOOL	CBackDropKeyDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_BackDropKey_Settings BackDropKeySettings;

	BackDropKeySettings.m_Threshold=m_Threshold;
	if (m_bCaptureBackDrop) {
		BackDropKeySettings.m_UseThisFrameAsBackDrop=100.0f;
	} else {
		BackDropKeySettings.m_UseThisFrameAsBackDrop=0.0f;
	}

	Pete_BackDropKey_Render(&m_InstanceData,&BackDropKeySettings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CBackDropKeyDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CBackDropKeyDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CBackDropKeyDfx::GetInputState()
{
	return 0xffffffff;
}

