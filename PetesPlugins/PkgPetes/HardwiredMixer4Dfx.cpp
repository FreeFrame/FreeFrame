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

class CHardWiredMixer4Dfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CHardWiredMixer4Dfx();
	CHardWiredMixer4Dfx();
	
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

	void	SetupTables(void);
	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	int		m_nInputIndex;

	int		m_nFrameCount;

};

BEGIN_INPUTSCREENS_DESC(CHardWiredMixer4Dfx)
{{-1,-1}, "RGB 0"},
{{-1,-1}, "RGB 1"},
{{-1,-1}, "RGB 2"},
{{-1,-1}, "RGB 3"},
END_INPUTSCREENS_DESC(CHardWiredMixer4Dfx)

BEGIN_OUTPUTSCREENS_DESC(CHardWiredMixer4Dfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CHardWiredMixer4Dfx)

BEGIN_EFFECT_DESC(CHardWiredMixer4Dfx, 10203216861479977, PETE_VJO_VERSION , "Petes\\HardWiredMixer4", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CHardWiredMixer4Dfx) 

REGISTER_DFXFACTORY(CHardWiredMixer4Dfx)

const int cnNumberOfInputs=4;
const int cnInputStartChar='W';
const int cnControlKey=VK_CONTROL;

CHardWiredMixer4Dfx::CHardWiredMixer4Dfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nInputIndex=0;

	m_nFrameCount=0;

	m_nMemUsage = sizeof(CHardWiredMixer4Dfx);
}

CHardWiredMixer4Dfx::~CHardWiredMixer4Dfx()
{
	// do nothing
}

HBITMAP	CHardWiredMixer4Dfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CHardWiredMixer4Dfx::SetupDialog()
{
	VJO_MANAGE_STATE();

	m_pEngine->ConfigureVariable(FXVARALLVARS); 

	return FALSE;

}

void	CHardWiredMixer4Dfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CHardWiredMixer4Dfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;	
}

BOOL	CHardWiredMixer4Dfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CHardWiredMixer4Dfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	return TRUE;
}

BOOL	CHardWiredMixer4Dfx::Render(CScreen **ppInput, CScreen *pOutput)
{
	
	DWORD* pOutputMem = (DWORD*)(pOutput->GetBuffer());
	DWORD* pInputMem = (DWORD*)(ppInput[m_nInputIndex]->GetBuffer());

	WriteOutputFrame(pInputMem,pOutputMem);

	if ((GetAsyncKeyState(cnControlKey)&(1<<15))==0) {

		int nNumberOfKeysPressed=0;

		bool bInputKeyPressed[cnNumberOfInputs];
		
		int nCount;
		for (nCount=0; nCount<cnNumberOfInputs; ++nCount) {

			bInputKeyPressed[nCount]=
				((GetAsyncKeyState((nCount+cnInputStartChar))&(1<<15))!=0);

			if (bInputKeyPressed[nCount]) {
				nNumberOfKeysPressed+=1;
			}

		}

		if (nNumberOfKeysPressed>0) {

			const int nWhichToChoose=
				(m_nFrameCount%nNumberOfKeysPressed);

			int nCurrentIndex=0;
			
			for (nCount=0; nCount<cnNumberOfInputs; ++nCount) {

				if (bInputKeyPressed[nCount]) {

					if (nCurrentIndex==nWhichToChoose) {
						m_nInputIndex=nCount;
					}

					nCurrentIndex+=1;

				}
			}

		}
	}

	m_nFrameCount+=1;

	return TRUE;

}

void	CHardWiredMixer4Dfx::WriteOutputFrame(DWORD* pSource, DWORD* pOutput) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nNumPixels = nWidth*nHeight;

	const int nNumBytes=nNumPixels*sizeof(DWORD);

	memcpy(pOutput,pSource,nNumBytes);

}

DWORD	CHardWiredMixer4Dfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CHardWiredMixer4Dfx::ConfirmRenderSource(int i)
{
	return (i==m_nInputIndex);
}

DWORD CHardWiredMixer4Dfx::GetInputState()
{
	return (0xffffffff);
}

