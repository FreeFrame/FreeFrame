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

#include "TVPixel.h"

class CTVPixelDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CTVPixelDfx();
	CTVPixelDfx();
	
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

	int		m_nMemUsage;

	SPete_TVPixel_Data m_InstanceData;
	SPete_TVPixel_Settings m_Settings;
	SPete_Parameter* m_pParameters;
	int		m_nParameterCount;

};

BEGIN_INPUTSCREENS_DESC(CTVPixelDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CTVPixelDfx)

BEGIN_OUTPUTSCREENS_DESC(CTVPixelDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CTVPixelDfx)

BEGIN_EFFECT_DESC(CTVPixelDfx, 10203215862372929, PETE_VJO_VERSION , "Petes\\TVPixel", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CTVPixelDfx) 

REGISTER_DFXFACTORY(CTVPixelDfx)

CTVPixelDfx::CTVPixelDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	Pete_TVPixel_GetParameters(&m_pParameters,&m_nParameterCount);

	float* pSettingsStart=(float*)(&m_Settings);

	int nCount;
	for (nCount=0; nCount<m_nParameterCount; nCount+=1) {

		pSettingsStart[nCount]=m_pParameters[nCount].Default;

	}

	m_nMemUsage = sizeof(CTVPixelDfx);
}

CTVPixelDfx::~CTVPixelDfx()
{

	Pete_TVPixel_DeInit(&m_InstanceData);

}

HBITMAP	CTVPixelDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CTVPixelDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CTVPixelDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CTVPixelDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CTVPixelDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	float* pSettingsStart=(float*)(&m_Settings);

	int nCount;
	for (nCount=0; nCount<m_nParameterCount; nCount+=1) {

		SPete_Parameter* pCurrentParameter=
			(&m_pParameters[nCount]);

		if (pCurrentParameter->Type==PETE_PARAM_BOOL) {

			RegisterBool(m_pEngine,&(pSettingsStart[nCount]),
				pCurrentParameter->pName);

		} else {

			RegisterFloat(m_pEngine,&(pSettingsStart[nCount]),
				pCurrentParameter->pName,
				pCurrentParameter->Minimum,
				pCurrentParameter->Maximum);

		}

	}

	return TRUE;
}

BOOL	CTVPixelDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_TVPixel_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CTVPixelDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	Pete_TVPixel_Render(&m_InstanceData,&m_Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CTVPixelDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CTVPixelDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CTVPixelDfx::GetInputState()
{
	return 0xffffffff;
}

