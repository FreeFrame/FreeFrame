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

// FreeframeHost.cpp: implementation of the FreeframeHost class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreeframeMFC.h"
#include "FreeframeHost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Parameter
//////////////////////////////////////////////////////////////////////
Parameter::Parameter()
{
	defaultValue = 0;
	value = 0;
	
	sprintf(displayValue, "Display_Value___");
	sprintf(name, "Name____________");
}

Parameter::~Parameter()
{
}

//////////////////////////////////////////////////////////////////////
// Freeframe Host Construction/Destruction
//////////////////////////////////////////////////////////////////////

FreeframeHost::FreeframeHost()
{
	hinstLib = 0;
	plugMain = 0;
	pPlugInfo = 0;
	isInitialised = false;
	numParameters = 0;
	parameters = 0;
}

FreeframeHost::~FreeframeHost()
{
	if (hinstLib) {
		unloadPlugin();
	}
	if (parameters) {
	// de-allocate memory for parameternames strings
		for (DWORD n=0; n<numParameters; n++) {
			delete parameters[n];
		}
		//de-allocate memory for parameternames array
		delete parameters;
	}
}



void FreeframeHost::loadPlugin(char* pluginFile)
{
   BOOL fRunTimeLinkSuccess = FALSE; 

    // Get a handle to the DLL module.
   CString plugFileFull("C:\\stuff\\projects\\FreeFrame\\FreeframeMFC\\plugins\\");
 
   plugFileFull += pluginFile;

    hinstLib = LoadLibrary(plugFileFull.GetBuffer(1)); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
        plugMain = (FF_Main_FuncPtr) GetProcAddress(hinstLib, "plugMain"); 
 
    } 
	else MessageBox(0, "Couldn't load Plugin", "Freeframe Error", MB_OK);
}

void FreeframeHost::unloadPlugin()
{
	//De-initialise if not already done
	if (isInitialised) {
		deInitialise();
		isInitialised = 0;
	}
  // free the DLL module
  BOOL fFreeResult;
  if (hinstLib) {
	  fFreeResult = FreeLibrary(hinstLib); 
	  hinstLib = 0;
  }

}

void FreeframeHost::getPlugInfo()
{
	pPlugInfo = (PlugInfoStruct*)(plugMain)(FF_GETINFO, 0, 0);
}

DWORD FreeframeHost::initialise(VideoInfoStruct* videoInfo)
{
	isInitialised = true;
	return (DWORD)(plugMain)(FF_INITIALISE, (void*)videoInfo, 0);
}

DWORD FreeframeHost::processFrame(LPVOID pFrame)
{
	return (DWORD)(plugMain)(FF_PROCESSFRAME, pFrame, 0);

}

void FreeframeHost::deInitialise()
{
	if (isInitialised){
		isInitialised = false;
		(plugMain)(FF_DEINITIALISE, 0, 0);
	}
}

void FreeframeHost::getNumParameters()
{
	numParameters = (DWORD)(plugMain)(FF_GETNUMPARAMETERS, 0, 0);
	//allocate memory for the parameterNames array
	parameters = new Parameter* [numParameters];
	for (DWORD n=0; n<numParameters; n++) {
		parameters[n] = new Parameter;
	}
}

void FreeframeHost::getParameterName(DWORD param)
{
	char *pName = (char *)(plugMain)(FF_GETPARAMETERNAME, (LPVOID)param, 0); 
	memcpy(parameters[param]->name, pName, 16);
}

void FreeframeHost::getParameterNames()
{
	for (DWORD n=0; n<numParameters; n++) {
		getParameterName(n);
	}
}

void FreeframeHost::getParameterDefault(DWORD param)
{
	parameters[param]->defaultValue = (float)(DWORD)(plugMain)(FF_GETPARAMETERDEFAULT, (LPVOID)param, 0);
}

void FreeframeHost::getParameterDefaults()
{
	for (DWORD n=0; n<numParameters; n++) {
		getParameterDefault(n);
	}
	
}

void FreeframeHost::getParameter(DWORD index)
{
	paramUnion uvalue;
	uvalue.pvalue = (plugMain)(FF_GETPARAMETER, (LPVOID)index, 0);
	parameters[index]->value = uvalue.fvalue;
}

void FreeframeHost::getParameters()
{
	for (DWORD n=0; n<numParameters; n++) {
		getParameter(n);
	}

}

void FreeframeHost::getParameterDisplay(DWORD index)
{
	char *pDisplay = (char *)(plugMain)(FF_GETPARAMETERDISPLAY, (LPVOID)index, 0); 
	memcpy(parameters[index]->displayValue, pDisplay, 16);
}

void FreeframeHost::getParameterDisplays()
{
	for (DWORD n=0; n<numParameters; n++) {
		getParameterDisplay(n);
	}
}

void FreeframeHost::setParameter(DWORD index, float value)
{
	if (index<numParameters) {
		SetParameterStruct paramStruct;
		paramStruct.index = index;
		paramStruct.value = value;
		parameters[index]->value = value;
		if ((plugMain)(FF_SETPARAMETER, &paramStruct, 0) != FF_SUCCESS) MessageBox(0,"setParameter error","",MB_OK); 
		LPVOID test = (plugMain)(FF_GETPARAMETER, (LPVOID)index, 0);
	}
}
