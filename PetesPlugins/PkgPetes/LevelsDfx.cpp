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
#include "LevelsDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "Levels.h"

class CLevelsDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CLevelsDfx();
	CLevelsDfx();
	
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

	void	SetupVariables(void);
	void	UnregisterUsedVariables(void);

	void	SetupTables(void);
	void	WriteOutputFrame(DWORD* pInput,DWORD* pOutput);

	void	CalculateAutoLevels(DWORD* pSource);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;


	ELevelsStyle m_eStyle;

	bool	m_bAllowInversion;
	int		m_nAllowInversionID;

	float	m_UniformInputFloor;
	int		m_nUniformInputFloorID;
	
	float	m_UniformInputCeiling;
	int		m_nUniformInputCeilingID;

	float	m_UniformOutputFloor;
	int		m_nUniformOutputFloorID;
	
	float	m_UniformOutputCeiling;
	int		m_nUniformOutputCeilingID;


	float	m_RedInputFloor;
	int		m_nRedInputFloorID;
	
	float	m_RedInputCeiling;
	int		m_nRedInputCeilingID;

	float	m_RedOutputFloor;
	int		m_nRedOutputFloorID;
	
	float	m_RedOutputCeiling;
	int		m_nRedOutputCeilingID;


	float	m_GreenInputFloor;
	int		m_nGreenInputFloorID;
	
	float	m_GreenInputCeiling;
	int		m_nGreenInputCeilingID;

	float	m_GreenOutputFloor;
	int		m_nGreenOutputFloorID;
	
	float	m_GreenOutputCeiling;
	int		m_nGreenOutputCeilingID;


	float	m_BlueInputFloor;
	int		m_nBlueInputFloorID;
	
	float	m_BlueInputCeiling;
	int		m_nBlueInputCeilingID;

	float	m_BlueOutputFloor;
	int		m_nBlueOutputFloorID;
	
	float	m_BlueOutputCeiling;
	int		m_nBlueOutputCeilingID;

	float	m_LowPercentile;
	int		m_nLowPercentileID;

	float	m_HighPercentile;
	int		m_nHighPercentileID;

	SPete_Levels_Data m_InstanceData;

};

BEGIN_INPUTSCREENS_DESC(CLevelsDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CLevelsDfx)

BEGIN_OUTPUTSCREENS_DESC(CLevelsDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLevelsDfx)

BEGIN_EFFECT_DESC(CLevelsDfx, 10203216861479955, PETE_VJO_VERSION , "Petes\\Levels", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLevelsDfx) 

REGISTER_DFXFACTORY(CLevelsDfx)

CLevelsDfx::CLevelsDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;
	
	m_eStyle = eUniformStyle;

	m_bAllowInversion=true;

	m_UniformInputFloor=0.0f;
	m_nUniformInputFloorID=0;
	
	m_UniformInputCeiling=255.0f;
	m_nUniformInputCeilingID=0;

	m_UniformOutputFloor=0.0f;
	m_nUniformOutputFloorID=0;
	
	m_UniformOutputCeiling=255.0f;
	m_nUniformOutputCeilingID=0;


	m_RedInputFloor=0.0f;
	m_nRedInputFloorID=0;
	
	m_RedInputCeiling=255.0f;
	m_nRedInputCeilingID=0;

	m_RedOutputFloor=0.0f;
	m_nRedOutputFloorID=0;
	
	m_RedOutputCeiling=255.0f;
	m_nRedOutputCeilingID=0;


	m_GreenInputFloor=0.0f;
	m_nGreenInputFloorID=0;
	
	m_GreenInputCeiling=255.0f;
	m_nGreenInputCeilingID=0;

	m_GreenOutputFloor=0.0f;
	m_nGreenOutputFloorID=0;
	
	m_GreenOutputCeiling=255.0f;
	m_nGreenOutputCeilingID=0;


	m_BlueInputFloor=0.0f;
	m_nBlueInputFloorID=0;
	
	m_BlueInputCeiling=255.0f;
	m_nBlueInputCeilingID=0;

	m_BlueOutputFloor=0.0f;
	m_nBlueOutputFloorID=0;
	
	m_BlueOutputCeiling=255.0f;
	m_nBlueOutputCeilingID=0;

	m_LowPercentile=5.0f;
	m_HighPercentile=95.0f;

	m_nMemUsage = sizeof(CLevelsDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

}

CLevelsDfx::~CLevelsDfx()
{
	Pete_Levels_DeInit(&m_InstanceData);
}

HBITMAP	CLevelsDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLevelsDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CLevelsDfxDlg	dlg;

	dlg.m_eStyle = m_eStyle;
	dlg.m_bAllowInversion = m_bAllowInversion;

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_eStyle = dlg.m_eStyle;
	m_bAllowInversion = dlg.m_bAllowInversion;

	if (dlg.m_bStyleChanged) {
		UnregisterUsedVariables();
	}

	return TRUE;

}

void	CLevelsDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
	p->SetInt("S", m_eStyle);

}


BOOL	CLevelsDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_eStyle = static_cast<ELevelsStyle>(p->GetInt("S", m_eStyle));
	
	return TRUE;	
}

BOOL	CLevelsDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	SetupVariables();

	return TRUE;
}

BOOL	CLevelsDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_Levels_Init(&m_InstanceData,m_nXRes,m_nYRes);

	return TRUE;
}

BOOL	CLevelsDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_Levels_Settings Settings;

	if ((m_eStyle==eAutoUniformStyle)||(m_eStyle==eAutoSeperateRGBStyle)) {
		Settings.m_DoAuto=100.0f;
	} else {
		Settings.m_DoAuto=0.0f;
	}
	if ((m_eStyle==eUniformStyle)||(m_eStyle==eAutoUniformStyle)) {
		Settings.m_DoUniform=100.0f;
	} else {
		Settings.m_DoUniform=0.0f;
	}
	Settings.m_DoAllowInversion=(m_bAllowInversion?100.0f:0.0f);

	Settings.m_UniformInputFloor=m_UniformInputFloor;
	Settings.m_UniformInputCeiling=m_UniformInputCeiling;
	Settings.m_UniformOutputFloor=m_UniformOutputFloor;
	Settings.m_UniformOutputCeiling=m_UniformOutputCeiling;

	Settings.m_RedInputFloor=m_RedInputFloor;
	Settings.m_RedInputCeiling=m_RedInputCeiling;
	Settings.m_RedOutputFloor=m_RedOutputFloor;
	Settings.m_RedOutputCeiling=m_RedOutputCeiling;

	Settings.m_GreenInputFloor=m_GreenInputFloor;
	Settings.m_GreenInputCeiling=m_GreenInputCeiling;
	Settings.m_GreenOutputFloor=m_GreenOutputFloor;
	Settings.m_GreenOutputCeiling=m_GreenOutputCeiling;

	Settings.m_BlueInputFloor=m_BlueInputFloor;
	Settings.m_BlueInputCeiling=m_BlueInputCeiling;
	Settings.m_BlueOutputFloor=m_BlueOutputFloor;
	Settings.m_BlueOutputCeiling=m_BlueOutputCeiling;

	Settings.m_LowPercentile=m_LowPercentile;
	Settings.m_HighPercentile=m_HighPercentile;

	Pete_Levels_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;

}


DWORD	CLevelsDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CLevelsDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CLevelsDfx::GetInputState()
{
	return 0xffffffff;
}

void	CLevelsDfx::SetupVariables(void) {

	switch (m_eStyle) {

		case eUniformStyle: {

			m_nAllowInversionID=
				RegisterBool(m_pEngine,&m_bAllowInversion,"Allow Inversion");

			m_nUniformInputFloorID=
				RegisterFloat(m_pEngine,&m_UniformInputFloor,"Input-Floor",0.0f,255.0f,1.0f);
			m_nUniformInputCeilingID=
				RegisterFloat(m_pEngine,&m_UniformInputCeiling,"Input-Ceiling",0.0f,255.0f,1.0f);
	
			m_nUniformOutputFloorID=
				RegisterFloat(m_pEngine,&m_UniformOutputFloor,"Output-Floor",0.0f,255.0f,1.0f);
			m_nUniformOutputCeilingID=
				RegisterFloat(m_pEngine,&m_UniformOutputCeiling,"Output-Ceiling",0.0f,255.0f,1.0f);
							
		}break;

		case eSeperateRGBStyle: {

			m_nAllowInversionID=
				RegisterBool(m_pEngine,&m_bAllowInversion,"Allow Inversion");

			m_nRedInputFloorID=
				RegisterFloat(m_pEngine,&m_RedInputFloor,"Red-Input-Floor",0.0f,255.0f,1.0f);
			m_nRedInputCeilingID=
				RegisterFloat(m_pEngine,&m_RedInputCeiling,"Red-Input-Ceiling",0.0f,255.0f,1.0f);
	
			m_nRedOutputFloorID=
				RegisterFloat(m_pEngine,&m_RedOutputFloor,"Red-Output-Floor",0.0f,255.0f,1.0f);
			m_nRedOutputCeilingID=
				RegisterFloat(m_pEngine,&m_RedOutputCeiling,"Red-Output-Ceiling",0.0f,255.0f,1.0f);


			m_nGreenInputFloorID=
				RegisterFloat(m_pEngine,&m_GreenInputFloor,"Green-Input-Floor",0.0f,255.0f,1.0f);
			m_nGreenInputCeilingID=
				RegisterFloat(m_pEngine,&m_GreenInputCeiling,"Green-Input-Ceiling",0.0f,255.0f,1.0f);
	
			m_nGreenOutputFloorID=
				RegisterFloat(m_pEngine,&m_GreenOutputFloor,"Green-Output-Floor",0.0f,255.0f,1.0f);
			m_nGreenOutputCeilingID=
				RegisterFloat(m_pEngine,&m_GreenOutputCeiling,"Green-Output-Ceiling",0.0f,255.0f,1.0f);


			m_nBlueInputFloorID=
				RegisterFloat(m_pEngine,&m_BlueInputFloor,"Blue-Input-Floor",0.0f,255.0f,1.0f);
			m_nBlueInputCeilingID=
				RegisterFloat(m_pEngine,&m_BlueInputCeiling,"Blue-Input-Ceiling",0.0f,255.0f,1.0f);
	
			m_nBlueOutputFloorID=
				RegisterFloat(m_pEngine,&m_BlueOutputFloor,"Blue-Output-Floor",0.0f,255.0f,1.0f);
			m_nBlueOutputCeilingID=
				RegisterFloat(m_pEngine,&m_BlueOutputCeiling,"Blue-Output-Ceiling",0.0f,255.0f,1.0f);


		}break;

		case eAutoUniformStyle:
		case eAutoSeperateRGBStyle: {

			m_nLowPercentileID=
				RegisterFloat(m_pEngine,&m_LowPercentile,"Floor Percentage",0.0f,100.0f);
			m_nHighPercentileID=
				RegisterFloat(m_pEngine,&m_HighPercentile,"Ceiling Percentage",0.0f,100.0f);

		}break;

		default: {
			ASSERT(FALSE);
		}

	}
	

}

void	CLevelsDfx::UnregisterUsedVariables(void) {

	if (m_nAllowInversionID!=0) {
		m_pEngine->UnregisterVariable(m_nAllowInversionID);
		m_nAllowInversionID=0;
	}

	if (m_nUniformInputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nUniformInputFloorID);
		m_nUniformInputFloorID=0;
	}
	
	if (m_nUniformInputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nUniformInputCeilingID);
		m_nUniformInputCeilingID=0;
	}

	if (m_nUniformOutputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nUniformOutputFloorID);
		m_nUniformOutputFloorID=0;
	}

	if (m_nUniformOutputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nUniformOutputCeilingID);
		m_nUniformOutputCeilingID=0;
	}


	if (m_nRedInputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nRedInputFloorID);
		m_nRedInputFloorID=0;
	}
	
	if (m_nRedInputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nRedInputCeilingID);
		m_nRedInputCeilingID=0;
	}

	if (m_nRedOutputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nRedOutputFloorID);
		m_nRedOutputFloorID=0;
	}

	if (m_nRedOutputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nRedOutputCeilingID);
		m_nRedOutputCeilingID=0;
	}


	if (m_nGreenInputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nGreenInputFloorID);
		m_nGreenInputFloorID=0;
	}
	
	if (m_nGreenInputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nGreenInputCeilingID);
		m_nGreenInputCeilingID=0;
	}

	if (m_nGreenOutputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nGreenOutputFloorID);
		m_nGreenOutputFloorID=0;
	}

	if (m_nGreenOutputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nGreenOutputCeilingID);
		m_nGreenOutputCeilingID=0;
	}


	if (m_nBlueInputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nBlueInputFloorID);
		m_nBlueInputFloorID=0;
	}
	
	if (m_nBlueInputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nBlueInputCeilingID);
		m_nBlueInputCeilingID=0;
	}

	if (m_nBlueOutputFloorID!=0) {
		m_pEngine->UnregisterVariable(m_nBlueOutputFloorID);
		m_nBlueOutputFloorID=0;
	}

	if (m_nBlueOutputCeilingID!=0) {
		m_pEngine->UnregisterVariable(m_nBlueOutputCeilingID);
		m_nBlueOutputCeilingID=0;
	}

	if (m_nLowPercentileID!=0) {
		m_pEngine->UnregisterVariable(m_nLowPercentileID);
		m_nLowPercentileID=0;
	}

	if (m_nHighPercentileID!=0) {
		m_pEngine->UnregisterVariable(m_nHighPercentileID);
		m_nHighPercentileID=0;
	}

}