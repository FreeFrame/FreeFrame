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

/*
	This module is a terrible hack to make adapting my plugins to the
	TranZendenT api easier

*/

#include<windows.h>
#include<ddraw.h>
#if 0
extern "C" {
	__declspec(dllexport) int 					Init(LPDIRECTDRAW7);
	__declspec(dllexport) int					UpdateWaveData(short *pwave,int *pspectrum);
	__declspec(dllexport) int 					Release(void);
	__declspec(dllexport) int 					Start(void);
	__declspec(dllexport) int 					Stop(void);
	__declspec(dllexport) int 					SetInputSurface(LPDIRECTDRAWSURFACE7,LPDIRECTDRAWSURFACE7);
	__declspec(dllexport) LPDIRECTDRAWSURFACE7	GetOutputSurface(void);
	__declspec(dllexport) int 					Effect(DWORD);
	__declspec(dllexport) int 					SetFader(int);
	__declspec(dllexport) int 					SetFaderEx(int);
	__declspec(dllexport) char *				GetName(void);
    __declspec(dllexport) DWORD					GetPluginType(void);
	__declspec(dllexport) int					SetBpm(int);
	__declspec(dllexport) int					SetBeatSync(int);
	__declspec(dllexport) int					SetSpeed(int);
};

typedef struct SPete_TZT_Data {
	LPDIRECTDRAW7			lpDd;
	LPDIRECTDRAWSURFACE7	lpddsIn;
	LPDIRECTDRAWSURFACE7	lpddsPlayer;
	LPDIRECTDRAWSURFACE7	lpddsOutput;
	int						nWhichParameterSelected;
} SPete_TZT_Data;

bool g_bIsInitialised=false;

LPDIRECTDRAWSURFACE7 CreateSurface();

#define PLUGINTYPE_INVALID		0
#define PLUGINTYPE_COMPOSITION	1
#define PLUGINTYPE_EFFECT		2
#define PLUGINTYPE_GENERATOR	3

const int TZT_Width=320;
const int TZT_Height=240;

SPete_TZT_Data g_TZTInstanceData;
SPete_GlobalData g_TZTGlobalData;

__declspec(dllexport) char *GetName(){
	return (char*)&(g_PluginInfo.pluginName[0]);
}

__declspec(dllexport) DWORD GetPluginType(void){
	return PLUGINTYPE_EFFECT;
}

__declspec(dllexport) int Init(LPDIRECTDRAW7 lpdd){

//	if (!g_bIsInitialised) {

		ZeroMemory(&g_TZTInstanceData,sizeof(g_TZTInstanceData));
		g_TZTInstanceData.lpDd=lpdd;
		g_TZTInstanceData.lpddsOutput=CreateSurface();

		VideoInfoStruct FFVideoInfo;
		FFVideoInfo.frameWidth=TZT_Width;
		FFVideoInfo.frameHeight=TZT_Height;
		FFVideoInfo.bitDepth=0;

		initialise(&FFVideoInfo,&g_TZTGlobalData);

		SetFader(0);

//		g_bIsInitialised=true;
//
//	}

	return 1;
}

__declspec(dllexport) int UpdateWaveData(short *pwave,int *pspectrum){
	return 1;
}

__declspec(dllexport) int Release(void){

	if(g_TZTInstanceData.lpddsOutput){
		g_TZTInstanceData.lpddsOutput->Release();
		g_TZTInstanceData.lpddsOutput=NULL;
	}

	deInitialise(&g_TZTGlobalData);

	return 1;
}
__declspec(dllexport) int Start(void){
	return 1;
}

__declspec(dllexport) int Stop(void){
	return 1;
}

__declspec(dllexport) int SetInputSurface(LPDIRECTDRAWSURFACE7 in,LPDIRECTDRAWSURFACE7 player){
	g_TZTInstanceData.lpddsIn=in;
	g_TZTInstanceData.lpddsPlayer=player;
	return 1;
}

__declspec(dllexport) LPDIRECTDRAWSURFACE7 GetOutputSurface(void){
	return g_TZTInstanceData.lpddsOutput;
}

__declspec(dllexport) int SetFader(int x){
	g_TZTInstanceData.nWhichParameterSelected=x;
	return 1;
}

__declspec(dllexport) int SetFaderEx(int x){

	int nParamIndex=g_TZTInstanceData.nWhichParameterSelected;

	const int nNumParameters=getNumParameters(&g_TZTGlobalData);

	if (nParamIndex>=nNumParameters) {
		nParamIndex=(nNumParameters-1);
	}

	setParameter(nParamIndex,(x/10.0f),&g_TZTGlobalData);

	return 1;
}

__declspec(dllexport) int Effect(DWORD dwmsec){
	DDSURFACEDESC2 Ddsd={sizeof(DDSURFACEDESC2)};

#ifndef PLUGIN_INPUT_COUNT

	g_TZTInstanceData.lpddsPlayer->Lock(NULL,&Ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
	const int nSourcePitch=Ddsd.lPitch >> 1;
	PETE_PIXELDATA16* pSourceStart=(WORD*)Ddsd.lpSurface;

	g_TZTInstanceData.lpddsOutput->Lock(NULL,&Ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
	const int nDestPitch=Ddsd.lPitch >> 1;
	PETE_PIXELDATA16* pDestStart=(WORD*)Ddsd.lpSurface;

	PETE_PIXELDATA32* pSourceBuffer=g_TZTGlobalData.m_pSourceBuffer;
	PETE_PIXELDATA32* pOutputBuffer=g_TZTGlobalData.m_pOutputBuffer;

	int nY;
	for (nY=0; nY<TZT_Height; nY+=1) {

		PETE_PIXELDATA16* pSourceLineStart=pSourceStart+(nY*nSourcePitch);
		PETE_PIXELDATA32* pSourceBufferLineStart=pSourceBuffer+(((TZT_Height-1)-nY)*TZT_Width);
//		PETE_PIXELDATA32* pSourceBufferLineStart=pSourceBuffer+(nY*TZT_Width);

		Pete_CopyAndConvert16Bit565To32Bit(pSourceLineStart,pSourceBufferLineStart,TZT_Width);

	}

	PLUGIN_RENDER(&g_TZTGlobalData.m_InstanceData,&g_TZTGlobalData.m_Settings,pSourceBuffer,pOutputBuffer);

	for (nY=0; nY<TZT_Height; nY+=1) {

		PETE_PIXELDATA32* pOutputBufferLineStart=pOutputBuffer+(((TZT_Height-1)-nY)*TZT_Width);
//		PETE_PIXELDATA32* pOutputBufferLineStart=pOutputBuffer+(nY*TZT_Width);

		PETE_PIXELDATA16* pDestLineStart=pDestStart+(nY*nDestPitch);

		Pete_CopyAndConvert32BitTo16Bit565(pOutputBufferLineStart,pDestLineStart,TZT_Width);

	}

	g_TZTInstanceData.lpddsPlayer->Unlock(NULL);
	g_TZTInstanceData.lpddsOutput->Unlock(NULL);

#endif // PLUGIN_INPUT_COUNT

	return 1;
}

LPDIRECTDRAWSURFACE7 CreateSurface(){
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 lpdds;
	ZeroMemory(&ddsd,sizeof(DDSURFACEDESC2));
    ddsd.dwSize  =sizeof(DDSURFACEDESC2);
    ddsd.dwFlags =DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.dwWidth =320;
    ddsd.dwHeight=240;
    ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	g_TZTInstanceData.lpDd->CreateSurface(&ddsd,&lpdds,NULL);
	return lpdds;
}

#endif