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

#include "Vectorize.h"

class CVectorizeDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CVectorizeDfx();
	CVectorizeDfx();

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

	float	m_PreFilterSize;
	int		m_nPreFilterSizeID;

	float	m_Resolution;
	int		m_nResolutionID;

	bool	m_bMonochrome;
	int		m_nMonochromeID;

	float	m_ColourReduceColours;
	int		m_nColourReduceColoursID;

	float	m_ColourReducePalettePersistence;
	int		m_nColourReducePalettePersistenceID;

	float	m_DifferenceThreshold;
	int		m_nDifferenceThresholdID;

	int		m_nMemUsage;

	SPete_Vectorize_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CVectorizeDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CVectorizeDfx)

BEGIN_OUTPUTSCREENS_DESC(CVectorizeDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CVectorizeDfx)

BEGIN_EFFECT_DESC(CVectorizeDfx, 10203216861471976, PETE_VJO_VERSION , "Petes\\Vectorize", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CVectorizeDfx)

REGISTER_DFXFACTORY(CVectorizeDfx)

CVectorizeDfx::CVectorizeDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_PreFilterSize = 5.0f;
	m_Resolution = 8.0f;
	m_bMonochrome = false;
	m_ColourReduceColours = 4.0f;
	m_ColourReducePalettePersistence = 0.99f;
	m_DifferenceThreshold=0.1f;

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	m_nMemUsage = sizeof(CVectorizeDfx);
}

CVectorizeDfx::~CVectorizeDfx()
{

	Pete_Vectorize_DeInit(&m_InstanceData);

}

HBITMAP	CVectorizeDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CVectorizeDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS);

	return FALSE;

}

void	CVectorizeDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CVectorizeDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CVectorizeDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nPreFilterSizeID=RegisterFloat(m_pEngine,&m_PreFilterSize,"Pre-Filter Size",1.0f,255.0f);
	m_nResolutionID=RegisterFloat(m_pEngine,&m_Resolution,"Resolution",1.0f,255.0f);
	m_nMonochromeID=RegisterBool(m_pEngine,&m_bMonochrome,"Monochrome");
	m_nColourReduceColoursID=RegisterFloat(m_pEngine,&m_ColourReduceColours,"Colour Count",1.0f,255.0f);
	m_nColourReducePalettePersistenceID=RegisterFloat(m_pEngine,&m_ColourReducePalettePersistence,"Palette Persistence",0.0f,1.0f);
	m_nDifferenceThresholdID=RegisterFloat(m_pEngine,&m_DifferenceThreshold,"Difference Threshold",0.0f,0.2f);

	return TRUE;
}

BOOL	CVectorizeDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Vectorize_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CVectorizeDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Vectorize_Settings Settings;

	Settings.m_PreFilterSize=m_PreFilterSize;
	Settings.m_CellCount=m_Resolution;
	if (m_bMonochrome) {
		Settings.m_Monochrome=1.0f;
	} else {
		Settings.m_Monochrome=0.0f;
	}
	Settings.m_ColourReduceLevels=m_ColourReduceColours;
	Settings.m_ColourReducePalettePersistence=m_ColourReducePalettePersistence;
	Settings.m_DifferenceThreshold=m_DifferenceThreshold;

	Pete_Vectorize_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}

DWORD	CVectorizeDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CVectorizeDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CVectorizeDfx::GetInputState()
{
	return 0xffffffff;
}

