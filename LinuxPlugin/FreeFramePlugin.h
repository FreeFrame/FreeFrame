/*********************************************************
 *  Copyright (c) 2002, Russell Blakeborough
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the <ORGANIZATION> nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ********************************************************/



/****************************************
 *
 *
 *
 *
 *
 */

typedef unsigned int DWORD;

struct TPluginInfoStruct {
  DWORD APIMajorVersion;
  DWORD APIMinorVersion;
  char PluginUniqueID[4];
  char PluginName[16];
  DWORD PluginType; // woz effect / synth - could be live video, etc
  DWORD BitDepth; // Supported bit depths
};

struct TVideoInfoStruct {
  DWORD FrameWidth;
  DWORD FrameHeight;
  DWORD BitDepth; /* Provided bit depth 0=16 bit 5-6-5, 1=24bit, 2=32bit 
		     (I propose 1,2,4) */
};

TPluginInfoStruct *getInfo();

extern "C" {
DWORD InitPlugin(TVideoInfoStruct *);
DWORD DeInitPlugin(TVideoInfoStruct *);
DWORD ProcessFrame(DWORD *src,DWORD *dst);
DWORD GetNumParameters();
DWORD GetParameterName(DWORD);
DWORD GetParameterDefault(DWORD);
DWORD GetParameterDisplay(DWORD);
DWORD SetParameter(DWORD);
DWORD GetParameter(DWORD);
}
