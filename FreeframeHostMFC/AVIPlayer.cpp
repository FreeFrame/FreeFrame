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

// AVIPlayer.cpp: implementation of the AVIPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreeframeMFC.h"
#include "AVIPlayer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVIPlayer::AVIPlayer()
{
	pBitmapInfo = 0;
	currentFrame = 0;
	pBitmapInfoHdr = 0;
	pFrame = 0;
	pData = 0;
	videoInfo.bitDepth = 0;
	videoInfo.frameHeight = 0;
	videoInfo.frameWidth = 0;
	isOpen = false;
}

AVIPlayer::~AVIPlayer()
{
	if (isOpen) closeAVI();
}

// LoadAVIFile - loads AVIFile and opens an AVI file.
//
// szfile - filename
// hwnd - window handle
//
void AVIPlayer::openAVI(char* szFile)
{
	unsigned int n;
	PAVISTREAM ptempStream;
	//test
	//szFile = "C:\\stuff\\projects\\pink flower.avi";
	//szFile = "C:\\winnt\\clock.avi";

    AVIFileInit();          // opens AVIFile library

    hr = AVIFileOpen(&pfile, szFile, OF_SHARE_DENY_WRITE, 0L);
    if (hr != 0){
        MessageBox(0, "Unable to open file", "Freeframe Error", MB_OK);
        return;
    }

//
// Place functions here that interact with the open file.
//

    hr = AVIFileInfo( pfile, &fileInfo, sizeof(fileInfo) );
    if (hr != 0){
        MessageBox(0, "Unable to get avifile info", "Freeframe Error", MB_OK);
        return;
    }

	for (n=0; n<fileInfo.dwStreams; n++) {
		hr = AVIFileGetStream(pfile, &ptempStream, 0, n);
		hr = AVIStreamInfo( ptempStream, &streamInfo, sizeof(streamInfo) );
	    if (hr != 0){
	        MessageBox(0, "Unable to get avistream info", "Freeframe Error", MB_OK);
	        return;
		}
		if (streamInfo.fccType == streamtypeVIDEO) {
			pstream = ptempStream;

			pGetFrameObj = AVIStreamGetFrameOpen(pstream, 0);
			if (pGetFrameObj == 0){
				MessageBox(0, "Unable to decompress", "Freeframe Error", MB_OK);
	        return;
			}
			videoInfo.bitDepth = 1;  //TODO sort this out
			videoInfo.frameHeight = streamInfo.rcFrame.bottom;
			videoInfo.frameWidth = streamInfo.rcFrame.right;
			numFrames = AVIStreamEnd(pstream);


		}
	}

	isOpen = true;
}

void AVIPlayer::getFrame() {

	pBitmapInfo = (BITMAPINFO*) AVIStreamGetFrame(pGetFrameObj, currentFrame);
	if (!pBitmapInfo) {
		MessageBox(0, "couldn't AVIStreamGetFrame","",MB_OK);
		return;
	}
	pFrame = (BYTE*)((BYTE*)pBitmapInfo + pBitmapInfo->bmiHeader.biSize +
						pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD));
	pBitmapInfoHdr = &pBitmapInfo->bmiHeader;
}


void AVIPlayer::closeAVI()
{
	if (pGetFrameObj) AVIStreamGetFrameClose(pGetFrameObj);
    if (pstream) AVIStreamRelease(pstream);
	if (pfile) AVIFileRelease(pfile);  // closes the file
    AVIFileExit();          // releases AVIFile library
}

void AVIPlayer::nextFrame()
{
	currentFrame++;
	if (currentFrame == numFrames) {
		currentFrame = 0;
	}


}

void AVIPlayer::play()
{
	playing = true;
}

void AVIPlayer::pause()
{
	playing = false;
}
