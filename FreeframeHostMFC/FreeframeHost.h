/*

Copyright (c) 2003, Marcus Clements www.freeframe.org
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
// FreeframeHost.h: interface for the FreeframeHost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREEFRAMEHOST_H__B0FE5984_E14B_4E83_AC93_DA2162B17FD3__INCLUDED_)
#define AFX_FREEFRAMEHOST_H__B0FE5984_E14B_4E83_AC93_DA2162B17FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\CPLUGIN\FreeFrame.h"	// Added by ClassView

typedef union paramUnionTag {
	DWORD ivalue;
	float fvalue;
	VideoInfoStruct *VISvalue;
	PlugInfoStruct *PISvalue;
	char *svalue;
	LPVOID pvalue;
} paramUnion;

class Parameter {
public:
	Parameter();
	virtual ~Parameter();
	float value;
	float defaultValue;
	char displayValue[17];	// this is 16 char name + null terminator
	char name[17];			// this is 16 char name + null terminator
};


class FreeframeHost
{
public:
	void setParameter(DWORD index, float value);
	void getParameterDisplays();
	void getParameterDisplay(DWORD index);
	void getParameter(DWORD index);
	void getParameters();
	void getParameterDefaults();
	void getParameterDefault( DWORD index);
	void getParameterNames();
	void getParameterName(DWORD);
	Parameter ** parameters;
	DWORD numParameters;
	DWORD instance;
	void getNumParameters();
	bool isInitialised;
	void deInitialise();
	DWORD processFrame(LPVOID pFrame);
	DWORD initialise(VideoInfoStruct* videoInfo);
	bool isInstantiated;
	DWORD instantiate(VideoInfoStruct* videoInfo);
	DWORD deInstantiate();
	void getPlugInfo();
	void unloadPlugin();
	PlugInfoStruct *pPlugInfo;
	FF_Main_FuncPtr plugMain;
	HINSTANCE hinstLib;
	void loadPlugin(char* pluginFile);
	FreeframeHost();
	virtual ~FreeframeHost();

};

#endif // !defined(AFX_FREEFRAMEHOST_H__B0FE5984_E14B_4E83_AC93_DA2162B17FD3__INCLUDED_)
