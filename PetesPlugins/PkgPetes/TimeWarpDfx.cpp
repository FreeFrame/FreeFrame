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
#include "TimeWarpDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

#include "TimeWarp.h"

class CTimeWarpDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CTimeWarpDfx();
	CTimeWarpDfx();
	
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
	void	UnregisterUsedVariables(void);

	void	StoreInputFrame(DWORD* pInput);
	void	WriteOutputFrame(DWORD* pOutput);
	void	Update(void);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	HGLOBAL	m_hStoredFrames;
	int		m_nStoredFramesMemUsage;
	int		m_nCurrentStoredFramesCount;
	int		m_nMaxStoredFramesCount;
	int		m_nCurrentStorageFrame;

	float	m_PlaybackOffset;

	ETimeWarpStyle m_eStyle;
	ETimeWarpStyle m_ePreviousStyle;

	float	m_PolariBPM;
	int		m_nPolariBPMID;
	bool	m_bPolariDoLoop;
	int		m_nPolariDoLoopID;

	float	m_SineSlurAngle;
	float	m_SineSlurPeriod;
	int		m_nSineSlurPeriodID;
	float	m_SineSlurAmplitude;
	int		m_nSineSlurAmplitudeID;

	float	m_StutterAmplitude;
	int		m_nStutterAmplitudeID;

	float	m_CustomPlaybackOffset;
	int		m_nCustomPlaybackOffsetID;

	bool	m_bPlayingForward;

	SPete_TimeWarp_Data m_InstanceData;
};

BEGIN_INPUTSCREENS_DESC(CTimeWarpDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CTimeWarpDfx)

BEGIN_OUTPUTSCREENS_DESC(CTimeWarpDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CTimeWarpDfx)

BEGIN_EFFECT_DESC(CTimeWarpDfx, 10203216861479948, PETE_VJO_VERSION , "Petes\\TimeWarp", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CTimeWarpDfx) 

REGISTER_DFXFACTORY(CTimeWarpDfx)

const int cnFramesToStore=25;

CTimeWarpDfx::CTimeWarpDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;
	
	m_hStoredFrames = NULL;
	m_nStoredFramesMemUsage = 0;
	m_nCurrentStoredFramesCount=0;
	m_nMaxStoredFramesCount = cnFramesToStore;
	m_nCurrentStorageFrame = 0;

	m_eStyle = ePolariStyle;
	m_ePreviousStyle = ePolariStyle;

	m_PolariBPM = 120.0f;
	m_nPolariBPMID = 0;
	m_bPolariDoLoop = false;
	m_nPolariDoLoopID = 0;
	
	m_SineSlurPeriod = 30.0f;
	m_nSineSlurPeriodID=0;
	m_SineSlurAmplitude = 0.5f;
	m_nSineSlurAmplitudeID=0;

	m_StutterAmplitude = 1.0f;
	m_nStutterAmplitudeID = 0;

	m_CustomPlaybackOffset = 0.0f;
	m_nCustomPlaybackOffsetID = 0;

	m_PlaybackOffset = 0.0f;

	m_bPlayingForward=false;

	m_nMemUsage = sizeof(CTimeWarpDfx);

	ZeroMemory(&m_InstanceData,sizeof(m_InstanceData));

	SetupPlayback();

}

CTimeWarpDfx::~CTimeWarpDfx()
{
	Pete_TimeWarp_DeInit(&m_InstanceData);
}

HBITMAP	CTimeWarpDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CTimeWarpDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CTimeWarpDfxDlg	dlg;

	dlg.m_eStyle = m_eStyle;
	dlg.m_nFramesToStore = m_nMaxStoredFramesCount;

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_eStyle = dlg.m_eStyle;
	m_nMaxStoredFramesCount = dlg.m_nFramesToStore;

	if (dlg.m_bStyleChanged) {
		UnregisterUsedVariables();
	}

	return TRUE;

}

void	CTimeWarpDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
	p->SetInt("M", m_nMaxStoredFramesCount);
	p->SetInt("S", m_eStyle);

}


BOOL	CTimeWarpDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_nMaxStoredFramesCount = p->GetInt("M", m_nMaxStoredFramesCount);
	m_eStyle = static_cast<ETimeWarpStyle>(p->GetInt("S", m_eStyle));
	
	return TRUE;	
}

BOOL	CTimeWarpDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

//	if (m_eStyle!=m_ePreviousStyle) {
//		UnregisterUsedVariables();
		SetupPlayback();
		m_ePreviousStyle=m_eStyle;
//	}

	return TRUE;
}

BOOL	CTimeWarpDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	Pete_TimeWarp_Init(&m_InstanceData,m_nXRes,m_nYRes,m_nMaxStoredFramesCount);

	return TRUE;
}

BOOL	CTimeWarpDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	SPete_TimeWarp_Settings Settings;
	Settings.m_PolariBPM=m_PolariBPM;
	Settings.m_CustomPlaybackOffset=m_CustomPlaybackOffset;
	Settings.m_SineSlurAmplitude=m_SineSlurAmplitude;
	Settings.m_SineSlurPeriod=m_SineSlurPeriod;
	Settings.m_StutterAmplitude=m_StutterAmplitude;
	Settings.m_Style=((int)(m_eStyle));
	if (m_bPolariDoLoop) {
		Settings.m_PolariDoLoop=1.0f;
	} else {
		Settings.m_PolariDoLoop=0.0f;
	}
	
	Pete_TimeWarp_Render(&m_InstanceData,&Settings,pInputMem,pOutputMem);

	return TRUE;
}

DWORD	CTimeWarpDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CTimeWarpDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CTimeWarpDfx::GetInputState()
{
	return 0xffffffff;
}

void CTimeWarpDfx::SetupStoredFramesBuffer(int nWidth, int nHeight) {

	const int nStoredFramesSize=
		(nWidth*nHeight*m_nMaxStoredFramesCount*sizeof(DWORD)*2);

	REALLOCBUFFER(m_hStoredFrames,nStoredFramesSize,m_nMemUsage);

	m_nCurrentStoredFramesCount=0;
	m_nCurrentStorageFrame = 0;

}

void	CTimeWarpDfx::SetupPlayback(void) {

	switch (m_eStyle) {

		case ePolariStyle: {
			m_PlaybackOffset = 0.0f;

			if (m_pEngine!=NULL) {
				m_nPolariBPMID=
					RegisterFloat(m_pEngine,&m_PolariBPM,"BPM",10.0f,180.0f);
				m_nPolariDoLoopID=
					RegisterBool(m_pEngine,&m_bPolariDoLoop,"Do Loop");
			}

		}break;

		case eSineSlurStyle: {
			m_SineSlurAngle = 0.0f;							 
			m_nSineSlurPeriodID=
				RegisterFloat(m_pEngine,&m_SineSlurPeriod,"Period",1.0f,10000.0f,1.0f,100000.0f,2000.0f);
			m_nSineSlurAmplitudeID=
				RegisterFloat(m_pEngine,&m_SineSlurAmplitude,"Amplitude",0.0f,1.0f);
		}break;

		case eStutterStyle: {

			m_nStutterAmplitudeID=
				RegisterFloat(m_pEngine,&m_StutterAmplitude,"Amplitude",0.0f,1.0f);
							
		}break;

		case eCustomStyle: {
			m_nCustomPlaybackOffsetID=
				RegisterFloat(m_pEngine,&m_CustomPlaybackOffset,"Playback Offset",0.0f,1.0f);
		}break;

		case eStopStartStyle: {
			m_PlaybackOffset=0.0f;
			m_bPlayingForward=false;
		}

		default: {
			ASSERT(FALSE);
		}

	}
	
	m_SineSlurAngle = 0.0f;

}

void	CTimeWarpDfx::UnregisterUsedVariables(void) {

	if (m_nPolariBPMID!=0) {
		m_pEngine->UnregisterVariable(m_nPolariBPMID);
		m_nPolariBPMID=0;
	}

	if (m_nPolariDoLoopID!=0) {
		m_pEngine->UnregisterVariable(m_nPolariDoLoopID);
		m_nPolariDoLoopID=0;
	}

	if (m_nSineSlurPeriodID!=0) {
		m_pEngine->UnregisterVariable(m_nSineSlurPeriodID);
		m_nSineSlurPeriodID=0;
	}
	
	if (m_nSineSlurAmplitudeID!=0) {
		m_pEngine->UnregisterVariable(m_nSineSlurAmplitudeID);
		m_nSineSlurAmplitudeID=0;
	}

	if (m_nStutterAmplitudeID!=0) {
		m_pEngine->UnregisterVariable(m_nStutterAmplitudeID);
		m_nStutterAmplitudeID=0;
	}

	if (m_nCustomPlaybackOffsetID!=0) {
		m_pEngine->UnregisterVariable(m_nCustomPlaybackOffsetID);
		m_nCustomPlaybackOffsetID=0;
	}

}