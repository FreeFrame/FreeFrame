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

#include "Kaleidascope.h"

class CKaleidascopeDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CKaleidascopeDfx();
	CKaleidascopeDfx();
	
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

	float	m_Divisions;
	int		m_nDivisionsID;

	float	m_OutputAnglePreIncrement;
	int		m_nOutputAnglePreIncrementID;

	float	m_SourceAnglePreIncrement;
	int		m_nSourceAnglePreIncrementID;

	float	m_SourceCentreX;
	int		m_nSourceCentreXID;
	
	float	m_SourceCentreY;
	int		m_nSourceCentreYID;

	float	m_OutputCentreX;
	int		m_nOutputCentreXID;

	float	m_OutputCentreY;
	int		m_nOutputCentreYID;

	float	m_ReflectionLineProportion;
	int		m_nReflectionLineProportionID;

	float	m_SourceAngleProportion;
	int		m_nSourceAngleProportionID;

	int		m_nMemUsage;

	SPete_Kaleidascope_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CKaleidascopeDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CKaleidascopeDfx)

BEGIN_OUTPUTSCREENS_DESC(CKaleidascopeDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CKaleidascopeDfx)

BEGIN_EFFECT_DESC(CKaleidascopeDfx, 10203216861271919, PETE_VJO_VERSION , "Petes\\Kaleidascope", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CKaleidascopeDfx) 

REGISTER_DFXFACTORY(CKaleidascopeDfx)

CKaleidascopeDfx::CKaleidascopeDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Divisions = 7.0f;
	m_OutputAnglePreIncrement=0.0f;
	m_SourceAnglePreIncrement=0.0f;
	m_SourceCentreX=0.5f;
	m_SourceCentreY=0.5f;
	m_OutputCentreX=0.5f;
	m_OutputCentreY=0.5f;
	m_ReflectionLineProportion=0.5f;
	m_SourceAngleProportion=1.0f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CKaleidascopeDfx);
}

CKaleidascopeDfx::~CKaleidascopeDfx()
{

	Pete_Kaleidascope_DeInit(&m_InstanceData);

}

HBITMAP	CKaleidascopeDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CKaleidascopeDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CKaleidascopeDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CKaleidascopeDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CKaleidascopeDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nDivisionsID=RegisterFloat(m_pEngine,&m_Divisions,"Divisions",0.0f,64.0f);
	m_nOutputAnglePreIncrementID=RegisterFloat(m_pEngine,&m_OutputAnglePreIncrement,"Output Angle Increment",0.0f,360.0f);
	m_nSourceAnglePreIncrementID=RegisterFloat(m_pEngine,&m_SourceAnglePreIncrement,"Source Angle Increment",0.0f,360.0f);
	m_nSourceCentreXID=RegisterFloat(m_pEngine,&m_SourceCentreX,"Source Centre X",0.0f,1.0f);
	m_nSourceCentreYID=RegisterFloat(m_pEngine,&m_SourceCentreY,"Source Centre Y",0.0f,1.0f);
	m_nOutputCentreXID=RegisterFloat(m_pEngine,&m_OutputCentreX,"Output Centre X",0.0f,1.0f);
	m_nOutputCentreYID=RegisterFloat(m_pEngine,&m_OutputCentreY,"Output Centre Y",0.0f,1.0f);
	m_nReflectionLineProportionID=RegisterFloat(m_pEngine,&m_ReflectionLineProportion,"Reflection Line Proportion",0.0f,1.0f);
	m_nSourceAngleProportionID=RegisterFloat(m_pEngine,&m_SourceAngleProportion,"Source Angle Proportion",0.1f,10.0f);

	return TRUE;
}

BOOL	CKaleidascopeDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Kaleidascope_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CKaleidascopeDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Kaleidascope_Settings Settings;

	Settings.m_Divisions=m_Divisions;
	Settings.m_OutputAnglePreIncrement=(m_OutputAnglePreIncrement/360.0f)*Pete_TwoPi;
	Settings.m_SourceAnglePreIncrement=(m_SourceAnglePreIncrement/360.0f)*Pete_TwoPi;
	Settings.m_SourceCentreX=m_SourceCentreX;
	Settings.m_SourceCentreY=m_SourceCentreY;
	Settings.m_OutputCentreX=m_OutputCentreX;
	Settings.m_OutputCentreY=m_OutputCentreY;
	Settings.m_ReflectionLineProportion=m_ReflectionLineProportion;
	Settings.m_SourceAngleProportion=m_SourceAngleProportion;

	Pete_Kaleidascope_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CKaleidascopeDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CKaleidascopeDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CKaleidascopeDfx::GetInputState()
{
	return 0xffffffff;
}

