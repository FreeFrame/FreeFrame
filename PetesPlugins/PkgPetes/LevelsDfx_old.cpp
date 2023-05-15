/* ________________________________________________________________________________________

	THIS FILE IS PART OF THE VISUALJOCKEY, COPYRIGHT (C) BY VISUALIGHT, ALL RIGHTS RESERVED

	Sample for dynamic inputs:
	- how to disable the rendering of nodes:
		BOOL	ConfirmRenderSource(int i)

	- how to disable displaying nodes (implies disable rendering too!):
		DWORD	GetInputState()

	- how to save the configuration:
		void  Set- and GetConfigData(...), called within BOOL UpdateConfig()
   ______________________________________________________________________________________*/

#include "stdafx.h"

#include <math.h>

#include "pkgSAMPLES.h"
#include "LevelsDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

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

	void	WriteOutputFrame(DWORD* pInput,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;


	ELevelsStyle m_eStyle;

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

};

BEGIN_INPUTSCREENS_DESC(CLevelsDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CLevelsDfx)

BEGIN_OUTPUTSCREENS_DESC(CLevelsDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLevelsDfx)

BEGIN_EFFECT_DESC(CLevelsDfx, 10203216861479948, PETE_VJO_VERSION , "Petes\\TimeWarp", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLevelsDfx)

REGISTER_DFXFACTORY(CLevelsDfx)

CLevelsDfx::CLevelsDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_eStyle = eUniformStyle;

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

	m_nMemUsage = sizeof(CLevelsDfx);
}

CLevelsDfx::~CLevelsDfx()
{

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

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_eStyle = dlg.m_eStyle;

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

	UnregisterUsedVariables();
	SetupVariables();

	return TRUE;
}

BOOL	CLevelsDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	return TRUE;
}

BOOL	CLevelsDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	WriteOutputFrame(pInputMem,pOutputMem);

	return TRUE;

}

void	CLevelsDfx::WriteOutputFrame(DWORD* pInput,DWORD* pOutput) {
	const int nNumPixels = m_nXRes*m_nYRes;

	memcpy(pOutput,pInput,(nNumPixels*sizeof(DWORD)));

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

		default: {
			ASSERT(FALSE);
		}

	}


}

void	CLevelsDfx::UnregisterUsedVariables(void) {

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

}