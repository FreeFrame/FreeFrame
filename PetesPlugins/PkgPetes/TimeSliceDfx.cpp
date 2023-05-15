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

#include "TimeSliceDfxDlg.h"

#include "PeteHelpers.h"

#undef DECLARE_INPUTSCREENS

#define	DECLARE_INPUTSCREENS()\
protected:\
		static _screen_info	m_InputScreensInfo[];\
virtual	SIZE	__getInputScreenRes(int p)		{return m_InputScreensInfo[p].resolution;}\
virtual	CHAR	*__getInputScreenName(int p)	{return m_InputScreensInfo[p].title;}

class CTimeSliceDfx: public CDFX
{
	DECLARE_EFFECT()

	virtual INT CTimeSliceDfx::__getInputScreensCount(void);

public:
	~CTimeSliceDfx();
	CTimeSliceDfx();

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

	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);
	void	WriteOutputFrameImageMap(DWORD* pSource, DWORD* pImageMap,DWORD* pOutput);
	void	CopyRectangle(DWORD* pInput,DWORD* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY);

	void	SetupStoredFramesBuffer(int nWidth, int nHeight);
	void	SetupPlayback(void);

	void	StoreInputFrame(DWORD* pInput);
	void	UpdatePlaybackOffset(float Time);
	void	UnregisterUsedVariables(void);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	HGLOBAL m_hDisplayBuffer;
	HGLOBAL	m_hStoredFrames;
	int		m_nStoredFramesMemUsage;
	int		m_nCurrentStoredFramesCount;
	int		m_nMaxStoredFramesCount;
	int		m_nCurrentStorageFrame;

	float	m_PlaybackOffset;

	ETimeSliceStyle m_eStyle;
	ETimeSliceStyle m_ePreviousStyle;
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

	bool	m_bIsHorizontal;
	int		m_nIsHorizontalID;

	float	m_SliceSize;
	int		m_nSliceSizeID;

	int		m_nSliceStart;

};

BEGIN_INPUTSCREENS_DESC(CTimeSliceDfx)
{{-1,-1}, "RGB 0"},
{{-1,-1}, "Image Map"},
};

BEGIN_OUTPUTSCREENS_DESC(CTimeSliceDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CTimeSliceDfx)

BEGIN_EFFECT_DESC(CTimeSliceDfx, 10203216861479981, PETE_VJO_VERSION , "Petes\\TimeSlice", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CTimeSliceDfx)

REGISTER_DFXFACTORY(CTimeSliceDfx)


const int cnFramesToStore=50;

CTimeSliceDfx::CTimeSliceDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CTimeSliceDfx);

	m_bIsHorizontal=false;
	m_SliceSize=2.0f;
	m_nSliceStart=0;

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

	SetupPlayback();

	m_hDisplayBuffer=NULL;

}

CTimeSliceDfx::~CTimeSliceDfx()
{

	FREEBUFFER(m_hDisplayBuffer,m_nMemUsage);
	FREEBUFFER(m_hStoredFrames,m_nMemUsage);

}

INT CTimeSliceDfx::__getInputScreensCount(void) {

	if (m_eStyle==eImageMapStyle) {
		return 2;
	} else {
		return 1;
	}

}

HBITMAP	CTimeSliceDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CTimeSliceDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CTimeSliceDfxDlg	dlg;

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

void	CTimeSliceDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

	p->SetInt("M", m_nMaxStoredFramesCount);
	p->SetInt("S", m_eStyle);

}


BOOL	CTimeSliceDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_nMaxStoredFramesCount = p->GetInt("M", m_nMaxStoredFramesCount);
	m_eStyle = static_cast<ETimeSliceStyle>(p->GetInt("S", m_eStyle));

	return TRUE;
}

BOOL	CTimeSliceDfx::UpdateConfig()
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

BOOL	CTimeSliceDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	SetupStoredFramesBuffer(sz.cx,sz.cy);

	m_nIsHorizontalID=RegisterBool(
		m_pEngine,&m_bIsHorizontal,"Horizontal");

	m_nSliceSizeID=RegisterFloat(
		m_pEngine,&m_SliceSize,"Slice Size",1.0f,100.0f);

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels=nWidth*nHeight;

	const int nNumBytes=(nNumPixels*sizeof(DWORD));

	REALLOCBUFFER(m_hDisplayBuffer,nNumBytes,m_nMemUsage);

	DWORD* pDisplayBuffer=(DWORD*)GETPTR(m_hDisplayBuffer);

	ZeroMemory(pDisplayBuffer,nNumBytes);

	FREEPTR(m_hDisplayBuffer);

	return TRUE;
}

BOOL	CTimeSliceDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	StoreInputFrame(pInputMem);

	if (m_eStyle==eImageMapStyle) {

		DWORD* pImageMap = (DWORD*)(ppInput[1]->GetBuffer());

		WriteOutputFrameImageMap(pInputMem,pImageMap,pOutputMem);

	} else {

		WriteOutputFrame(pInputMem,pOutputMem);

	}

	return TRUE;

}

void	CTimeSliceDfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(DWORD));

	DWORD* pStoredFrames=static_cast<DWORD*>(GETPTR(m_hStoredFrames));

	bool bIsDone=false;
	int nSliceStart=0;
	float Time=0.0f;
	while (!bIsDone) {

		UpdatePlaybackOffset(Time);
		Time+=1.0f;

		int nCurrentPlaybackOffset=static_cast<int>(m_PlaybackOffset);

		nCurrentPlaybackOffset=GateInt(nCurrentPlaybackOffset,0,(m_nCurrentStoredFramesCount-1));

		int nCurrentPlaybackFrame=
			(m_nCurrentStorageFrame)-nCurrentPlaybackOffset;

		if (nCurrentPlaybackFrame<0) {
			nCurrentPlaybackFrame+=m_nCurrentStoredFramesCount;
		}

		DWORD*const pStoredSource=pStoredFrames+(nNumPixels*nCurrentPlaybackFrame);

		int nLeftX;
		int nTopY;
		int nRightX;
		int nBottomY;
		if (m_bIsHorizontal) {

			nLeftX=0;
			nRightX=nWidth;

			nTopY=nSliceStart;
			nBottomY=nTopY+(int)(m_SliceSize);

			nSliceStart+=(int)m_SliceSize;
			if (nSliceStart>=nHeight) {
				bIsDone=true;
			}

		} else {

			nTopY=0;
			nBottomY=nHeight;

			nLeftX=nSliceStart;
			nRightX=nLeftX+(int)(m_SliceSize);

			nSliceStart+=(int)(m_SliceSize);
			if (nSliceStart>=nWidth) {
				bIsDone=true;
			}

		}

		CopyRectangle(pStoredSource,pOutput,nLeftX,nTopY,nRightX,nBottomY);

	}

	FREEPTR(m_hStoredFrames);

}

void	CTimeSliceDfx::WriteOutputFrameImageMap(DWORD* pSource, DWORD* pImageMap,DWORD* pOutput) {

	const int nFixedShift=8;
	const int nFixedMult=(1<<nFixedShift);

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(DWORD));

	DWORD* pStoredFramesStart=static_cast<DWORD*>(GETPTR(m_hStoredFrames));

	DWORD* pStoredFramesCurrent=
		pStoredFramesStart+
		(m_nCurrentStorageFrame*nNumPixels);

	const int nFramesToStore=m_nCurrentStoredFramesCount;

	pStoredFramesCurrent-=nNumPixels;
	if (pStoredFramesCurrent<pStoredFramesStart) {
		pStoredFramesCurrent+=(nFramesToStore*nNumPixels);
	}

	const int nOffsetMult=(nFramesToStore*nFixedMult)/256;

	DWORD* pCurrentOutput=pOutput;
	const DWORD* pOutputEnd=(pOutput+nNumPixels);
	DWORD* pCurrentImageMap=pImageMap;

	while (pCurrentOutput!=pOutputEnd) {

		const DWORD ImageMapColour=*pCurrentImageMap;

		unsigned int nTimeOffset=GetLuminance(ImageMapColour);
		nTimeOffset*=nOffsetMult;
		nTimeOffset>>=(nFixedShift+8);

		DWORD* pSourceStoredFrame=
			pStoredFramesCurrent-
			(nTimeOffset*nNumPixels);

		if (pSourceStoredFrame<pStoredFramesStart) {
			pSourceStoredFrame+=(nFramesToStore*nNumPixels);
		}

		const DWORD SourceColour=*pSourceStoredFrame;

		*pCurrentOutput=SourceColour;

		pCurrentOutput+=1;
		pCurrentImageMap+=1;
		pStoredFramesCurrent+=1;

	}

	FREEPTR(m_hStoredFrames);

}

void	CTimeSliceDfx::StoreInputFrame(DWORD* pInput) {
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

void	CTimeSliceDfx::UpdatePlaybackOffset(float Time) {

	switch (m_eStyle) {

		case ePolariStyle: {

			const float FramesCount=static_cast<float>((m_nCurrentStoredFramesCount-2.0f)*2);

			m_PlaybackOffset=Time;
			if (m_PlaybackOffset>=FramesCount) {
				m_PlaybackOffset=fmodf(m_PlaybackOffset,FramesCount);
			}

			if (m_PlaybackOffset<0.0f) {
				m_PlaybackOffset=FramesCount-fmodf(-m_PlaybackOffset,FramesCount);
			}

			if (m_PlaybackOffset>=(FramesCount/2.0f)) {
				m_PlaybackOffset=FramesCount-m_PlaybackOffset;
			}

			m_PlaybackOffset+=1.0f;

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
void CTimeSliceDfx::SetupStoredFramesBuffer(int nWidth, int nHeight) {

	const int nStoredFramesSize=
		(nWidth*nHeight*m_nMaxStoredFramesCount*sizeof(DWORD)*2);

	REALLOCBUFFER(m_hStoredFrames,nStoredFramesSize,m_nMemUsage);

	m_nCurrentStoredFramesCount=0;
	m_nCurrentStorageFrame = 0;

}

void	CTimeSliceDfx::SetupPlayback(void) {

	switch (m_eStyle) {

		case ePolariStyle: {
			m_PlaybackOffset = 0.0f;
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

		case eImageMapStyle: {
			// do nothing
		}break;

		default: {
			ASSERT(FALSE);
		}

	}

	m_SineSlurAngle = 0.0f;

}

void	CTimeSliceDfx::CopyRectangle(DWORD* pInput,DWORD* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nStartX=GateInt(nLeftX,0,(nWidth-1));
	const int nEndX=GateInt(nRightX,0,(nWidth-1));

	const int nStartY=GateInt(nTopY,0,(nHeight-1));
	const int nEndY=GateInt(nBottomY,0,(nHeight-1));

	int nY;
	for (nY=nStartY; nY<nEndY; ++nY) {

		DWORD* pInputLineStart=pInput+(nY*nWidth);
		DWORD* pOutputLineStart=pOutput+(nY*nWidth);

		DWORD* pCurrentInput=pInputLineStart+nStartX;
		DWORD* pCurrentOutput=pOutputLineStart+nStartX;

		int nX;
		for (nX=nStartX; nX<nEndX; ++nX) {

			*pCurrentOutput=*pCurrentInput;

			++pCurrentInput;
			++pCurrentOutput;

		}

	}

}

DWORD	CTimeSliceDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CTimeSliceDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CTimeSliceDfx::GetInputState()
{
	return 0xffffffff;
}

void	CTimeSliceDfx::UnregisterUsedVariables(void) {

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