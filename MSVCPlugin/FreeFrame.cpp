///////////////////////////////////////////////////////////////////////////////////
// FreeFrame.cpp
//
// FreeFrame Open Video Plugin Prototype
// ANSI C Version

// www.freeframe.org
// marcus@freeframe.org

/*

Copyright (c) 2002, Marcus Clements www.freeframe.org
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

///////////////////////////////////////////////////////////////////////////////////////////
//
// includes 
//

#include "FreeFrame.h"
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////
//
// implementation specific definitions
//

#define NUM_PARAMS 3

struct ParameterStruct {
	float value;
	float defaultValue;
	char displayValue[16];
	char name[16];
};

struct VideoPixel24bit {
	BYTE blue;
	BYTE green;
	BYTE red;
};

///////////////////////////////////////////////////////////////////////////////////////////
// Windows DLL Entry point
//
// notes: we may want to capture hModule as the instance of the host...

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
// plugMain - The one and only exposed function
// parameters: 
//	functionCode - tells the plugin which function is being called
//  pParam - 32-bit parameter or 32-bit pointer to parameter structure
//
// PLUGIN DEVELOPERS:  you shouldn't need to change this function
//
// All parameters are cast as 32-bit untyped pointers and cast to appropriate
// types here
// 
// All return values are cast to 32-bit untyped pointers here before return to 
// the host
//

#ifdef WIN32
 __declspec(dllexport) LPVOID __stdcall plugMain(DWORD functionCode, LPVOID pParam) 
#elif LINUX
   LPVOID plugMain(DWORD functionCode,LPVOID pParam)
#endif	
{
	switch(functionCode) {

	case FF_GETINFO:
		return (LPVOID) getInfo();
	
	case FF_INITIALISE:
		return (LPVOID) initialise( (VideoInfoStruct*) pParam);
	
	case FF_DEINITIALISE:
		return (LPVOID) deInitialise();
	
	case FF_GETNUMPARAMETERS:
		return (LPVOID) getNumParameters();

	case FF_GETPARAMETERNAME:
		return (LPVOID) getParameterName( (DWORD) pParam );
	
	case FF_GETPARAMETERDEFAULT:
		return (LPVOID)(DWORD) getParameterDefault( (DWORD) pParam );
	
	case FF_GETPARAMETERDISPLAY:
		return  (LPVOID) getParameterDisplay( (DWORD) pParam );
	
	// parameters are passed in here as a packed struct of two DWORDS:
	// index and value
	case FF_SETPARAMETER:
		return (LPVOID) setParameter( (SetParameterStruct*) pParam );
	
	case FF_PROCESSFRAME:
		return (LPVOID) processFrame(pParam);

	case FF_GETPARAMETER:
		return (LPVOID)(DWORD) getParameter((DWORD) pParam);

	case FF_GETPLUGINCAPS:
		return (LPVOID) getPluginCaps( (DWORD) pParam);
	
	default:
		return (LPVOID) FF_FAIL;
	}
 }


/////////////////////////////////////////////////////
//
// Plugin Globals
//
// these are defined globally in this sample plugin for simplicities sake
// Plugin developers should allocate memory from the heap for this stuff
//

PlugInfoStruct plugInfo;
VideoInfoStruct videoInfo;
ParameterStruct parameters[NUM_PARAMS];


/////////////////////////////////////////////////////
//
// Sample Function implementation
//

///////////////////////////////////////////////////////////////////////////////////////
// getInfo
//
// gets information about the plugin - version, unique id, short name and type 
// This function should be identical in all future versions of the FreeFrame API
//  
// return values (32-bit pointer to PlugInfoStruct)
// FF_FAIL on error
// 32-bit pointer to PlugInfoStruct
//
// HOST: call this function first to get version information
//       the version defines the other fucntion codes that are supported
//       supported function codes are listed in the documentation www.freeframe.org

PlugInfoStruct* getInfo() 
{
	plugInfo.APIMajorVersion = 0;
	plugInfo.APIMinorVersion = 1024; // lets keep this in sync with the delphi host for now
	char ID[5] = "MPB1";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "MSVC_Plugin_Base";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	return &plugInfo;
}

///////////////////////////////////////////////////////////////////////////////////////
// initialise
//
// Prepare the Plug-in for processing.  
// Set default values, allocate memory
// When the plug-in returns from this function it must be ready to proces a frame
//
// return values (DWORD)
// FF_SUCCESS - success
// non-zero - fail (error values to be defined)
//
// HOST: This function *must* return before a call to processFrame

DWORD initialise(VideoInfoStruct* pVideoInfo)
{
	// make a copy of the VideoInfoStruct
	videoInfo.frameWidth = pVideoInfo->frameWidth;
	videoInfo.frameHeight = pVideoInfo->frameHeight;
	videoInfo.bitDepth = pVideoInfo->bitDepth;

	// this shouldn't happen if the host is checking the capabilities properly
	if (videoInfo.bitDepth != 1)
	{
		return FF_FAIL;
	}

	// populate the parameters structs
	parameters[0].defaultValue = 0.5f;
	parameters[1].defaultValue = 0.0f;
	parameters[2].defaultValue = 1.0f;
	parameters[0].value = 0.5f;
	parameters[1].value = 0.0f;
	parameters[2].value = 1.0f;
	char tempName1[17] = "brightness";
	char tempName2[17] = "unused____param2";
	char tempName3[17] = "unused____param3";
	memcpy(parameters[0].name, tempName1, 16);
	memcpy(parameters[1].name, tempName2, 16);
	memcpy(parameters[2].name, tempName3, 16);

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// deinitialise
//
// Tidy up   
// Deallocate memory
//
// return values (DWORD)
// FF_SUCCESS - success
// non-zero - fail (error values to be defined)
//
// HOST: This *must* be the last function called on the plugin

DWORD deInitialise()
{
	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getNumParameters 
//
// returns number of parameters in plugin
// 
// return values (DWORD)
// number of parameters
// FF_FAIL on error
//

DWORD getNumParameters()
{
	return NUM_PARAMS;  
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameterName
//
// returns pointer to 16 byte char array containing the name of parameter specified by index
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit pointer to char):
// 32-bit pointer to array of char
// FF_FAIL on error
//

char* getParameterName(DWORD index)
{
	return parameters[index].name;
}


///////////////////////////////////////////////////////////////////////////////////////
// getParameterDefault
//
// returns default value of parameter specified by index as 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit float):
// 32-bit float value
// FF_FAIL on error
//

float getParameterDefault(DWORD index)
{
	return parameters[index].defaultValue;
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameterDisplay
//
// returns pointer to array of 16 char containing a string to display as the value of
// parameter index
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit pointer to char):
// 32-bit pointer to array of char
// FF_FAIL on error
//

char* getParameterDisplay(DWORD index)
{
	// fill the array with spaces first
	for (int n=0; n<16; n++) {
		parameters[index].displayValue[n] = ' ';
	}
	sprintf(parameters[index].displayValue, "%f",parameters[index].value);
	return parameters[index].displayValue;
}

///////////////////////////////////////////////////////////////////////////////////////
// setParameter
//
// Sets the value of parameter specified by index
// value is a 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
// 32-bit float value
//
// return values (DWORD):
// FF_FAIL on error
//

DWORD setParameter(SetParameterStruct* pParam)
{
	parameters[pParam->index].value = pParam->value;
	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameter
//
// returns value of parameter specified by index as 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit float):
// 32-bit float value
// FF_FAIL on error
//

float getParameter(DWORD index)
{
	return parameters[index].value;
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrame
//
// process a frame of video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD processFrame(LPVOID pFrame)
{
	VideoPixel24bit* pPixel = (VideoPixel24bit*) pFrame;
	for (DWORD x = 0; x < videoInfo.frameWidth; x++) {
		for (DWORD y = 0; y < videoInfo.frameHeight; y++) {
			pPixel->blue = (BYTE) (pPixel->blue * parameters[0].value);
			pPixel->green = (BYTE) (pPixel->green * parameters[0].value);
			pPixel->red = (BYTE) (pPixel->red * parameters[0].value);
			pPixel++;
		}
	}

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getpluginCaps
//
// returns true or false to indicate whether cappable of feature specified by index
//
// parameters:
// DWORD index - index of parameter 
// allowed values:
// 0 - 16 bit video
// 1 - 24 bit video
// 2 - 32 bit video
//
// return values (DWORD):
// FF_TRUE
// FF_FALSE
//

DWORD getPluginCaps(DWORD index)
{
	switch (index) {
	case 0:
		return FF_FALSE;
	case 1:
		return FF_TRUE;
	case 2:
		return FF_FALSE;
	default:
		return FF_FALSE;
	}
}
