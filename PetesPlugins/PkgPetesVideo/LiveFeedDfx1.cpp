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

#include "LiveFeed.h"


class CLiveFeed1Dfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CLiveFeed1Dfx();
	CLiveFeed1Dfx();
	
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

	SPete_LiveFeed_Data m_InstanceData;
	SPete_LiveFeed_Settings m_Settings;
	SPete_Parameter* m_pParameters;
	int		m_nParameterCount;

};

_screen_info CLiveFeed1Dfx::m_InputScreensInfo[1];
INT CLiveFeed1Dfx::m_InputScreensCount=0;

BEGIN_OUTPUTSCREENS_DESC(CLiveFeed1Dfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLiveFeed1Dfx)

BEGIN_EFFECT_DESC(CLiveFeed1Dfx, 10203216851273229, PETE_VJO_VERSION , "PetesVideo\\LiveFeed2", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLiveFeed1Dfx) 

REGISTER_DFXFACTORY(CLiveFeed1Dfx)

CLiveFeed1Dfx::CLiveFeed1Dfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	Pete_LiveFeed_GetParameters(&m_pParameters,&m_nParameterCount);

	float* pSettingsStart=(float*)(&m_Settings);

	int nCount;
	for (nCount=0; nCount<m_nParameterCount; nCount+=1) {

		pSettingsStart[nCount]=m_pParameters[nCount].Default;

	}

	m_nMemUsage = sizeof(CLiveFeed1Dfx);
}

CLiveFeed1Dfx::~CLiveFeed1Dfx()
{

	Pete_LiveFeed_DeInit(&m_InstanceData);

}

HBITMAP	CLiveFeed1Dfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLiveFeed1Dfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CLiveFeed1Dfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CLiveFeed1Dfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CLiveFeed1Dfx::UpdateConfig()
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

BOOL	CLiveFeed1Dfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_LiveFeed_Init(&m_InstanceData,m_nXRes,m_nYRes,1);

	return TRUE;
}

BOOL	CLiveFeed1Dfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();

	Pete_LiveFeed_Render(&m_InstanceData,&m_Settings,NULL,pOutputMem);

	return TRUE;

}

DWORD	CLiveFeed1Dfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CLiveFeed1Dfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CLiveFeed1Dfx::GetInputState()
{
	return 0xffffffff;
}
