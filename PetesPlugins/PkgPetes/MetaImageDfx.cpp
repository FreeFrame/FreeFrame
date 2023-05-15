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

#include "MetaImageDfxDlg.h"

#include "PeteHelpers.h"

#undef DECLARE_INPUTSCREENS

#define	DECLARE_INPUTSCREENS()\
protected:\
		static _screen_info	m_InputScreensInfo[];\
virtual	SIZE	__getInputScreenRes(int p)		{return m_InputScreensInfo[p].resolution;}\
virtual	CHAR	*__getInputScreenName(int p)	{return m_InputScreensInfo[p].title;}

struct SSubImageHeader {
	DWORD m_AverageColour;
};

DWORD* GetSubImagePixelData(SSubImageHeader* pHeader) {
	return (DWORD*)(((char*)pHeader)+sizeof(SSubImageHeader));
}

const int nMaxInputCount=10;

class CMetaImageDfx: public CDFX
{
	DECLARE_EFFECT()

	virtual INT CMetaImageDfx::__getInputScreensCount(void);

public:
	~CMetaImageDfx();
	CMetaImageDfx();

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

	void	WriteOutputFrame(DWORD* pSource, DWORD* pOutput,DWORD** ppInputs,int nInputsCount);
	void	DrawAllSubImages(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight);
	void	DrawSubImage(DWORD* pSource,
		DWORD* pShrunkBuffer,
		DWORD* pOutput,
		int nLeftX,
		int nTopY,
		int nRightX,
		int nBottomY,
		DWORD WholeImageAverage,
		int nClippedLeftX,
		int nClippedTopY,
		int nClippedRightX,
		int nClippedBottomY,
		DWORD SubImageAverage);
	DWORD	ShrinkSourceImage(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight);
	DWORD	ShrinkSourceImageFast(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight);

	void	CreateSubImages(DWORD** ppInputs,int nInputsCount,float SubWidth,float SubHeight);

	DWORD	GetAreaAverage(DWORD* pImage,int nLeftX,int nTopY,int nRightX,int nBottomY,int nStride);

	void	LockSubImagesData(void);
	void	UnLockSubImagesData(void);
	SSubImageHeader* GetSubImage(int nIndex);
	SSubImageHeader* GetBestSubImage(DWORD TargetColour);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	float	m_Size;
	int		m_nSizeID;

	bool	m_bFreeze;
	int		m_nFreezeID;

	bool	m_bDistanceBased;
	int		m_nDistanceBasedID;

	bool	m_bUseSourceAsSubImage;
	int		m_nUseSourceAsSubImageID;

	bool	m_bCheapAndNasty;
	int		m_nCheapAndNastyID;

	int		m_nInputCount;

	HGLOBAL m_hSubImages;
	char*	m_pSubImagesData;

};

BEGIN_INPUTSCREENS_DESC(CMetaImageDfx)
{{-1,-1}, "RGB 0"},
{{-1,-1}, "RGB 1"},
{{-1,-1}, "RGB 2"},
{{-1,-1}, "RGB 3"},
{{-1,-1}, "RGB 4"},
{{-1,-1}, "RGB 5"},
{{-1,-1}, "RGB 6"},
{{-1,-1}, "RGB 7"},
{{-1,-1}, "RGB 8"},
{{-1,-1}, "RGB 9"},
{{-1,-1}, "RGB 10"},
};

BEGIN_OUTPUTSCREENS_DESC(CMetaImageDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CMetaImageDfx)

BEGIN_EFFECT_DESC(CMetaImageDfx, 10203216861479925, PETE_VJO_VERSION , "Petes\\MetaImage", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CMetaImageDfx)

REGISTER_DFXFACTORY(CMetaImageDfx)

CMetaImageDfx::CMetaImageDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_Size=1.0f;

	m_bFreeze=false;
	m_bDistanceBased=false;
	m_bUseSourceAsSubImage=false;
	m_bCheapAndNasty=false;

	m_nInputCount=1;
	m_InputScreensCount=m_nInputCount;

	m_hSubImages=NULL;

	m_nMemUsage = sizeof(CMetaImageDfx);
}

CMetaImageDfx::~CMetaImageDfx()
{

	FREEBUFFER(m_hSubImages,m_nMemUsage);

}

INT CMetaImageDfx::__getInputScreensCount(void) {
	return m_nInputCount;
}

HBITMAP	CMetaImageDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CMetaImageDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CMetaImageDfxDlg dlg;

	dlg.m_nInputCount = m_nInputCount;

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	m_nInputCount = dlg.m_nInputCount;

	m_nInputCount = GateInt(m_nInputCount,1,(nMaxInputCount-1));

	m_InputScreensCount = m_nInputCount;

	return TRUE;

}

void	CMetaImageDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

	p->SetInt("I", m_nInputCount);

}


BOOL	CMetaImageDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	m_nInputCount=p->GetInt("I",m_nInputCount);

	return TRUE;
}

BOOL	CMetaImageDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nSizeID=RegisterFloat(m_pEngine,&m_Size,"Size",0.0f,1.0f);
	m_nFreezeID=RegisterBool(m_pEngine,&m_bFreeze,"Freeze");
	m_nDistanceBasedID=RegisterBool(m_pEngine,&m_bDistanceBased,"Distance Based");
	m_nUseSourceAsSubImageID=RegisterBool(m_pEngine,&m_bUseSourceAsSubImage,"Use Source As Sub Image");
	m_nCheapAndNastyID=RegisterBool(m_pEngine,&m_bCheapAndNasty,"Cheap And Nasty");

	m_InputScreensCount = m_nInputCount;

	return TRUE;
}

BOOL	CMetaImageDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	const int nSizeOfSubImage=
		sizeof(SSubImageHeader)+(m_nXRes*m_nYRes*sizeof(DWORD));

	REALLOCBUFFER(m_hSubImages,nSizeOfSubImage*m_nInputCount,m_nMemUsage);

	return TRUE;
}

BOOL	CMetaImageDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	DWORD* ppInputs[nMaxInputCount];

	int nCountStart;
	if (m_bUseSourceAsSubImage) {
		nCountStart=0;
	} else {
		if (m_nInputCount>1) {
			nCountStart=1;
		} else {
			nCountStart=0;
		}
	}

	const int nInputCount=m_nInputCount-nCountStart;

	int nCount;
	for (nCount=nCountStart; nCount<m_nInputCount; ++nCount) {
		ppInputs[nCount-nCountStart]=(DWORD*)(ppInput[nCount]->GetBuffer());
	}

	WriteOutputFrame(pInputMem,pOutputMem,ppInputs,nInputCount);

	return TRUE;

}

void	CMetaImageDfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput,DWORD** ppInputs,int nInputsCount) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	float SubWidth;
	float SubHeight;

	if (m_bDistanceBased) {

		const float Distance=1.0f+(m_Size*(nHeight-1.0f));

		SubWidth=nWidth/Distance;
		SubHeight=nHeight/Distance;

	} else {

		SubWidth=1+(m_Size*(nWidth-1));
		SubHeight=1+(m_Size*(nHeight-1));
	}


	LockSubImagesData();

	CreateSubImages(ppInputs,nInputsCount,SubWidth,SubHeight);

	DrawAllSubImages(pSource,pOutput,SubWidth,SubHeight);

	UnLockSubImagesData();

}

void	CMetaImageDfx::CreateSubImages(DWORD** ppInputs,int nInputsCount,float SubWidth,float SubHeight) {

	int nCount;
	for (nCount=0; nCount<nInputsCount; ++nCount) {

		SSubImageHeader* pSubImageHeader=GetSubImage(nCount);

		DWORD* pShrunkBuffer=GetSubImagePixelData(pSubImageHeader);

		if (m_bCheapAndNasty) {
			pSubImageHeader->m_AverageColour=
				ShrinkSourceImageFast(ppInputs[nCount],pShrunkBuffer,SubWidth,SubHeight);
		} else {
			pSubImageHeader->m_AverageColour=
				ShrinkSourceImage(ppInputs[nCount],pShrunkBuffer,SubWidth,SubHeight);
		}


	}

}

void	CMetaImageDfx::DrawAllSubImages(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight) {

//	if (m_bFreeze) {
//		pSource=pShrunkBuffer;
//	}

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nHalfWidth=nWidth/2;
	const int nHalfHeight=nHeight/2;

	float CentreX=nHalfWidth+(SubWidth/2);
	float CentreY=nHalfHeight+(SubHeight/2);

	int nSubImageCountX=static_cast<int>(CentreX/SubWidth);
	int nSubImageCountY=static_cast<int>(CentreY/SubHeight);

	const float StartX=(nHalfWidth-(SubWidth/2))-(nSubImageCountX*SubWidth);
	const float EndX=(nHalfWidth+(SubWidth/2))+(nSubImageCountX*SubWidth);

	const float StartY=(nHalfHeight-(SubHeight/2))-(nSubImageCountY*SubHeight);
	const float EndY=(nHalfHeight+(SubHeight/2))+(nSubImageCountY*SubHeight);

	float CurrentY;
	for (CurrentY=StartY; CurrentY<EndY; CurrentY+=SubHeight) {

		float CurrentX;
		for (CurrentX=StartX; CurrentX<EndX; CurrentX+=SubWidth) {

			const int nLeftX=static_cast<int>(CurrentX);
			const int nTopY=static_cast<int>(CurrentY);
			const int nRightX=static_cast<int>(CurrentX+SubWidth);
			const int nBottomY=static_cast<int>(CurrentY+SubHeight);

			const int nClippedLeftX=GateInt(nLeftX,0,nWidth);
			const int nClippedTopY=GateInt(nTopY,0,nHeight);
			const int nClippedRightX=GateInt(nRightX,0,nWidth);
			const int nClippedBottomY=GateInt(nBottomY,0,nHeight);

			DWORD SubImageAverage=GetAreaAverage(
				pSource,
				nClippedLeftX,
				nClippedTopY,
				nClippedRightX,
				nClippedBottomY,
				4);

			SSubImageHeader* pSubImage=GetBestSubImage(SubImageAverage);

			DrawSubImage(pSource,
				GetSubImagePixelData(pSubImage),
				pOutput,
				nLeftX,
				nTopY,
				nRightX,
				nBottomY,
				pSubImage->m_AverageColour,
				nClippedLeftX,
				nClippedTopY,
				nClippedRightX,
				nClippedBottomY,
				SubImageAverage);

		}

	}

}

void	CMetaImageDfx::DrawSubImage(DWORD* pSource,DWORD* pShrunkBuffer,DWORD* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY,DWORD WholeImageAverage,int nClippedLeftX,int nClippedTopY,int nClippedRightX,int nClippedBottomY,DWORD SubImageAverage) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nSubRed=(SubImageAverage>>SHIFT_RED)&0xff;
	const int nSubGreen=(SubImageAverage>>SHIFT_GREEN)&0xff;
	const int nSubBlue=(SubImageAverage>>SHIFT_BLUE)&0xff;

	const int nWholeRed=(WholeImageAverage>>SHIFT_RED)&0xff;
	const int nWholeGreen=(WholeImageAverage>>SHIFT_GREEN)&0xff;
	const int nWholeBlue=(WholeImageAverage>>SHIFT_BLUE)&0xff;

	const int nRedDelta=(nSubRed-nWholeRed);
	const int nGreenDelta=(nSubGreen-nWholeGreen);
	const int nBlueDelta=(nSubBlue-nWholeBlue);

	const int nXDelta=nClippedRightX-nClippedLeftX;
	const int nYDelta=nClippedBottomY-nClippedTopY;

	if ((nXDelta<=0)||
		(nYDelta<=0)) {
		return;
	}


	DWORD* pCurrentSource=pShrunkBuffer;

	pCurrentSource+=((nClippedTopY-nTopY)*nWidth);
	pCurrentSource+=(nClippedLeftX-nLeftX);

	DWORD* pCurrentOutput=pOutput+(nClippedTopY*nWidth)+nClippedLeftX;
	DWORD*const pOutputEnd=(pCurrentOutput+(nYDelta*nWidth)+nXDelta);

	while (pCurrentOutput<pOutputEnd) {

		DWORD*const pOutputLineStart=pCurrentOutput;
		DWORD*const pOutputLineEnd=pCurrentOutput+nXDelta;

		DWORD* pSourceLineStart=pCurrentSource;

		while (pCurrentOutput<pOutputLineEnd) {

			const DWORD SourceColour=*pCurrentSource;

			const DWORD nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const DWORD nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const DWORD nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;

			const DWORD nOutputRed=GateInt(nSourceRed+nRedDelta,0,255);
			const DWORD nOutputGreen=GateInt(nSourceGreen+nGreenDelta,0,255);
			const DWORD nOutputBlue=GateInt(nSourceBlue+nBlueDelta,0,255);

			const DWORD OutputColour=
				((nOutputRed&0xff)<<SHIFT_RED)|
				((nOutputGreen&0xff)<<SHIFT_GREEN)|
				((nOutputBlue&0xff)<<SHIFT_BLUE);

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;
		pCurrentSource=pSourceLineStart+nWidth;

	}

}

DWORD	CMetaImageDfx::GetAreaAverage(DWORD* pImage,int nLeftX,int nTopY,int nRightX,int nBottomY,int nStride) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nXDelta=nRightX-nLeftX;
	const int nYDelta=nBottomY-nTopY;

	if ((nXDelta<=0)||
		(nYDelta<=0)) {
		return 0x00000000;
	}

	DWORD* pCurrentImage=pImage+(nTopY*nWidth)+nLeftX;
	DWORD*const pImageEnd=(pCurrentImage+(nYDelta*nWidth)+nXDelta);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	while (pCurrentImage<pImageEnd) {

		DWORD*const pImageLineStart=pCurrentImage;
		DWORD*const pImageLineEnd=pCurrentImage+nXDelta;

		while (pCurrentImage<pImageLineEnd) {

			const DWORD ImageColour=*pCurrentImage;

			const DWORD nImageRed=(ImageColour>>SHIFT_RED)&0xff;
			const DWORD nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
			const DWORD nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nImageRed;
			nGreenTotal+=nImageGreen;
			nBlueTotal+=nImageBlue;

			nSampleCount+=1;

			pCurrentImage+=nStride;

		}

		pCurrentImage=pImageLineStart+(nStride*nWidth);

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	DWORD Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;

}

DWORD	CMetaImageDfx::ShrinkSourceImage(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	if (SubWidth>(float)(nWidth)) {
		SubWidth=(float)(nWidth);
	}

	if (SubHeight>(float)(nHeight)) {
		SubHeight=(float)(nHeight);
	}

	const float SourceYInc=(nHeight/SubHeight);
	const float SourceXInc=(nWidth/SubWidth);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	DWORD* pCurrentOutput=pOutput;

	float SourceY;
	for (SourceY=0.0f; SourceY<nHeight; SourceY+=SourceYInc) {

		DWORD* pOutputLineStart=pCurrentOutput;
		const int nTopY=static_cast<int>(SourceY);
		int nBottomY=static_cast<int>(SourceY+SourceYInc);
		nBottomY=GateInt(nBottomY,0,nHeight);

		float SourceX;
		for (SourceX=0.0f; SourceX<nWidth; SourceX+=SourceXInc) {

			const int nLeftX=static_cast<int>(SourceX);
			int nRightX=static_cast<int>(SourceX+SourceXInc);
			nRightX=GateInt(nRightX,0,nWidth);

			const DWORD OutputColour=
				GetAreaAverage(pSource,nLeftX,nTopY,nRightX,nBottomY,1);

			const DWORD nOutputRed=(OutputColour>>SHIFT_RED)&0xff;
			const DWORD nOutputGreen=(OutputColour>>SHIFT_GREEN)&0xff;
			const DWORD nOutputBlue=(OutputColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nOutputRed;
			nGreenTotal+=nOutputGreen;
			nBlueTotal+=nOutputBlue;

			nSampleCount+=1;

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	DWORD Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;

}

DWORD	CMetaImageDfx::ShrinkSourceImageFast(DWORD* pSource,DWORD* pOutput,float SubWidth,float SubHeight) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	if (SubWidth>(float)(nWidth)) {
		SubWidth=(float)(nWidth);
	}

	if (SubHeight>(float)(nHeight)) {
		SubHeight=(float)(nHeight);
	}

	const float SourceYInc=(nHeight/SubHeight);
	const float SourceXInc=(nWidth/SubWidth);

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;
	int nSampleCount=0;

	DWORD* pCurrentOutput=pOutput;

	float SourceY;
	for (SourceY=0.0f; SourceY<nHeight; SourceY+=SourceYInc) {

		DWORD* pOutputLineStart=pCurrentOutput;
		const int nTopY=static_cast<int>(SourceY);

		DWORD* pSourceLineStart=pSource+(nTopY*nWidth);

		float SourceX;
		for (SourceX=0.0f; SourceX<nWidth; SourceX+=SourceXInc) {

			const int nLeftX=static_cast<int>(SourceX);

			const DWORD OutputColour=*(pSourceLineStart+nLeftX);

			const DWORD nOutputRed=(OutputColour>>SHIFT_RED)&0xff;
			const DWORD nOutputGreen=(OutputColour>>SHIFT_GREEN)&0xff;
			const DWORD nOutputBlue=(OutputColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nOutputRed;
			nGreenTotal+=nOutputGreen;
			nBlueTotal+=nOutputBlue;

			nSampleCount+=1;

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

		}

		pCurrentOutput=pOutputLineStart+nWidth;

	}

	const int nAverageRed=(nRedTotal/nSampleCount);
	const int nAverageGreen=(nGreenTotal/nSampleCount);
	const int nAverageBlue=(nBlueTotal/nSampleCount);

	DWORD Average=
		(nAverageRed<<SHIFT_RED)|
		(nAverageGreen<<SHIFT_GREEN)|
		(nAverageBlue<<SHIFT_BLUE);

	return Average;

}

DWORD	CMetaImageDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CMetaImageDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CMetaImageDfx::GetInputState()
{
	return 0xffffffff;
}

void CMetaImageDfx::LockSubImagesData(void) {
	m_pSubImagesData=(char*)GETPTR(m_hSubImages);
}

void CMetaImageDfx::UnLockSubImagesData(void) {
	FREEPTR(m_hSubImages);
	m_pSubImagesData=NULL;
}

SSubImageHeader* CMetaImageDfx::GetSubImage(int nIndex) {

	if ((nIndex<0)||(nIndex>=m_nInputCount)||(m_pSubImagesData==NULL)) {
		return NULL;
	}

	char* pSubImageData=m_pSubImagesData;
	const int nSizeOfSubImage=
		sizeof(SSubImageHeader)+(m_nXRes*m_nYRes*sizeof(DWORD));

	return (SSubImageHeader*)(pSubImageData+(nIndex*nSizeOfSubImage));

}

SSubImageHeader* CMetaImageDfx::GetBestSubImage(DWORD TargetColour) {

	int nInputCount;
	if (m_bUseSourceAsSubImage) {
		nInputCount=m_nInputCount;
	} else {
		if (m_nInputCount>1) {
			nInputCount=(m_nInputCount-1);
		} else {
			nInputCount=1;
		}
	}

	const int nTargetRed=(TargetColour>>SHIFT_RED)&0xff;
	const int nTargetGreen=(TargetColour>>SHIFT_GREEN)&0xff;
	const int nTargetBlue=(TargetColour>>SHIFT_BLUE)&0xff;

	int nClosestDistance=0x7fffffff;
	SSubImageHeader* pResult=NULL;

	int nCount;
	for (nCount=0; nCount<nInputCount; ++nCount) {

		SSubImageHeader* pSubImage=GetSubImage(nCount);

		DWORD SubAverage=pSubImage->m_AverageColour;

		const int nSubRed=(SubAverage>>SHIFT_RED)&0xff;
		const int nSubGreen=(SubAverage>>SHIFT_GREEN)&0xff;
		const int nSubBlue=(SubAverage>>SHIFT_BLUE)&0xff;

		const int nDeltaRed=(nSubRed-nTargetRed);
		const int nDeltaGreen=(nSubGreen-nTargetGreen);
		const int nDeltaBlue=(nSubBlue-nTargetBlue);

		const int nDistSquared=
			(nDeltaRed*nDeltaRed)+
			(nDeltaRed*nDeltaRed)+
			(nDeltaRed*nDeltaRed);

		if (nDistSquared<nClosestDistance) {

			nClosestDistance=nDistSquared;
			pResult=pSubImage;

		}

	}

	return pResult;

}
