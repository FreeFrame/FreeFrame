///////////////////////////////////////////////////////////////////////////////////
// FreeFrame.h
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

#ifndef __FREEFRAMESAMPLE_H__
#define __FREEFRAMESAMPLE_H__

//////////////////////////////////////////////////////////////////////////////////
//
// includes
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#elif defined LINUX

//typedefs for linux - in windows these are defined in files included by windows.h
typedef unsigned int DWORD;
typedef void * LPVOID;
typedef char BYTE;

#endif

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

PlugInfoStruct*	getInfo();							

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
// HOST:	This function *must* return before a call to processFrame.  Pointer to 
//			videoInfoStruct *must* be valid until function returns
// PLUGIN:  make a copy of the videoInfoStruct locally as pointer may not be valid after
//			function returns

DWORD	initialise(VideoInfoStruct* videoInfo);								

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

DWORD	deInitialise();								

///////////////////////////////////////////////////////////////////////////////////////
// getNumParameters 
//
// returns number of parameters in plugin
// 
// return values (DWORD)
// number of parameters
// FF_FAIL on error
//

DWORD	getNumParameters();							

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

char*	getParameterName(DWORD index);				

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

float	getParameterDefault(DWORD index);			

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

char*	getParameterDisplay(DWORD index);			

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

DWORD	setParameter(SetParameterStruct* pParam);		

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

float	getParameter(DWORD index);					

///////////////////////////////////////////////////////////////////////////////////////
// processFrame
//
// process a frame of video 'in place' 
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//
// HOST: pFrame needs to be a valid pointer throughout this call as the plugin processes
// the frame 'in place'.

DWORD	processFrame(LPVOID pFrame);				


///////////////////////////////////////////////////////////////////////////////////////
// getpluginCaps
//
// returns true pof false to indicate whether cappable of feature specified by index
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

DWORD	getPluginCaps(DWORD index);					

#endif
