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
#include "mmintrin.h"

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "RadialBlur.h"
#include "PeteHelpers.h"

class CRadialBlurDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CRadialBlurDfx();
	CRadialBlurDfx();
	
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

	template<int nLayersCountShift> void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	float	m_LayerScale;
	int		m_nLayerScaleID;

	bool	m_bHighestQuality;
	int		m_nHighestQualityID;

	bool	m_bLowestQuality;
	int		m_nLowestQualityID;

	SPete_RadialBlur_Data m_InstanceData;

	DWORD   m_nMemUsage;

};

BEGIN_INPUTSCREENS_DESC(CRadialBlurDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CRadialBlurDfx)

BEGIN_OUTPUTSCREENS_DESC(CRadialBlurDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CRadialBlurDfx)

BEGIN_EFFECT_DESC(CRadialBlurDfx, 10203216861479969, PETE_VJO_VERSION , "Petes\\RadialBlur", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CRadialBlurDfx) 

REGISTER_DFXFACTORY(CRadialBlurDfx)


CRadialBlurDfx::CRadialBlurDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_LayerScale=0.01f;
	m_bHighestQuality=true;
	m_bLowestQuality=false;
	
	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CRadialBlurDfx);
}

CRadialBlurDfx::~CRadialBlurDfx()
{
	Pete_RadialBlur_DeInit(&m_InstanceData);
}

HBITMAP	CRadialBlurDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CRadialBlurDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CRadialBlurDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CRadialBlurDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CRadialBlurDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nLayerScaleID=
		RegisterFloat(m_pEngine,&m_LayerScale,"Scale",0.0f,0.1f);

	m_nHighestQualityID=
		RegisterBool(m_pEngine,&m_bHighestQuality,"Highest Quality");

	m_nLowestQualityID=
		RegisterBool(m_pEngine,&m_bLowestQuality,"Lowest Quality (turn off highest first)");

	return TRUE;
}

BOOL	CRadialBlurDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_RadialBlur_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CRadialBlurDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_RadialBlur_Settings Settings;

	Settings.m_LayerScale=m_LayerScale;

	if (m_bHighestQuality) {

		Settings.m_eQuality=SPete_RadialBlur_Settings::eHighQuality;

	} else if (m_bLowestQuality) {

		Settings.m_eQuality=SPete_RadialBlur_Settings::eLowQuality;

	} else {

		Settings.m_eQuality=SPete_RadialBlur_Settings::eMediumQuality;

	}

	Pete_RadialBlur_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CRadialBlurDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CRadialBlurDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CRadialBlurDfx::GetInputState()
{
	return 0xffffffff;
}

