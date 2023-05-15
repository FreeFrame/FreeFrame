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

#include "mmsystem.h"

#include <math.h>

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "RectDistort.h"
#include "PeteHelpers.h"

class CRectDistort: public CDFX
{
	DECLARE_EFFECT()

public:
	~CRectDistort();
	CRectDistort();

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

	DWORD   m_nMemUsage;

	bool	m_bEnableClear;
	int		m_nEnableClearID;

	bool	m_bEnableRectangles;
	int		m_nEnableRectanglesID;

	float	m_RectangleRate;
	int		m_nRectangleRateID;

	float	m_RectangleWidth;
	int		m_nRectangleWidthID;

	float	m_RectangleHeight;
	int		m_nRectangleHeightID;

	float	m_RectangleXDeltaScale;
	int		m_nRectangleXDeltaScaleID;

	float	m_RectangleYDeltaScale;
	int		m_nRectangleYDeltaScaleID;

	float	m_Persistence;
	int		m_nPersistenceID;

	SPete_RectDistort_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CRectDistort)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CRectDistort)

BEGIN_OUTPUTSCREENS_DESC(CRectDistort)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CRectDistort)

BEGIN_EFFECT_DESC(CRectDistort, 10203216861479970, PETE_VJO_VERSION , "Petes\\RectDistort", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CRectDistort)

REGISTER_DFXFACTORY(CRectDistort)


CRectDistort::CRectDistort()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_bEnableClear=false;
	m_bEnableRectangles=true;

	m_RectangleRate=6.0f;
	m_RectangleWidth=1.0f;
	m_RectangleHeight=1.0f;
	m_RectangleXDeltaScale=8.0f;
	m_RectangleYDeltaScale=8.0f;
	m_Persistence=0.95f;

	m_nMemUsage = sizeof(CRectDistort);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));
}

CRectDistort::~CRectDistort()
{
	Pete_RectDistort_DeInit(&m_InstanceData);
}

HBITMAP	CRectDistort::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CRectDistort::SetupDialog()
{
	VJO_MANAGE_STATE();

	return FALSE;
}

void	CRectDistort::SetConfigData(CConfigData *p)
{
	ASSERT(p);
}


BOOL	CRectDistort::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CRectDistort::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nEnableClearID=RegisterBool(
		m_pEngine,&m_bEnableClear,"Clear");

	m_nEnableRectanglesID=RegisterBool(
		m_pEngine,&m_bEnableRectangles,"Enable Rects");

	m_nRectangleRateID=RegisterFloat(
		m_pEngine,&m_RectangleRate,"Rectangle Rate",0.0f,32.0f);

	m_nRectangleWidthID=RegisterFloat(
		m_pEngine,&m_RectangleWidth,"Rectangle Width");

	m_nRectangleHeightID=RegisterFloat(
		m_pEngine,&m_RectangleHeight,"Rectangle Height");

	m_nRectangleXDeltaScaleID=RegisterFloat(
		m_pEngine,&m_RectangleXDeltaScale,"Rectangle X Movement",0.0f,32.0f);

	m_nRectangleYDeltaScaleID=RegisterFloat(
		m_pEngine,&m_RectangleYDeltaScale,"Rectangle Y Movement",0.0f,32.0f);

	m_nPersistenceID=RegisterFloat(
		m_pEngine,&m_Persistence,"Persistence",0.0f,1.0f);

	return TRUE;
}

BOOL	CRectDistort::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_RectDistort_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CRectDistort::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_RectDistort_Settings Settings;
	Settings.m_DoEnableClear=(m_bEnableClear?100.0f:0.0f);
	Settings.m_DoEnableRectangles=(m_bEnableRectangles?100.0f:0.0f);
	Settings.m_RectangleRate=m_RectangleRate;
	Settings.m_RectangleWidth=m_RectangleWidth;
	Settings.m_RectangleHeight=m_RectangleHeight;
	Settings.m_RectangleXDeltaScale=m_RectangleXDeltaScale;
	Settings.m_RectangleYDeltaScale=m_RectangleYDeltaScale;
	Settings.m_Persistence=m_Persistence;

	Pete_RectDistort_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CRectDistort::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CRectDistort::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CRectDistort::GetInputState()
{
	return 0xffffffff;
}

