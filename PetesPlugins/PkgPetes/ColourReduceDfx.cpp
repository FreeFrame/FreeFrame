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

#include "ColourReduce.h"

class CColourReduceDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CColourReduceDfx();
	CColourReduceDfx();
	
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

	int m_nMemUsage;

	SPete_ColourReduce_Data m_InstanceData;
	SPete_ColourReduce_Settings m_Settings;
	SPete_Parameter* m_pParameters;
	int		m_nParameterCount;

};

BEGIN_INPUTSCREENS_DESC(CColourReduceDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CColourReduceDfx)

BEGIN_OUTPUTSCREENS_DESC(CColourReduceDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CColourReduceDfx)

BEGIN_EFFECT_DESC(CColourReduceDfx, 10203216861479715, PETE_VJO_VERSION , "Petes\\ColourReduce", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CColourReduceDfx) 

REGISTER_DFXFACTORY(CColourReduceDfx)

CColourReduceDfx::CColourReduceDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	Pete_ColourReduce_GetParameters(&m_pParameters,&m_nParameterCount);

	float* pSettingsStart=(float*)(&m_Settings);

	int nCount;
	for (nCount=0; nCount<m_nParameterCount; nCount+=1) {

		pSettingsStart[nCount]=m_pParameters[nCount].Default;

	}

	m_nMemUsage = sizeof(CColourReduceDfx);
}

CColourReduceDfx::~CColourReduceDfx()
{

	Pete_ColourReduce_DeInit(&m_InstanceData);

}

HBITMAP	CColourReduceDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CColourReduceDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CColourReduceDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CColourReduceDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CColourReduceDfx::UpdateConfig()
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

BOOL	CColourReduceDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_ColourReduce_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CColourReduceDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	Pete_ColourReduce_Render(&m_InstanceData,&m_Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CColourReduceDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CColourReduceDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CColourReduceDfx::GetInputState()
{
	return 0xffffffff;
}

