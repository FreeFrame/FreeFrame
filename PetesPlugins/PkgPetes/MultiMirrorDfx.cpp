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

#include "PeteHelpers.h"

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

class CMultiMirrorDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CMultiMirrorDfx();
	CMultiMirrorDfx();
	
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

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

};

BEGIN_INPUTSCREENS_DESC(CMultiMirrorDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CMultiMirrorDfx)

BEGIN_OUTPUTSCREENS_DESC(CMultiMirrorDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CMultiMirrorDfx)

BEGIN_EFFECT_DESC(CMultiMirrorDfx, 10203216861479924, PETE_VJO_VERSION , "Petes\\MultiMirror", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CMultiMirrorDfx) 

REGISTER_DFXFACTORY(CMultiMirrorDfx)

CMultiMirrorDfx::CMultiMirrorDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CMultiMirrorDfx);
}

CMultiMirrorDfx::~CMultiMirrorDfx()
{
	// do nothing
}

HBITMAP	CMultiMirrorDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CMultiMirrorDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CMultiMirrorDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CMultiMirrorDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CMultiMirrorDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CMultiMirrorDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	return TRUE;
}

BOOL	CMultiMirrorDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	WriteOutputFrame(pInputMem,pOutputMem);

	return TRUE;

}

void	CMultiMirrorDfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);
	
	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels = nWidth*nHeight;

	DWORD* pCurrentSource=pSource;
	DWORD* pCurrentOutput=pOutput;
	DWORD*const pSourceEnd=(pSource+nNumPixels);

	DWORD* pSourceVertFlip=pSourceEnd-(nWidth);

	while (pCurrentSource!=pSourceEnd) {
		
		DWORD*const pSourceLineEnd=pCurrentSource+nWidth;
		DWORD*const pSourceVertFlipLineEnd=pSourceVertFlip+nWidth;

		DWORD* pSourceHorzFlip=pSourceLineEnd-1;
		DWORD* pSourceVertHorzFlip=pSourceVertFlipLineEnd-1;

		while (pCurrentSource!=pSourceLineEnd) {

			DWORD SourceColour=*pCurrentSource;
			DWORD VertFlipColour=*pSourceVertFlip;
			DWORD HorzFlipColour=*pSourceHorzFlip;
			DWORD VertHorzFlipColour=*pSourceVertHorzFlip;

			SourceColour&=0xfcfcfcfc;
			VertFlipColour&=0xfcfcfcfc;
			HorzFlipColour&=0xfcfcfcfc;
			VertHorzFlipColour&=0xfcfcfcfc;

			DWORD OutputColour=
				SourceColour+
				VertFlipColour+
				HorzFlipColour+
				VertHorzFlipColour;

			OutputColour>>=2;

			*pCurrentOutput=OutputColour;

			pCurrentSource+=1;
			pSourceVertFlip+=1;
			pCurrentOutput+=1;
			pSourceHorzFlip-=1;
			pSourceVertHorzFlip-=1;

		}

		pSourceVertFlip-=(nWidth*2);

	}


}

DWORD	CMultiMirrorDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CMultiMirrorDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CMultiMirrorDfx::GetInputState()
{
	return 0xffffffff;
}

