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

#include "GaussianBlur.h"

class CGaussianBlurDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CGaussianBlurDfx();
	CGaussianBlurDfx();
	
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

	float	m_Radius;
	int		m_nRadiusID;

	float	m_EdgeFactor;
	int		m_nEdgeFactorID;

	bool	m_bFakeIt;
	int		m_nFakeItID;

	float	m_FakeItPasses;
	int		m_nFakeItPassesID;

	bool	m_bDoUnSharpMask;
	int		m_nDoUnSharpMaskID;

	float	m_AspectRatio;
	int		m_nAspectRatioID;

	float	m_UnSharpMaskAmount;
	int		m_nUnSharpMaskAmountID;

	bool	m_bDoOddButCool;
	int		m_nDoOddButCoolID;
	
	int		m_nMemUsage;

	SPete_GaussianBlur_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CGaussianBlurDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CGaussianBlurDfx)

BEGIN_OUTPUTSCREENS_DESC(CGaussianBlurDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CGaussianBlurDfx)

BEGIN_EFFECT_DESC(CGaussianBlurDfx, 10203216861479919, PETE_VJO_VERSION , "Petes\\GaussianBlur", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CGaussianBlurDfx) 

REGISTER_DFXFACTORY(CGaussianBlurDfx)

CGaussianBlurDfx::CGaussianBlurDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Radius = 5.0f;
	m_EdgeFactor = 0.002f;
	m_bFakeIt = TRUE;
	m_FakeItPasses = 3.0f;
	m_bDoUnSharpMask=FALSE;
	m_AspectRatio=1.0f;
	m_UnSharpMaskAmount=0.5f;
	m_bDoOddButCool=FALSE;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CGaussianBlurDfx);
}

CGaussianBlurDfx::~CGaussianBlurDfx()
{

	Pete_GaussianBlur_DeInit(&m_InstanceData);

}

HBITMAP	CGaussianBlurDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CGaussianBlurDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CGaussianBlurDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CGaussianBlurDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CGaussianBlurDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nRadiusID=RegisterFloat(m_pEngine,&m_Radius,"Radius",1.0f,255.0f);
	m_nEdgeFactorID=RegisterFloat(m_pEngine,&m_EdgeFactor,"Edge Factor",0.0f,1.0f);
	m_nFakeItID=RegisterBool(m_pEngine,&m_bFakeIt,"Fake It");
	m_nFakeItPassesID=RegisterFloat(m_pEngine,&m_FakeItPasses,"Number of passes when faking",1.0f,10.0f);
	m_nDoUnSharpMaskID=RegisterBool(m_pEngine,&m_bDoUnSharpMask,"Unsharp Mask");
	m_nAspectRatioID=RegisterFloat(m_pEngine,&m_AspectRatio,"AspectRatio",0.0f,2.0f);
	m_nUnSharpMaskAmountID=RegisterFloat(m_pEngine,&m_UnSharpMaskAmount,"Unsharp Amount",0.0f,2.0f);
	m_nDoOddButCoolID=RegisterBool(m_pEngine,&m_bDoOddButCool,"Odd Bug");

	return TRUE;
}

BOOL	CGaussianBlurDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_GaussianBlur_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CGaussianBlurDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_GaussianBlur_Settings Settings;

	Settings.m_Radius=m_Radius;
	Settings.m_EdgeFactor=m_EdgeFactor;
	if (m_bFakeIt) {
		Settings.m_DoFakeIt=1.0f;
	} else {
		Settings.m_DoFakeIt=0.0f;
	}
	Settings.m_FakeItPasses=m_FakeItPasses;
	if (m_bDoUnSharpMask) {
		Settings.m_DoUnSharpMask=1.0f;
	} else {
		Settings.m_DoUnSharpMask=0.0f;
	}
	Settings.m_AspectRatio=m_AspectRatio;
	Settings.m_UnSharpMaskAmount=m_UnSharpMaskAmount;
	if (m_bDoOddButCool) {
		Settings.m_DoOddButCool=1.0f;
	} else {
		Settings.m_DoOddButCool=0.0f;
	}

	Pete_GaussianBlur_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CGaussianBlurDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CGaussianBlurDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CGaussianBlurDfx::GetInputState()
{
	return 0xffffffff;
}

