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
#include "PShopFiltersDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "Burn.h"
#include "Chromium.h"
#include "Dodge.h"
#include "Exclusion.h"
#include "Posterize.h"
#include "Solarize.h"

class CPShopFiltersDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CPShopFiltersDfx();
	CPShopFiltersDfx();

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

	void	SetupStoredFramesBuffer(int nWidth, int nHeight);
	void	SetupPlayback(void);
	void	UnRegisterAllVariables(void);

	void	StoreInputFrame(DWORD* pInput);
	void	WriteOutputFrame(DWORD* pOutput);
	void	Update(void);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	EPShopFiltersStyle m_eStyle;
	EPShopFiltersStyle m_ePreviousStyle;

	SPete_Burn_Data			m_BurnInstanceData;
	SPete_Chromium_Data		m_ChromiumInstanceData;
	SPete_Dodge_Data		m_DodgeInstanceData;
	SPete_Exclusion_Data	m_ExclusionInstanceData;
	SPete_Posterize_Data	m_PosterizeInstanceData;
	SPete_Solarize_Data		m_SolarizeInstanceData;

	float	m_ChromiumEdge1;
	int		m_nChromiumEdge1ID;
	float	m_ChromiumEdge2;
	int		m_nChromiumEdge2ID;
	float	m_ChromiumRedTint;
	int		m_nChromiumRedTintID;
	float	m_ChromiumGreenTint;
	int		m_nChromiumGreenTintID;
	float	m_ChromiumBlueTint;
	int		m_nChromiumBlueTintID;

	float	m_PosterizeLevels;
	int		m_nPosterizeLevelsID;

	float	m_SolarizeThreshold;
	int		m_nSolarizeThresholdID;
	float	m_SolarizeStart;
	int		m_nSolarizeStartID;
	float	m_SolarizeEnd;
	int		m_nSolarizeEndID;
	bool	m_bSolarizeLimit;
	int		m_nSolarizeLimitID;
	float	m_SolarizeFloor;
	int		m_nSolarizeFloorID;
	float	m_SolarizeCeiling;
	int		m_nSolarizeCeilingID;

};

BEGIN_INPUTSCREENS_DESC(CPShopFiltersDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CPShopFiltersDfx)

BEGIN_OUTPUTSCREENS_DESC(CPShopFiltersDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CPShopFiltersDfx)

BEGIN_EFFECT_DESC(CPShopFiltersDfx, 10203216861459948, PETE_VJO_VERSION , "Petes\\PShopFilters", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CPShopFiltersDfx)

REGISTER_DFXFACTORY(CPShopFiltersDfx)

CPShopFiltersDfx::CPShopFiltersDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_eStyle = eBurnStyle;
	m_ePreviousStyle = eBurnStyle;

	m_nMemUsage = sizeof(CPShopFiltersDfx);

	ZeroMemory(&m_BurnInstanceData,sizeof(m_BurnInstanceData));
	ZeroMemory(&m_ChromiumInstanceData,sizeof(m_ChromiumInstanceData));
	ZeroMemory(&m_DodgeInstanceData,sizeof(m_DodgeInstanceData));
	ZeroMemory(&m_ExclusionInstanceData,sizeof(m_ExclusionInstanceData));
	ZeroMemory(&m_PosterizeInstanceData,sizeof(m_PosterizeInstanceData));
	ZeroMemory(&m_SolarizeInstanceData,sizeof(m_SolarizeInstanceData));

	m_ChromiumEdge1=0.0f;
	m_nChromiumEdge1ID=0;
	m_ChromiumEdge2=128.0f;
	m_nChromiumEdge2ID=0;
	m_ChromiumRedTint=7.0f;
	m_nChromiumRedTintID=0;
	m_ChromiumGreenTint=22.0f;
	m_nChromiumGreenTintID=0;
	m_ChromiumBlueTint=30.0f;
	m_nChromiumBlueTintID=0;

	m_PosterizeLevels=8.0f;
	m_nPosterizeLevelsID=0;

	m_SolarizeThreshold=128.0f;
	m_nSolarizeThresholdID=0;
	m_SolarizeStart=0.0f;
	m_nSolarizeStartID=0;
	m_SolarizeEnd=255.0f;
	m_nSolarizeEndID=0;
	m_bSolarizeLimit=false;
	m_nSolarizeLimitID=0;
	m_SolarizeFloor=0.0f;
	m_nSolarizeFloorID=0;
	m_SolarizeCeiling=255.0f;
	m_nSolarizeCeilingID=0;

}

CPShopFiltersDfx::~CPShopFiltersDfx()
{

	Pete_Burn_DeInit(&m_BurnInstanceData);
	Pete_Chromium_DeInit(&m_ChromiumInstanceData);
	Pete_Dodge_DeInit(&m_DodgeInstanceData);
	Pete_Exclusion_DeInit(&m_ExclusionInstanceData);
	Pete_Posterize_DeInit(&m_PosterizeInstanceData);
	Pete_Solarize_DeInit(&m_SolarizeInstanceData);

}

HBITMAP	CPShopFiltersDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CPShopFiltersDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CPShopFiltersDfxDlg	dlg;

	dlg.m_eStyle = m_eStyle;

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_eStyle = dlg.m_eStyle;

	return TRUE;

}

void	CPShopFiltersDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
	p->SetInt("S", m_eStyle);

}


BOOL	CPShopFiltersDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_eStyle = static_cast<EPShopFiltersStyle>(p->GetInt("S", m_eStyle));

	return TRUE;
}

BOOL	CPShopFiltersDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	UnRegisterAllVariables();

	switch (m_eStyle) {

		case eBurnStyle: {
			// Do nothing
		}break;

		case eChromiumStyle: {
			m_nChromiumEdge1ID=RegisterFloat(m_pEngine,&m_ChromiumEdge1,"Edge 1",0.0f,255.0f);
			m_nChromiumEdge2ID=RegisterFloat(m_pEngine,&m_ChromiumEdge2,"Edge 2",0.0f,255.0f);
			m_nChromiumRedTintID=RegisterFloat(m_pEngine,&m_ChromiumRedTint,"Red Tint",0.0f,255.0f);
			m_nChromiumGreenTintID=RegisterFloat(m_pEngine,&m_ChromiumGreenTint,"Green Tint",0.0f,255.0f);
			m_nChromiumBlueTintID=RegisterFloat(m_pEngine,&m_ChromiumBlueTint,"Blue Tint",0.0f,255.0f);
		}break;

		case eDodgeStyle: {
			// Do nothing
		}break;

		case eExclusionStyle: {
			// Do nothing
		}break;

		case ePosterizeStyle: {
			m_nPosterizeLevelsID=RegisterFloat(m_pEngine,&m_PosterizeLevels,"Levels",1.0f,255.0f);
		}break;

		case eSolarizeStyle: {
			m_nSolarizeThresholdID=RegisterFloat(m_pEngine,&m_SolarizeThreshold,"Threshold",0.0f,255.0f);
			m_nSolarizeStartID=RegisterFloat(m_pEngine,&m_SolarizeStart,"Start",0.0f,255.0f);
			m_nSolarizeEndID=RegisterFloat(m_pEngine,&m_SolarizeEnd,"End",0.0f,255.0f);
			m_nSolarizeLimitID=RegisterBool(m_pEngine,&m_bSolarizeLimit,"Limit");
			m_nSolarizeFloorID=RegisterFloat(m_pEngine,&m_SolarizeFloor,"Floor",0.0f,255.0f);
			m_nSolarizeCeilingID=RegisterFloat(m_pEngine,&m_SolarizeCeiling,"Ceiling",0.0f,255.0f);
		}break;

		default: {
			ASSERT(FALSE);
		}break;

	}

	return TRUE;

}

BOOL	CPShopFiltersDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	switch (m_eStyle) {

		case eBurnStyle: {
			Pete_Burn_Init(&m_BurnInstanceData,m_nXRes,m_nYRes);
		}break;

		case eChromiumStyle: {
			Pete_Chromium_Init(&m_ChromiumInstanceData,m_nXRes,m_nYRes);
		}break;

		case eDodgeStyle: {
			Pete_Dodge_Init(&m_DodgeInstanceData,m_nXRes,m_nYRes);
		}break;

		case eExclusionStyle: {
			Pete_Exclusion_Init(&m_ExclusionInstanceData,m_nXRes,m_nYRes);
		}break;

		case ePosterizeStyle: {
			Pete_Posterize_Init(&m_PosterizeInstanceData,m_nXRes,m_nYRes);
		}break;

		case eSolarizeStyle: {
			Pete_Solarize_Init(&m_SolarizeInstanceData,m_nXRes,m_nYRes);
		}break;

		default: {
			ASSERT(FALSE);
		}break;

	}

	return TRUE;
}

BOOL	CPShopFiltersDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();


	switch (m_eStyle) {

		case eBurnStyle: {

			SPete_Burn_Settings Settings;

			Pete_Burn_Render(&m_BurnInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		case eChromiumStyle: {

			SPete_Chromium_Settings Settings;
			Settings.m_Edge1=m_ChromiumEdge1;
			Settings.m_Edge2=m_ChromiumEdge2;
			Settings.m_RedTint=m_ChromiumRedTint;
			Settings.m_GreenTint=m_ChromiumGreenTint;
			Settings.m_BlueTint=m_ChromiumBlueTint;

			Pete_Chromium_Render(&m_ChromiumInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		case eDodgeStyle: {

			SPete_Dodge_Settings Settings;

			Pete_Dodge_Render(&m_DodgeInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		case eExclusionStyle: {

			SPete_Exclusion_Settings Settings;

			Pete_Exclusion_Render(&m_ExclusionInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		case ePosterizeStyle: {

			SPete_Posterize_Settings Settings;
			Settings.m_PosterizeLevels=m_PosterizeLevels;

			Pete_Posterize_Render(&m_PosterizeInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		case eSolarizeStyle: {

			SPete_Solarize_Settings Settings;

			Settings.m_Threshold=m_SolarizeThreshold;
			Settings.m_Start=m_SolarizeStart;
			Settings.m_End=m_SolarizeEnd;
			if (m_bSolarizeLimit) {
				Settings.m_DoLimit=1.0f;
			} else {
				Settings.m_DoLimit=0.0f;
			}
			Settings.m_Floor=m_SolarizeFloor;
			Settings.m_Ceiling=m_SolarizeCeiling;

			Pete_Solarize_Render(&m_SolarizeInstanceData,&Settings,pInputMem,pOutputMem);

		}break;

		default: {
			ASSERT(FALSE);
		}break;

	}

	return TRUE;
}

DWORD	CPShopFiltersDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CPShopFiltersDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CPShopFiltersDfx::GetInputState()
{
	return 0xffffffff;
}

void	CPShopFiltersDfx::UnRegisterAllVariables(void) {

	m_pEngine->UnregisterVariable(m_nChromiumEdge1ID);
	m_nChromiumEdge1ID=0;
	m_pEngine->UnregisterVariable(m_nChromiumEdge2ID);
	m_nChromiumEdge2ID=0;
	m_pEngine->UnregisterVariable(m_nChromiumRedTintID);
	m_nChromiumRedTintID=0;
	m_pEngine->UnregisterVariable(m_nChromiumGreenTintID);
	m_nChromiumGreenTintID=0;
	m_pEngine->UnregisterVariable(m_nChromiumBlueTintID);
	m_nChromiumBlueTintID=0;

	m_pEngine->UnregisterVariable(m_nPosterizeLevelsID);
	m_nPosterizeLevelsID=0;

	m_pEngine->UnregisterVariable(m_nSolarizeThresholdID);
	m_nSolarizeThresholdID=0;
	m_pEngine->UnregisterVariable(m_nSolarizeStartID);
	m_nSolarizeStartID=0;
	m_pEngine->UnregisterVariable(m_nSolarizeEndID);
	m_nSolarizeEndID=0;
	m_pEngine->UnregisterVariable(m_nSolarizeLimitID);
	m_nSolarizeLimitID=0;
	m_pEngine->UnregisterVariable(m_nSolarizeFloorID);
	m_nSolarizeFloorID=0;
	m_pEngine->UnregisterVariable(m_nSolarizeCeilingID);
	m_nSolarizeCeilingID=0;

}