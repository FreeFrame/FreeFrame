// FreeFrame Open Video Plugin Test Container
//  and Delphi Host Inclusion Template

// www.freeframe.org
// boblists@brightonart.org

{

Copyright (c) 2002, Russell Blakeborough
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of FreeFrame nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}

unit AVI;

interface

uses VFW, pluginHost, winprocs; 

var
  AVIfilename: string;

procedure Init;
procedure DeInit;
function OpenAVI(Filename: string): Tvideoinfostruct;
procedure CloseAVI;
function GetFrame(FrameNumber: integer): pointer;

implementation

uses main;

var
  pAnAviFile: PAviFile;          //The Avi File
  pVideoStream: pAviStream;      //Pointer to Video Stream
  AGetframe: pGetFrame;          //Pointer to GetFrame struct
  HDrawDibDC: HDrawDib;          //HDrawdib

procedure Init;
begin
  AVIfileInit;
end;

procedure DeInit;
begin
  AviFileExit;
end;

function OpenAVI(Filename: string): Tvideoinfostruct;
var
  i: integer;
  pFileInfo: TAviFileInfo;
  pAnAviStream: pAviStream;
  StreamInfo: TAviStreamInfo;
  tempVideoinfostruct: Tvideoinfostruct;
begin
  AVIFileOpen(pAnAviFile, pchar(Filename), OF_READ, Nil);
  AVIFileInfo(pAnAviFile, @pFileInfo, sizeof(TAviFileInfo));
  for i := 0 to pFileInfo.dwstreams -1 do begin
    AVIFileGetStream(pAnAviFile, pAnAviStream, 0, i);
    AVIStreamInfo(pAnAviStream, @StreamInfo, sizeof(TAviStreamInfo));
    if (StreamInfo.fccType = streamtypeVIDEO) then begin
      pVideoStream := pAnAviStream;
      //fps := round(StreamInfo.dwRate /StreamInfo.dwScale);
      hDrawDibDC := DrawDibOpen();
      AGetFrame := AVIStreamGetFrameOpen(pVideoStream,nil);
      tempVideoInfoStruct.Framewidth:=streaminfo.rcFrame.Right;
      tempVideoInfoStruct.FrameHeight:=streaminfo.rcFrame.Bottom;
      // 1=24bit packed MCI standard
      // 2=32bit video, or 24bit dword aligned for the moment really, with Alpha running at 0
      if not VideoInfoStruct.BitDepth=2 then tempVideoInfoStruct.BitDepth:=2 else tempVideoInfoStruct.BitDepth:=1;
      // Set Orientation Upsidedown
      tempVideoInfoStruct.orientation:=2;
      result:=tempVideoInfoStruct;
      numframes := AVIStreamEnd(pVideoStream);
      exit;
    end;
  end;
end;

procedure CloseAVI;
begin
  if AGetframe <> nil then AVIStreamGetFrameClose(AGetFrame);
  if HDrawDibDC <> 0 then DrawDibClose(hDrawDibDC);
  if pVideoStream <> nil then AVIStreamRelease(pVideoStream);
  if pAnAviFile <> nil then AviFileRelease(pAnAviFile);
end;

function GetFrame(FrameNumber: integer): pointer;
begin
  result := AVIStreamGetFrame(AGetframe, FrameNumber);
end;

end.
