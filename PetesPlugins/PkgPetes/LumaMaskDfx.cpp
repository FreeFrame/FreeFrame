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

#include "LumaMask.h"

class CLumaMaskDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CLumaMaskDfx();
	CLumaMaskDfx();
	
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

	SPete_LumaMask_Data m_InstanceData;
	SPete_LumaMask_Settings m_Settings;
	SPete_Parameter* m_pParameters;
	int		m_nParameterCount;

};

BEGIN_INPUTSCREENS_DESC(CLumaMaskDfx)
{{-1,-1}, "RGB 0"},
{{-1,-1}, "RGB 1"},
{{-1,-1}, "RGB Mask"},
END_INPUTSCREENS_DESC(CLumaMaskDfx)

BEGIN_OUTPUTSCREENS_DESC(CLumaMaskDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLumaMaskDfx)

BEGIN_EFFECT_DESC(CLumaMaskDfx, 10204217861273929, PETE_VJO_VERSION , "Petes\\LumaMask", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLumaMaskDfx) 

REGISTER_DFXFACTORY(CLumaMaskDfx)

CLumaMaskDfx::CLumaMaskDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	Pete_LumaMask_GetParameters(&m_pParameters,&m_nParameterCount);

	float* pSettingsStart=(float*)(&m_Settings);

	int nCount;
	for (nCount=0; nCount<m_nParameterCount; nCount+=1) {

		pSettingsStart[nCount]=m_pParameters[nCount].Default;

	}

	m_nMemUsage = sizeof(CLumaMaskDfx);
}

CLumaMaskDfx::~CLumaMaskDfx()
{

	Pete_LumaMask_DeInit(&m_InstanceData);

}

HBITMAP	CLumaMaskDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLumaMaskDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CLumaMaskDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CLumaMaskDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CLumaMaskDfx::UpdateConfig()
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

BOOL	CLumaMaskDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_LumaMask_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CLumaMaskDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMemA = (DWORD*)ppInput[0]->GetBuffer();
	DWORD* pInputMemB = (DWORD*)ppInput[1]->GetBuffer();
	DWORD* pInputMemMask = (DWORD*)ppInput[2]->GetBuffer();

	Pete_LumaMask_Render(&m_InstanceData,&m_Settings,pInputMemA,pInputMemB,pInputMemMask,pOutputMem);

	return TRUE;

}

DWORD	CLumaMaskDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CLumaMaskDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CLumaMaskDfx::GetInputState()
{
	return 0xffffffff;
}

