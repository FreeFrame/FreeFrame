/* Copyright (C) 2002 Pete Warden

Effect plugin for VisualJockey

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "stdafx.h"

#include <math.h>
#include "mmintrin.h"

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

class CRadialBlurDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CRadialBlurDfx();
	CRadialBlurDfx();
	
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

	float	m_LayerScale;
	int		m_nLayerScaleID;

	DWORD   m_nMemUsage;

};

BEGIN_INPUTSCREENS_DESC(CRadialBlurDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CRadialBlurDfx)

BEGIN_OUTPUTSCREENS_DESC(CRadialBlurDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CRadialBlurDfx)

BEGIN_EFFECT_DESC(CRadialBlurDfx, 10203216861479969, PETE_VJO_VERSION , "Petes\\RadialBlur", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CRadialBlurDfx) 

REGISTER_DFXFACTORY(CRadialBlurDfx)

static inline int Gate(int nValue,int nMin,int nMax) {

	if (nValue<nMin) {
		return nMin;
	} else if (nValue>nMax) {
		return nMax;
	} else {
		return nValue;
	}
}

CRadialBlurDfx::CRadialBlurDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_LayerScale=0.01f;

	m_nMemUsage = sizeof(CRadialBlurDfx);
}

CRadialBlurDfx::~CRadialBlurDfx()
{
	// do nothing
}

HBITMAP	CRadialBlurDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CRadialBlurDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CRadialBlurDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CRadialBlurDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CRadialBlurDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nLayerScaleID=
		RegisterFloat(m_pEngine,&m_LayerScale,"Scale",0.0f,0.1f);

	return TRUE;
}

BOOL	CRadialBlurDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	return TRUE;
}

BOOL	CRadialBlurDfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	WriteOutputFrame(pInputMem,pOutputMem);

	return TRUE;

}

static inline int GetLuminance(const DWORD inColour) {
	const int nRed=(inColour&(0xff<<SHIFT_RED))>>SHIFT_RED;
	const int nGreen=(inColour&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
	const int nBlue=(inColour&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

	const int nLuminance = 
		((90 * nRed)+
		(115 * nGreen)+
		(51 * nBlue));

	return (nLuminance>>8);
}

void	CRadialBlurDfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput) {

	const int nFixedShift=16;
	const int nFixedMult=(1<<nFixedShift);

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	const int nLayersCountShift=4;
	const __m64 nLayersCountShiftMMX=_m_from_int(nLayersCountShift);
	const int nLayersCount=(1<<nLayersCountShift);
	const int nLayerScaleIncrement=static_cast<int>(m_LayerScale*nFixedMult);
	
	DWORD* pCurrentSource=pSource;
	DWORD* pCurrentOutput=pOutput;
	const DWORD* pOutputEnd=(pOutput+nNumPixels);

	const DWORD* pSourceCentre=pSource+(nHalfHeight*nWidth)+(nHalfWidth);

	const __m64 ZeroReg=_m_from_int(0);
	__asm {

		pxor		mm0,mm0

	}

	int nY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {
		
		const DWORD* pOutputLineStart=pCurrentOutput;
		const DWORD* pOutputLineEnd=pCurrentOutput+nWidth;

		int nX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			__asm {

				pxor		mm1,mm1

				mov			eax,16
				mov			esi,(1<<16)
				mov			edi,nLayerScaleIncrement

//LoopStart:
//				mov			eax,pSourceCentre

#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"
#include "innerloop.c"

//					sub			eax,1
//					cmp			eax,0
//					jnz			LoopStart
			
				psrlw		mm1,4//nLayersCountShift

				packuswb	mm1,mm0

				mov			esi,pCurrentOutput
				movq		[esi],mm1

			}

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

	_m_empty();

}

DWORD	CRadialBlurDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CRadialBlurDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CRadialBlurDfx::GetInputState()
{
	return 0xffffffff;
}

