/*

Copyright (c) 2002, Pete Warden www.freeframe.org
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of FreeFrame nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "stdafx.h"

#include <math.h>

#include "pkgSAMPLES.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"
#include "FreeFrame.h"

#include "PeteHelpers.h"

class CFreeFrameDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CFreeFrameDfx();
	CFreeFrameDfx();

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
	char	m_pFreeFrameFileName[MAX_PATH];
	FF_Main_FuncPtr m_pFreeFrameMain;
	HMODULE m_hFreeFrameModule;

	// Pete- This is an anomaly, using the VJo HGLOBAL stuff for memory
	// management is preferred, but in this case I have to register the
	// addresses of the parameters with VJo, so having an area of
	// memory that may be moved around won't work, so I fall back to
	// new/delete
	float*	m_pFreeFrameParameters;
	VideoInfoStruct m_VideoInfo;
	U32 m_InstanceCookie;

};

BEGIN_INPUTSCREENS_DESC(CFreeFrameDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CFreeFrameDfx)

BEGIN_OUTPUTSCREENS_DESC(CFreeFrameDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CFreeFrameDfx)

BEGIN_EFFECT_DESC(CFreeFrameDfx, 10203216861479997, 0 , "Petes\\FreeFrame", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CFreeFrameDfx)

REGISTER_DFXFACTORY(CFreeFrameDfx)

CFreeFrameDfx::CFreeFrameDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;

	m_nMemUsage = sizeof(CFreeFrameDfx);

    ZeroMemory(&m_OpenFileName, sizeof(m_OpenFileName));
    m_OpenFileName.lStructSize = sizeof(OPENFILENAME);
    m_OpenFileName.hwndOwner = NULL;
    m_OpenFileName.lpstrFilter = "FreeFrame Modules (*.dll)\0"
        "*.dll\0";
    m_OpenFileName.nFilterIndex = 1;
    m_OpenFileName.lpstrFile = &(m_pFreeFrameFileName[0]);
    m_OpenFileName.nMaxFile = MAX_PATH;
    m_OpenFileName.lpstrTitle = "Load FreeFrame Effect";
    m_OpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    m_OpenFileName.lpstrDefExt = "dll";

	strcpy(m_pFreeFrameFileName,"");

	m_pFreeFrameMain=NULL;
	m_pFreeFrameParameters=NULL;
	m_hFreeFrameModule=NULL;

}

CFreeFrameDfx::~CFreeFrameDfx()
{

	if (m_pFreeFrameMain!=NULL) {
		m_pFreeFrameMain(FF_DEINITIALISE,NULL,0);
	}

	if (m_pFreeFrameParameters!=NULL) {
		delete m_pFreeFrameParameters;
	}

}

HBITMAP	CFreeFrameDfx::GetLabelBitmap()
{
	return (HBITMAP)HBITMAP_LAYERS;
}

void	CFreeFrameDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

}


BOOL	CFreeFrameDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	return TRUE;
}

BOOL	CFreeFrameDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	return TRUE;
}

BOOL	CFreeFrameDfx::SetupDialog()
{
	VJO_MANAGE_STATE();

    if (!GetOpenFileName(&m_OpenFileName)) return FALSE;

	return TRUE;
}

BOOL	CFreeFrameDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	if (m_pFreeFrameMain!=NULL) {
		m_pFreeFrameMain(FF_DEINSTANTIATE,NULL,m_InstanceCookie);
		m_pFreeFrameMain(FF_DEINITIALISE,NULL,0);
		m_pFreeFrameMain=NULL;
	}

	if (m_hFreeFrameModule!=NULL) {
		FreeLibrary(m_hFreeFrameModule);
		m_hFreeFrameModule=NULL;
	}

	m_hFreeFrameModule=LoadLibrary(&m_pFreeFrameFileName[0]);

	if (m_hFreeFrameModule!=NULL) {

		FF_Main_FuncPtr pFreeFrameMain=
			(FF_Main_FuncPtr)
			GetProcAddress(
			m_hFreeFrameModule,
			"plugMain");

		if (pFreeFrameMain!=NULL) {
			m_pFreeFrameMain=pFreeFrameMain;
		}

	}

	if (m_pFreeFrameMain!=NULL) {

		m_VideoInfo.frameWidth=m_nXRes;
		m_VideoInfo.frameHeight=m_nYRes;

		if ((DWORD)m_pFreeFrameMain(FF_GETPLUGINCAPS,(void*)2,0)==FF_TRUE) {
			m_VideoInfo.bitDepth=2;
		} else if ((DWORD)m_pFreeFrameMain(FF_GETPLUGINCAPS,(void*)1,0)==FF_TRUE) {
			m_VideoInfo.bitDepth=1;
		} else if ((DWORD)m_pFreeFrameMain(FF_GETPLUGINCAPS,(void*)0,0)==FF_TRUE) {
			m_VideoInfo.bitDepth=0;
		} else {
			ASSERT(FALSE);
			m_VideoInfo.bitDepth=2;
		}

		m_pFreeFrameMain(FF_INITIALISE,&m_VideoInfo,0);
		const int nNumParameters=(int)m_pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);

		if (m_pFreeFrameParameters!=NULL) {
			free(m_pFreeFrameParameters);
		}
		m_pFreeFrameParameters=(float*)malloc(nNumParameters*sizeof(float));

		int nCount;
		for (nCount=0; nCount<nNumParameters; nCount+=1) {

			static void* pCastCount;
			pCastCount=reinterpret_cast<void*>(nCount);

			char* pName=(char*)(m_pFreeFrameMain(FF_GETPARAMETERNAME,pCastCount,0));

			bool bNullCharFound=false;
			int nCharCount;
			for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
				bNullCharFound=(pName[nCharCount]=='\0');
			}
			if (!bNullCharFound) {
				pName[15]='\0'; // Pete- Getting unterminated strings returned
			}

			if (pName==NULL) {
				pName="<NULL>";
			}

			static U32 ResultAsU32;
			ResultAsU32=(U32)m_pFreeFrameMain(FF_GETPARAMETERDEFAULT,pCastCount,0);
			m_pFreeFrameParameters[nCount]=*((float*)&ResultAsU32);

			static U32 Type;
			Type=(U32)m_pFreeFrameMain(FF_GETPARAMETERTYPE,pCastCount,0);

			if (Type==FF_TYPE_BOOLEAN) {

				RegisterBool(m_pEngine,&(m_pFreeFrameParameters[nCount]),pName);

			} else {

				RegisterFloat(m_pEngine,&(m_pFreeFrameParameters[nCount]),pName,0.0f,1.0f);

			}

		}

		m_InstanceCookie=(U32)(m_pFreeFrameMain(FF_INSTANTIATE,&m_VideoInfo,0));

	}

	return TRUE;
}

BOOL	CFreeFrameDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	if (m_pFreeFrameMain==NULL) {
		return FALSE;
	}

	const int nNumParameters=(int)m_pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);

	float* pParameterData=m_pFreeFrameParameters;

	int nCount;
	for (nCount=0; nCount<nNumParameters; nCount+=1) {

		void* pCastCount=(void*)(nCount);

		static U32 Type;
		Type=(U32)m_pFreeFrameMain(FF_GETPARAMETERTYPE,pCastCount,0);

		SetParameterStruct ArgStruct;
			ArgStruct.index=nCount;

		if (Type!=FF_TYPE_BOOLEAN) {

			ArgStruct.value=pParameterData[nCount];

		} else {

			BOOL bIsOn=*((BOOL*)(&pParameterData[nCount]));

			if (bIsOn==TRUE) {
				ArgStruct.value=1.0f;
			} else {
				ArgStruct.value=0.0f;
			}

		}

		m_pFreeFrameMain(FF_SETPARAMETER,&ArgStruct,m_InstanceCookie);

	}

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();
	DWORD** ppInputMemArray = &pInputMem;

	const int nNumPixels=m_nXRes*m_nYRes;
	const int nNumBytes=(nNumPixels*sizeof(U32));

//	switch (m_VideoInfo.bitDepth) {
//
//		case 2: {
//			memcpy(pOutputMem,pInputMem,nNumBytes);
//			m_pFreeFrameMain(FF_PROCESSFRAME,pOutputMem,m_InstanceCookie);
//		}break;
//
//		case 1: {
//			Pete_CopyAndConvert32BitTo24Bit(pInputMem,pOutputMem,nNumPixels);
//			m_pFreeFrameMain(FF_PROCESSFRAME,pOutputMem,0);
//			Pete_InPlaceConvert24BitTo32Bit(pOutputMem,nNumPixels);
//		}break;
//
//		case 0: {
//			memcpy(pOutputMem,pInputMem,nNumBytes);
//			m_pFreeFrameMain(FF_PROCESSFRAME,pOutputMem,m_InstanceCookie);
//		}break;
//
//		default: {
//			ASSERT(FALSE);
//		}break;
//
//	}

	ProcessFrameCopyStruct PFCData;

	PFCData.numInputFrames=1;
	PFCData.InputFrames=(void**)ppInputMemArray;
	PFCData.OutputFrame=pOutputMem;

	m_pFreeFrameMain(FF_PROCESSFRAMECOPY,(void*)(&PFCData),m_InstanceCookie);

	return TRUE;

}

DWORD	CFreeFrameDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CFreeFrameDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CFreeFrameDfx::GetInputState()
{
	return 0xffffffff;
}

