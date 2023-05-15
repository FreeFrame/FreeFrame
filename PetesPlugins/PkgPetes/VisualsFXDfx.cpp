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
#include "VisualsFX.h"

#include "PeteHelpers.h"

class CVisualsFXDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CVisualsFXDfx();
	CVisualsFXDfx();

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

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	OPENFILENAME m_OpenFileName;
	char	m_pVisualsFXFileName[MAX_PATH];
	VisualsFX_FunctionTable* m_pFunctionTable;

	// Pete- This is an anomaly, using the VJo HGLOBAL stuff for memory
	// management is preferred, but in this case I have to register the
	// addresses of the parameters with VJo, so having an area of
	// memory that may be moved around won't work, so I fall back to
	// new/delete
	float*	m_pVisualsFXParameters;

};

BEGIN_INPUTSCREENS_DESC(CVisualsFXDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CVisualsFXDfx)

BEGIN_OUTPUTSCREENS_DESC(CVisualsFXDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CVisualsFXDfx)

BEGIN_EFFECT_DESC(CVisualsFXDfx, 10203216861479997, PETE_VJO_VERSION , "Petes\\VisualsFX", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CVisualsFXDfx)

REGISTER_DFXFACTORY(CVisualsFXDfx)

CVisualsFXDfx::CVisualsFXDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CVisualsFXDfx);

    ZeroMemory(&m_OpenFileName, sizeof(m_OpenFileName));
    m_OpenFileName.lStructSize = sizeof(OPENFILENAME);
    m_OpenFileName.hwndOwner = NULL;
    m_OpenFileName.lpstrFilter = "VisualsFX Modules (*.dll)\0"
        "*.dll\0";
    m_OpenFileName.nFilterIndex = 1;
    m_OpenFileName.lpstrFile = &(m_pVisualsFXFileName[0]);
    m_OpenFileName.nMaxFile = MAX_PATH;
    m_OpenFileName.lpstrTitle = "Load VisualsFX Effect";
    m_OpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    m_OpenFileName.lpstrDefExt = "dll";

	strcpy(m_pVisualsFXFileName,"");

	m_pFunctionTable=NULL;
	m_pVisualsFXParameters=NULL;

}

CVisualsFXDfx::~CVisualsFXDfx()
{

	if (m_pFunctionTable!=NULL) {
		m_pFunctionTable->deInitialise();
	}

	if (m_pVisualsFXParameters!=NULL) {
		delete m_pVisualsFXParameters;
	}

}

HBITMAP	CVisualsFXDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

void	CVisualsFXDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CVisualsFXDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CVisualsFXDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CVisualsFXDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

    if (!GetOpenFileName(&m_OpenFileName)) return FALSE;

	return TRUE;
}

BOOL	CVisualsFXDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	m_pFunctionTable=NULL;

	HMODULE hVisualsFXModule=LoadLibrary(&m_pVisualsFXFileName[0]);

	if (hVisualsFXModule!=INVALID_HANDLE_VALUE) {

		VisualsFX_Main_FuncPtr pVisualsFXMain=
			(VisualsFX_Main_FuncPtr)
			GetProcAddress(
			hVisualsFXModule,
			"VisualsFX_Main");

		if (pVisualsFXMain!=NULL) {

			m_pFunctionTable=pVisualsFXMain();

		}

	}

	if (m_pFunctionTable!=NULL) {

		videoInfoStruct VideoInfo;

		VideoInfo.frameWidth=m_nXRes;
		VideoInfo.frameHeight=m_nYRes;

		m_pFunctionTable->initialise(&VideoInfo);

		const int nNumParameters=m_pFunctionTable->getNumParameters();

		if (m_pVisualsFXParameters!=NULL) {
			delete m_pVisualsFXParameters;
		}
		m_pVisualsFXParameters=new float[nNumParameters];

	}

	if (m_pFunctionTable!=NULL) {

		const int nNumParameters=m_pFunctionTable->getNumParameters();

		float* pParameterData=m_pVisualsFXParameters;

		int nCount;
		for (nCount=0; nCount<nNumParameters; nCount+=1) {

			char* pName=m_pFunctionTable->getParameterName(nCount);
			if (pName==NULL) {
				pName="<NULL>";
			}

			pParameterData[nCount]=m_pFunctionTable->getParameterDefault(nCount);

			RegisterFloat(m_pEngine,&(pParameterData[nCount]),pName,0.0f,100.0f);

		}

	}

	return TRUE;
}

BOOL	CVisualsFXDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	if (m_pFunctionTable==NULL) {
		return FALSE;
	}

	const int nNumParameters=m_pFunctionTable->getNumParameters();

	float* pParameterData=m_pVisualsFXParameters;

	int nCount;
	for (nCount=0; nCount<nNumParameters; nCount+=1) {

		m_pFunctionTable->setParameter(nCount,pParameterData[nCount]);

	}

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	const int nNumPixels=m_nXRes*m_nYRes;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	memcpy(pOutputMem,pInputMem,nNumBytes);

	frameInfoStruct frameInfo;
	frameInfo.pFrame=pOutputMem;

	m_pFunctionTable->processFrame(&frameInfo);

	return TRUE;

}

DWORD	CVisualsFXDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CVisualsFXDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CVisualsFXDfx::GetInputState()
{
	return 0xffffffff;
}
