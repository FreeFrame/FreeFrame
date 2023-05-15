///////////////////////////////////////////////////////////////////////////////////
// FreeFrameSample.cpp
//
// FreeFrame Open Video Plugin
// C Version
//
// Implementation of the Free Frame sample plugin
//
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

#include "FF_Star.h"

#include <stdio.h>
#include <math.h>

#define NUMSTARS 100

////////////////////////////////////////////////////////////////////////////////////////
//
// global struct


PlugStruct plug;

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
	plug.plugInfo.APIMajorVersion = 0;
	plug.plugInfo.APIMinorVersion = 53; // lets keep this in sync with the delphi host for now
	char ID[5] = "Star";		 // this *must* be unique to your plugin
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "Star Filter";

	memcpy(plug.plugInfo.uniqueID, ID, 4);
	memcpy(plug.plugInfo.pluginName, name, 16);
	plug.plugInfo.pluginType = FF_EFFECT;

	return &plug.plugInfo;
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
	plug.videoInfo.frameWidth = pVideoInfo->frameWidth;
	plug.videoInfo.frameHeight = pVideoInfo->frameHeight;
	plug.videoInfo.bitDepth = pVideoInfo->bitDepth;

	// this shouldn't happen if the host is checking the capabilities properly
	if (plug.videoInfo.bitDepth != 1)
	{
		return FF_FAIL;
	}

	// populate the parameters structs
	plug.threshold = 765;
	plug.parameters[0].defaultValue = 1.0f;
	plug.parameters[0].value = 1.0f;
	char tempName1[17] = "threshold";
	memcpy(plug.parameters[0].name, tempName1, 16);

/*	plug.fadeOutRate = 0;
	plug.parameters[1].defaultValue = 0.0f;
	plug.parameters[1].value = 0.0f;
	char tempName2[17] = "fade out rate";
	memcpy(plug.parameters[1].name, tempName2, 16);

	plug.fadeInRate = 0;
	plug.parameters[2].defaultValue = 0.0f;
	plug.parameters[2].value = 0.0f;
	char tempName3[17] = "fade in rate";
	memcpy(plug.parameters[2].name, tempName3, 16);
*/
	plug.frameSize = plug.videoInfo.frameHeight * plug.videoInfo.frameWidth;

	//Allocate memory

	plug.fadeFactor = new VideoPixel24bit[plug.frameSize];
	plug.lastFrame = new VideoPixel24bit[plug.frameSize];
	plug.tempFrame = new VideoPixel24bit[plug.frameSize];
	memset(plug.fadeFactor, 0, plug.frameSize * 3);
	memset(plug.lastFrame, 0, plug.frameSize * 3);
	memset(plug.tempFrame, 0, plug.frameSize * 3);
	plug.firstFrame = true;

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
	if (plug.fadeFactor) delete plug.fadeFactor;
	if (plug.lastFrame) delete plug.lastFrame;
	if (plug.tempFrame) delete plug.tempFrame;
	plug.fadeFactor = 0;
	plug.lastFrame = 0;
	plug.tempFrame = 0;
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
	return plug.parameters[index].name;
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
	return plug.parameters[index].defaultValue;
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
		plug.parameters[index].displayValue[n] = ' ';
	}
	sprintf(plug.parameters[index].displayValue, "%f", plug.parameters[index].value);
	return plug.parameters[index].displayValue;
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
	switch (pParam->index) {
	case 0:
		plug.parameters[pParam->index].value = (int)(pParam->value*765);
		plug.threshold  = plug.parameters[pParam->index].value;
		break;
	case 1:
		plug.parameters[pParam->index].value = (int)(pParam->value*100);
		plug.fadeOutRate  = plug.parameters[pParam->index].value;
		break;
	case 2:
		plug.parameters[pParam->index].value = (int)(pParam->value*100);
		plug.fadeInRate  = plug.parameters[pParam->index].value;
		break;
	default:
		return FF_FAIL;
	}
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
	return plug.parameters[index].value;
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
	VideoPixel24bit *pSource;
	VideoPixel24bit *pDest;
	VideoPixel24bit *pPixel;

	pSource = (VideoPixel24bit*) pFrame;
	pDest = (VideoPixel24bit*) plug.tempFrame;

	//determine points of highest luminance

	DWORD x, y, n;
	DWORD ty, tx;

	int lume;
	DWORD starSize = 10;
	BYTE starFade = 4;
	BYTE blue, green, red;

	int count = 0;
	int lowestLume = 0;
	n=0;
	memcpy(plug.tempFrame, pFrame, plug.frameSize * sizeof(VideoPixel24bit));

	struct coord {
		int lum;
		DWORD x;
		DWORD y;
	};

	coord pointArray[NUMSTARS];

	//initialise pointArray
	for (n=0; n<NUMSTARS; n++) {
		pointArray[n].x = 0;
		pointArray[n].y = 0;
		pointArray[n].lum = 0;
	}

	for ( y=0; y < plug.videoInfo.frameHeight; y++) {
		for ( x=0; x < plug.videoInfo.frameWidth; x++) {
			lume = pSource->blue + pSource->green + pSource->red;
			if (lume > plug.threshold  && lume>lowestLume) {
				//Collect points of highest luminance
				int newLowestLume = pointArray[0].lum;
				int replacePixel = 0;
				for (n=0; n<NUMSTARS; n++) {
					if (pointArray[n].lum < lume) {
						if (pointArray[n].lum < newLowestLume)
							newLowestLume = pointArray[n].lum;
						count++;
						if (!replacePixel) replacePixel = n;
					}
					pointArray[replacePixel].lum = lume;
					pointArray[replacePixel].x = x;
					pointArray[replacePixel].y = y;

				}
				if (lowestLume < newLowestLume) lowestLume = newLowestLume;
			}
			pSource++;
		}
	}

	pSource = (VideoPixel24bit*) pFrame;

	for (n=0; n<NUMSTARS; n++) {

		ty = pointArray[n].y;
		tx = pointArray[n].x;
		VideoPixel24bit* pOrigin = pDest + (tx) + ((plug.videoInfo.frameWidth) * ty );
		pPixel = pOrigin;

		blue = 0xFF;
		red = 0xFF;
		green = 0xFF;
		x=tx;
		y=ty;

		while (tx < plug.videoInfo.frameWidth && tx < x+starSize) {
			//if (blue < pSource->blue) blue = pSource->blue;
			//if (green < pSource->green) green = pSource->green;
			//if (red < pSource->red) red = pSource->red;
			pPixel->blue = blue;
			pPixel->red = green;
			pPixel->green = red;
			blue -= starFade;
			green -= starFade;
			red -= starFade;
			pPixel++;
			tx++;
		}

		pPixel = pOrigin;
		blue = 0xFF;
		red = 0xFF;
		green = 0xFF;

		while (ty < plug.videoInfo.frameHeight && ty < y+starSize) {
			//if (blue < pSource->blue) blue = pSource->blue;
			//if (green < pSource->green) green = pSource->green;
			//if (red < pSource->red) red = pSource->red;
			pPixel->blue = blue;
			pPixel->red = green;
			pPixel->green = red;
			blue -= starFade;
			green -= starFade;
			red -= starFade;
			pPixel += (plug.videoInfo.frameWidth);
			ty++;
		}
		ty = y;
		tx = x;
		pPixel = pOrigin;
		blue = 0xFF;
		red = 0xFF;
		green = 0xFF;

		while (tx > 0 && tx > x-starSize) {
			//if (blue < pSource->blue) blue = pSource->blue;
			//if (green < pSource->green) green = pSource->green;
			//if (red < pSource->red) red = pSource->red;
			pPixel->blue = blue;
			pPixel->red = green;
			pPixel->green = red;
			blue -= starFade;
			green -= starFade;
			red -= starFade;
			pPixel --;
			tx--;
		}
		pPixel = pOrigin;
		blue = 0xFF;
		red = 0xFF;
		green = 0xFF;

		while (ty > 0 && ty > y-starSize) {
			//if (blue < pSource->blue) blue = pSource->blue;
			//if (green < pSource->green) green = pSource->green;
			//if (red < pSource->red) red = pSource->red;
			pPixel->blue = blue;
			pPixel->red = green;
			pPixel->green = red;
			blue -= starFade;
			green -= starFade;
			red -= starFade;
			pPixel -= (plug.videoInfo.frameWidth);
			ty--;
		}

	}
	//overlay stars on these points

	memcpy(pFrame, plug.tempFrame, plug.frameSize * sizeof(VideoPixel24bit));
	plug.firstFrame = false;
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

