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

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

class CLaplacianDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CLaplacianDfx();
	CLaplacianDfx();

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

BEGIN_INPUTSCREENS_DESC(CLaplacianDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CLaplacianDfx)

BEGIN_OUTPUTSCREENS_DESC(CLaplacianDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CLaplacianDfx)

BEGIN_EFFECT_DESC(CLaplacianDfx, 10203216861479923, PETE_VJO_VERSION , "Petes\\Laplacian", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CLaplacianDfx)

REGISTER_DFXFACTORY(CLaplacianDfx)

static inline int Gate(int nValue,int nMin,int nMax) {

	if (nValue<nMin) {
		return nMin;
	} else if (nValue>nMax) {
		return nMax;
	} else {
		return nValue;
	}
}

CLaplacianDfx::CLaplacianDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CLaplacianDfx);
}

CLaplacianDfx::~CLaplacianDfx()
{
	// do nothing
}

HBITMAP	CLaplacianDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CLaplacianDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	return FALSE;

}

void	CLaplacianDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CLaplacianDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CLaplacianDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CLaplacianDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;


	return TRUE;
}

BOOL	CLaplacianDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	WriteOutputFrame(pInputMem,pOutputMem);

	return TRUE;

}

void	CLaplacianDfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels = nWidth*nHeight;

	DWORD* pCurrentSource=pSource;
	DWORD* pCurrentOutput=pOutput;
	const DWORD* pSourceEnd=(pSource+nNumPixels);

	int nY=0;
	while (pCurrentSource!=pSourceEnd) {

		const DWORD* pSourceLineStart=pCurrentSource;
		const DWORD* pSourceLineEnd=pCurrentSource+nWidth;

		const int nKernelWidth=3;
		const int nKernelHeight=3;

		int nX=0;
		while (pCurrentSource!=pSourceLineEnd) {

			int nRedRegion[3][3];
			int nGreenRegion[3][3];
			int nBlueRegion[3][3];

			int nOffsetY;
			for (nOffsetY=0; nOffsetY<nKernelHeight; nOffsetY+=1) {

				int nCurrentY=(nY+(nOffsetY-(nKernelHeight/2)));
				nCurrentY=Gate(nCurrentY,0,nHeight);

				int nOffsetX;
				for (nOffsetX=0; nOffsetX<nKernelWidth; nOffsetX+=1) {

					int nCurrentX=(nX+(nOffsetX-(nKernelWidth/2)));
					nCurrentX=Gate(nCurrentX,0,nWidth);

					DWORD* pCurrentKernel=(pSource+(nCurrentY*nWidth)+nCurrentX);

					DWORD KernelColour=(*pCurrentKernel);

					nRedRegion[nOffsetY][nOffsetX]=(KernelColour>>SHIFT_RED)&0xff;
					nGreenRegion[nOffsetY][nOffsetX]=(KernelColour>>SHIFT_GREEN)&0xff;
					nBlueRegion[nOffsetY][nOffsetX]=(KernelColour>>SHIFT_BLUE)&0xff;

				}

			}

			int nRedMinimum=255;
			int nGreenMinimum=255;
			int nBlueMinimum=255;

			int nRedMaximum=0;
			int nGreenMaximum=0;
			int nBlueMaximum=0;

			for (nOffsetY=0; nOffsetY<nKernelHeight; nOffsetY+=1) {

				int nCurrentY=(nY+(nOffsetY-(nKernelHeight/2)));
				nCurrentY=Gate(nCurrentY,0,nHeight);

				int nOffsetX;
				for (nOffsetX=0; nOffsetX<nKernelWidth; nOffsetX+=1) {

					const int nKernelRed=nRedRegion[nOffsetY][nOffsetX];
					const int nKernelGreen=nGreenRegion[nOffsetY][nOffsetX];
					const int nKernelBlue=nBlueRegion[nOffsetY][nOffsetX];

					if (nKernelRed<nRedMinimum) {
						nRedMinimum=nKernelRed;
					}

					if (nKernelRed>nRedMaximum) {
						nRedMaximum=nKernelRed;
					}

					if (nKernelGreen<nGreenMinimum) {
						nGreenMinimum=nKernelGreen;
					}

					if (nKernelGreen>nGreenMaximum) {
						nGreenMaximum=nKernelGreen;
					}

					if (nKernelBlue<nBlueMinimum) {
						nBlueMinimum=nKernelBlue;
					}

					if (nKernelBlue>nBlueMaximum) {
						nBlueMaximum=nKernelBlue;
					}

				}

			}

			const DWORD CurrentColour=(*pCurrentSource);

			const int nCurrentRed=(CurrentColour>>SHIFT_RED)&0xff;
			const int nCurrentGreen=(CurrentColour>>SHIFT_GREEN)&0xff;
			const int nCurrentBlue=(CurrentColour>>SHIFT_BLUE)&0xff;

			int nOutputRed=(nRedMaximum+nRedMinimum);
			nOutputRed-=(2*nCurrentRed);
			nOutputRed=Gate(nOutputRed,0,255);

			int nOutputGreen=(nGreenMaximum+nGreenMinimum);
			nOutputGreen-=(2*nCurrentGreen);
			nOutputGreen=Gate(nOutputGreen,0,255);

			int nOutputBlue=(nBlueMaximum+nBlueMinimum);
			nOutputBlue-=(2*nCurrentBlue);
			nOutputBlue=Gate(nOutputBlue,0,255);

			DWORD OutputColour=(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE);

			*pCurrentOutput=OutputColour;

			pCurrentSource+=1;
			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}


}

DWORD	CLaplacianDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CLaplacianDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CLaplacianDfx::GetInputState()
{
	return 0xffffffff;
}

