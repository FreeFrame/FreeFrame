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

#include "Posterize.h"

class CPosterizeDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CPosterizeDfx();
	CPosterizeDfx();

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

	float	m_PosterizeLevels;
	int		m_nPosterizeLevelsID;

	int		m_nMemUsage;

	SPete_Posterize_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CPosterizeDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CPosterizeDfx)

BEGIN_OUTPUTSCREENS_DESC(CPosterizeDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CPosterizeDfx)

BEGIN_EFFECT_DESC(CPosterizeDfx, 10203216861479815, PETE_VJO_VERSION , "Petes\\Posterize", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CPosterizeDfx)

REGISTER_DFXFACTORY(CPosterizeDfx)

CPosterizeDfx::CPosterizeDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_PosterizeLevels=8.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CPosterizeDfx);
}

CPosterizeDfx::~CPosterizeDfx()
{

	Pete_Posterize_DeInit(&m_InstanceData);

}

HBITMAP	CPosterizeDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CPosterizeDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CPosterizeDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CPosterizeDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CPosterizeDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nPosterizeLevelsID=RegisterFloat(m_pEngine,&m_PosterizeLevels,"Levels",1.0f,255.0f);

	return TRUE;
}

BOOL	CPosterizeDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Posterize_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CPosterizeDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Posterize_Settings Settings;

	Settings.m_PosterizeLevels=m_PosterizeLevels;

	Pete_Posterize_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CPosterizeDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CPosterizeDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CPosterizeDfx::GetInputState()
{
	return 0xffffffff;
}

