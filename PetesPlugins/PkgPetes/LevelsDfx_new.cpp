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
#include "TimeWarpDfxDlg.h"
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

	void	SetupStoredFramesBuffer(int nWidth, int nHeight);
	void	SetupPlayback(void);
	void	SetupVariables(void);
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
	int m_nFramesToStore;

	float	m_SineSlurAngle;
	float	m_SineSlurPeriod;
	int		m_nSineSlurPeriodID;
	float	m_SineSlurAmplitude;
	int		m_nSineSlurAmplitudeID;

	float	m_StutterAmplitude;
	int		m_nStutterAmplitudeID;

	float	m_CustomPlaybackOffset;
	int		m_nCustomPlaybackOffsetID;

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

BEGIN_EFFECT_DESC(CLevelsDfx, 10203216861479956, PETE_VJO_VERSION , "Petes\\Levels", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLevelsDfx) 

REGISTER_DFXFACTORY(CLevelsDfx)

const int cnFramesToStore=25;

CLevelsDfx::CLevelsDfx()
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

	m_SineSlurPeriod = 30.0f;
	m_nSineSlurPeriodID=0;
	m_SineSlurAmplitude = 0.5f;
	m_nSineSlurAmplitudeID=0;

	m_StutterAmplitude = 1.0f;
	m_nStutterAmplitudeID = 0;

	m_CustomPlaybackOffset = 0.0f;
	m_nCustomPlaybackOffsetID = 0;

	m_PlaybackOffset = 0.0f;

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

	SetupPlayback();

	SetupVariables();

	m_nMemUsage = sizeof(CLevelsDfx);
}

CLevelsDfx::~CLevelsDfx()
{
	FREEBUFFER(m_hStoredFrames,m_nMemUsage);
}

HBITMAP	CLevelsDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLevelsDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CTimeWarpDfxDlg	dlg;

	dlg.m_eStyle = m_eStyle;
	dlg.m_nFramesToStore = m_nMaxStoredFramesCount;

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_eStyle = dlg.m_eStyle;
	m_nMaxStoredFramesCount = dlg.m_nFramesToStore;

	return TRUE;

}

void	CLevelsDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);
	p->SetInt("M", m_nMaxStoredFramesCount);
	p->SetInt("S", m_eStyle);

}


BOOL	CLevelsDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_nMaxStoredFramesCount = p->GetInt("M", m_nMaxStoredFramesCount);
	m_eStyle = static_cast<ETimeWarpStyle>(p->GetInt("S", m_eStyle));
	
	return TRUE;	
}

BOOL	CLevelsDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

//	if (m_eStyle!=m_ePreviousStyle) {
		UnregisterUsedVariables();
		SetupPlayback();
		m_ePreviousStyle=m_eStyle;
//	}

	return TRUE;
}

BOOL	CLevelsDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	SetupStoredFramesBuffer(sz.cx,sz.cy);

	return TRUE;
}

BOOL	CLevelsDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	if (m_hStoredFrames==NULL) {
		return FALSE;
	}
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	StoreInputFrame(pInputMem);

	WriteOutputFrame(pOutputMem);

	Update();

	return TRUE;

}

void	CLevelsDfx::StoreInputFrame(DWORD* pInput) {
	const int nNumPixels = m_nXRes*m_nYRes;

	ASSERT((m_nCurrentStorageFrame>=0)&&(m_nCurrentStorageFrame<m_nMaxStoredFramesCount));

	DWORD* pStoredFrames=static_cast<DWORD*>(GETPTR(m_hStoredFrames));

	DWORD*const pDest=pStoredFrames+(nNumPixels*m_nCurrentStorageFrame);

	memcpy(pDest,pInput,(nNumPixels*sizeof(DWORD)));

	m_nCurrentStoredFramesCount += 1;
	if (m_nCurrentStoredFramesCount>=m_nMaxStoredFramesCount) {
		m_nCurrentStoredFramesCount=m_nMaxStoredFramesCount;
	}

	m_nCurrentStorageFrame +=1;
	if (m_nCurrentStorageFrame>=m_nMaxStoredFramesCount) {
		m_nCurrentStorageFrame%=m_nMaxStoredFramesCount;
	}

	FREEPTR(m_hStoredFrames);

}


void	CLevelsDfx::WriteOutputFrame(DWORD* pOutput) {
	const int nNumPixels = m_nXRes*m_nYRes;

	int nCurrentPlaybackOffset=static_cast<int>(m_PlaybackOffset);

	if (nCurrentPlaybackOffset>=m_nCurrentStoredFramesCount) {
		nCurrentPlaybackOffset=(m_nCurrentStoredFramesCount-1);
	}

	if (nCurrentPlaybackOffset<0) {
		nCurrentPlaybackOffset=0;
	}

	int nCurrentPlaybackFrame=
		m_nCurrentStorageFrame-nCurrentPlaybackOffset;

	if (nCurrentPlaybackFrame<0) {
		nCurrentPlaybackFrame+=m_nCurrentStoredFramesCount;
	}

	DWORD* pStoredFrames=static_cast<DWORD*>(GETPTR(m_hStoredFrames));
	DWORD*const pSource=pStoredFrames+(nNumPixels*nCurrentPlaybackFrame);

	memcpy(pOutput,pSource,(nNumPixels*sizeof(DWORD)));

	FREEPTR(m_hStoredFrames);

}

void	CLevelsDfx::Update(void) {

	switch (m_eStyle) {

		case ePolariStyle: {

			const float FramesCount=static_cast<float>(m_nCurrentStoredFramesCount);

			m_PlaybackOffset+=2.0f;
			if (m_PlaybackOffset>=m_nCurrentStoredFramesCount) {
				m_PlaybackOffset=fmodf(m_PlaybackOffset,FramesCount);
			}

			if (m_PlaybackOffset<0.0f) {
				m_PlaybackOffset=FramesCount-fmodf(-m_PlaybackOffset,FramesCount);
			}
						   
		}break;

		case eSineSlurStyle: {

			m_SineSlurAngle+=(PI/m_SineSlurPeriod);
			m_SineSlurAngle=fmodf(m_SineSlurAngle,2.0f*PI);
			
			const float HalfFramesCount=
				static_cast<float>(m_nCurrentStoredFramesCount)/2.0f;
			
			m_PlaybackOffset=HalfFramesCount+
				(sinf(m_SineSlurAngle)*HalfFramesCount*m_SineSlurAmplitude);
							 
		}break;

		case eStutterStyle: {

			const float RandomFloat=rand()/static_cast<float>(RAND_MAX);

			m_PlaybackOffset=
				RandomFloat*m_StutterAmplitude*static_cast<float>(m_nCurrentStoredFramesCount);
							
		}break;

		case eCustomStyle: {

			m_PlaybackOffset=
				m_CustomPlaybackOffset*static_cast<float>(m_nCurrentStoredFramesCount);
			
		}break;

		default: {
			ASSERT(FALSE);
		}

	}

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

void CLevelsDfx::SetupStoredFramesBuffer(int nWidth, int nHeight) {

	const int nStoredFramesSize=
		(nWidth*nHeight*m_nMaxStoredFramesCount*sizeof(DWORD)*2);

	REALLOCBUFFER(m_hStoredFrames,nStoredFramesSize,m_nMemUsage);

	m_nCurrentStoredFramesCount=0;
	m_nCurrentStorageFrame = 0;

}

void	CLevelsDfx::SetupPlayback(void) {

	switch (m_eStyle) {

		case ePolariStyle: {
			m_PlaybackOffset = 0.0f;
		}break;

		case eSineSlurStyle: {
			m_SineSlurAngle = 0.0f;							 
			m_nSineSlurPeriodID=
				RegisterFloat(m_pEngine,&m_SineSlurPeriod,"Period",1.0f,10000000.0f,1.0f,100000.0f,2000.0f);
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

		default: {
			ASSERT(FALSE);
		}

	}
	
	m_SineSlurAngle = 0.0f;

}

void	CLevelsDfx::SetupVariables(void) {

	switch (m_eStyle) {

		case eUniformStyle: {

			m_nUniformInputFloorID=
				RegisterFloat(m_pEngine,&m_UniformInputFloor,"InputFloor",1.0f,10000000.0f,1.0f,100000.0f,2000.0f);
#if 0
			m_nUniformInputCeilingID=
				RegisterFloat(m_pEngine,&m_UniformInputCeiling,"Input Ceiling",0.0f,255.0f,1.0f);
	
			m_nUniformOutputFloorID=
				RegisterFloat(m_pEngine,&m_UniformOutputFloor,"Output Floor",0.0f,255.0f,1.0f);
			m_nUniformOutputCeilingID=
				RegisterFloat(m_pEngine,&m_UniformOutputCeiling,"Output Ceiling",0.0f,255.0f,1.0f);
#endif							
		}break;

		case eSeperateRGBStyle: {
#if 0
			m_nRedInputFloorID=
				RegisterFloat(m_pEngine,&m_RedInputFloor,"Red Input Floor",0.0f,255.0f,1.0f);
			m_nRedInputCeilingID=
				RegisterFloat(m_pEngine,&m_RedInputCeiling,"Red Input Ceiling",0.0f,255.0f,1.0f);
	
			m_nRedOutputFloorID=
				RegisterFloat(m_pEngine,&m_RedOutputFloor,"Red Output Floor",0.0f,255.0f,1.0f);
			m_nRedOutputCeilingID=
				RegisterFloat(m_pEngine,&m_RedOutputCeiling,"Red Output Ceiling",0.0f,255.0f,1.0f);


			m_nGreenInputFloorID=
				RegisterFloat(m_pEngine,&m_GreenInputFloor,"Green Input Floor",0.0f,255.0f,1.0f);
			m_nGreenInputCeilingID=
				RegisterFloat(m_pEngine,&m_GreenInputCeiling,"Green Input Ceiling",0.0f,255.0f,1.0f);
	
			m_nGreenOutputFloorID=
				RegisterFloat(m_pEngine,&m_GreenOutputFloor,"Green Output Floor",0.0f,255.0f,1.0f);
			m_nGreenOutputCeilingID=
				RegisterFloat(m_pEngine,&m_GreenOutputCeiling,"Green Output Ceiling",0.0f,255.0f,1.0f);


			m_nBlueInputFloorID=
				RegisterFloat(m_pEngine,&m_BlueInputFloor,"Blue Input Floor",0.0f,255.0f,1.0f);
			m_nBlueInputCeilingID=
				RegisterFloat(m_pEngine,&m_BlueInputCeiling,"Blue Input Ceiling",0.0f,255.0f,1.0f);
	
			m_nBlueOutputFloorID=
				RegisterFloat(m_pEngine,&m_BlueOutputFloor,"Blue Output Floor",0.0f,255.0f,1.0f);
			m_nBlueOutputCeilingID=
				RegisterFloat(m_pEngine,&m_BlueOutputCeiling,"Blue Output Ceiling",0.0f,255.0f,1.0f);
#endif

		}break;

		default: {
			ASSERT(FALSE);
		}

	}
	

}

void	CLevelsDfx::UnregisterUsedVariables(void) {

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

