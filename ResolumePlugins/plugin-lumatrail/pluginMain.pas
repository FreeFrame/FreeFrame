// FreeFrame Open Video Plugin Prototype
// Delphi Version

// www.freeframe.org
// boblists@brightonart.org

{

Copyright (c) 2002, Russell Blakeborough
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

}

unit pluginMain;

interface

{$IFDEF LINUX}
uses
  sysutils,
  Types;
{$ENDIF}

{$IFDEF WIN32}
uses
  sysutils,
  windows;
{$ENDIF}

type
  TPluginInfoStruct = record
    APIMajorVersion: dword;
    APIMinorVersion: dword;
    PluginUniqueID: array [0..3] of char;   // 4 characters = 1 dword
    PluginName: array [0..15] of char;       // 16 characters = 4 Dwords
    PluginType: dword; // (effect, source); // woz effect / synth - but could be other than synth eg. live video input
  end;
  TVideoInfoStruct = record
    FrameWidth: dword;
    FrameHeight: dword;
    BitDepth: dword;     // 0 = 16 bit 5-6-5   1 = 24bit packed   2 = 32bit
  end;
  TParameterNameStruct = record
    Parameter0Name: array [0..15] of char;
    Parameter1Name: array [0..15] of char;
    Parameter2Name: array [0..15] of char;
  end;

  pdw = ^Dword;
  pw = ^word;
  pb = ^byte;

function GetInfo(pParam: pointer): pointer;
function Initialise(pParam: pointer): pointer;
function DeInitialise(pParam: pointer): pointer;
function ProcessFrame(pParam: pointer): pointer;
function GetNumParameters(pParam: pointer): pointer;
function GetParameterName(pParam: pointer): pointer;
function GetParameterDefault(pParam: pointer): pointer;
function GetParameterDisplay(pParam: pointer): pointer;
function SetParameter(pParam: pointer): pointer;
function GetParameter(pParam: pointer): pointer;
function GetPluginCaps(pParam: pointer): pointer;
function CopyMemory(dst: pointer; src: pointer; size: integer): pointer;

procedure InitLib;

const
  NumParameters: dword = 2;

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParameterNameStruct: TParameterNameStruct;
  pParameterNameStruct: pointer;
  ParameterArray: array [0..3] of single;
  ParameterDisplayValue: array [0..15] of char; // this is the current transfer value for when a parameter display value is requested
  // buffer
  p32bitFrame: pointer;
  p24bitFrame: pointer;
  //
  YTblR, YTblG, YTblB    : array[0..255] of Byte;


implementation

uses utils;

procedure InitLib;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1050;
    PluginUniqueID:='LTRA';
    PluginName:='Luma trail';
    PluginType:=0;
  end;
  ParameterNameStruct.Parameter0Name:='luma            ';
  ParameterNameStruct.Parameter1Name:='inverse         ';
  ParameterNameStruct.Parameter2Name:='                ';
  //ParameterNameStruct[3]:='sdkYYYYYYYYefwke';
end;

function GetInfo(pParam: pointer): pointer;
begin
  pPluginInfoBlock:=@pluginInfoStruct;
  result:=pointer(pPluginInfoBlock);
end;

function CopyMemory(dst: pointer;src: pointer; size: integer): pointer;
var
  temps: pb;
  tempd: pb;
  x: integer;
begin
  temps:=pb(src);
  tempd:=pb(dst);
  for x:=0 to (size-1) do begin
    tempd^:=temps^;
    inc(tempd);
    inc(temps);
  end;
  result:=pointer(0);
end;

function Initialise(pParam: pointer):pointer;
var
  tempPointer: pDw;
  i: integer;
begin
  tempPointer:=pDw(pParam);
  pVideoInfoStruct:=@VideoInfoStruct;
  pVideoInfoStruct^:=tempPointer^;   // Frame Width
  inc(tempPointer);
  inc(pVideoInfoStruct);
  pVideoInfoStruct^:=tempPointer^;   // Frame Height
  inc(tempPointer);
  inc(pVideoInfoStruct);
  pVideoInfoStruct^:=tempPointer^;   // Bit Depth
  result:=pointer(0);

  case VideoInfoStruct.BitDepth of
   1 : begin
        p24bitFrame := Make24bitBuffer(VideoInfoStruct);
        Fill24bitBuffer(p24bitFrame,0,VideoInfoStruct);
       end;
   2 : begin
        p32bitFrame := Make32bitBuffer(VideoInfoStruct);
        Fill32bitBuffer(p32bitFrame,0,VideoInfoStruct); //fill with black, using an other color could look like this rgb(255,0,0)
       end;
  end;

  for i:=0 to 255 do begin
    YTblR[i] := Trunc(0.3588*i);
    YTblG[i] := Trunc(0.4020*i);
    YTblB[i] := Trunc(0.2392*i);
  end;

end;

function DeInitialise(pParam: pointer): pointer;
begin
  case VideoInfoStruct.BitDepth of
   1 : Free24bitBuffer(p24bitFrame,VideoInfoStruct);
   2 : Free32bitBuffer(p32bitFrame,VideoInfoStruct);
  end;
  result:=pointer(0);
end;

function ProcessFrame(pParam: pointer): pointer;
var
  tempPdword : pdw;
  i,j : integer;
  PtrSrc,PtrDest : PByteArray;
  Pitch : integer;
  c: dword;
  d1,d2 : single;
  Amount,Luminance : byte;
  Inverse : boolean;

    function Tresh(value : integer) : byte;
    begin
      if Inverse then begin
       if value <= amount then result := 255
       else result := 0;
      end else begin
       if value <= amount then result := 0
       else result := 255;
      end;
    end;

begin
   Inverse := ParameterArray[1] > 0;
   if Inverse then begin
    Amount := round( (255/100)*(ParameterArray[0]*100) );
   end else begin
    Amount := 255 - round( (255/100)*(ParameterArray[0]*100) );
   end;

   if VideoInfoStruct.BitDepth = 1 then begin
     Pitch := VideoInfoStruct.FrameWidth * 3;
     PtrDest := PByteArray(Integer(pParam));
     PtrSrc  := PByteArray(Integer(p24bitFrame));
     for i := 0 to VideoInfoStruct.FrameHeight do begin
      for j := 0 to VideoInfoStruct.FrameWidth-1 do begin
         luminance := YTblB[PtrDest^[j*3]] + YTblG[PtrDest^[j*3+1]] + YTblR[PtrDest^[j*3+2]];
         luminance := tresh(luminance);
          if not ( luminance = 0 ) then begin
           PtrDest^[j*3   ] := PtrSrc^[j*3   ];
           PtrDest^[j*3 +1] := PtrSrc^[j*3 +1];
           PtrDest^[j*3 +2] := PtrSrc^[j*3 +2];
          end;
      end;
      PtrDest := PByteArray(Integer(pParam) + (i*Pitch));
      PtrSrc  := PByteArray(Integer(p24bitFrame)  + (i*Pitch));
     end;
    CopyMemory(p24bitFrame,pParam,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*3);
   end else if VideoInfoStruct.BitDepth = 2 then begin
     Pitch := VideoInfoStruct.FrameWidth * 4;
     PtrDest := PByteArray(Integer(pParam));
     PtrSrc  := PByteArray(Integer(p32bitFrame));
     for i := 0 to VideoInfoStruct.FrameHeight do begin
      for j := 0 to VideoInfoStruct.FrameWidth-1 do begin
         luminance := YTblB[PtrDest^[j shl 2]] + YTblG[PtrDest^[j shl 2+1]] + YTblR[PtrDest^[j shl 2+2]];
         luminance := tresh(luminance);
          if not ( luminance = 0 ) then begin
           PtrDest^[j shl 2   ] := PtrSrc^[j shl 2   ];
           PtrDest^[j shl 2 +1] := PtrSrc^[j shl 2 +1];
           PtrDest^[j shl 2 +2] := PtrSrc^[j shl 2 +2];
          end;
      end;
      PtrDest := PByteArray(Integer(pParam) + (i*Pitch));
      PtrSrc  := PByteArray(Integer(p32bitFrame)  + (i*Pitch));
     end;
     CopyMemory(p32bitFrame,pParam,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*4);
    end;

  result:=pointer(0);
end;

function GetNumParameters(pParam: pointer): pointer;
begin
  result:=pointer(NumParameters);
end;

function GetParameterName(pParam: pointer): pointer;
begin
  pParameterNameStruct:=@ParameterNameStruct;
  case integer(pParam) of
    0: result:=pParameterNameStruct;
    1: result:=pointer(integer(pParameterNameStruct)+16);
    2: result:=pointer(integer(pParameterNameStruct)+32);
    3: result:=pointer(integer(pParameterNameStruct)+48);
    else result:=pointer($FFFFFFFF);
  end;
end;

function GetParameterDefault(pParam: pointer): pointer;
var
  tempSingle: single;
begin
  case integer(pParam) of
    0: begin
      tempSingle:=0.1;
      result:=pointer(tempSingle);
    end;
    1: begin
      tempSingle:=0.0;
      result:=pointer(tempSingle);
    end;
    2: begin
      tempSingle:=0.0;
      result:=pointer(tempSingle);
    end;
    3: begin
      // Not yet implemented
      result:=pointer(90);
    end;
    else result:=pointer($FFFFFFFF);
  end;
end;

function GetParameterDisplay(pParam: pointer): pointer;
begin
  case integer(pParam) of
    0: begin
        result:=pointer(inttostr( round( (255/100)*(ParameterArray[0]*100) ) ) );
    end;
    1: begin
        if ParameterArray[1] > 0 then ParameterDisplayValue:='On    '
        else ParameterDisplayValue:='Off    ';
        result:=@ParameterDisplayValue;;
    end;
    2: begin
        ParameterDisplayValue:='dummy value2    ';
        result:=@ParameterDisplayValue;;
    end;
    3: begin
      ParameterDisplayValue:='dummy value3    ';
      result:=@ParameterDisplayValue;
    end;
    else begin
      ParameterDisplayValue:='9999999999999999';
      result:=@ParameterDisplayValue;
    end;
  end;
end;

function SetParameter(pParam: pointer): pointer;
var
  tempPDWparam, tempPDWvalue: pdw;
begin
  tempPDWparam:=pdw(pParam);
  tempPDWvalue:=pdw(pParam);
  inc(tempPDWvalue);
  case integer(tempPDWparam^) of
    0: begin
      copymemory(@ParameterArray[0],tempPDWvalue,4);
      result:=pointer(0);
    end;
    1: begin
      copymemory(@ParameterArray[1],tempPDWvalue,4);
      result:=pointer(0);
    end;
    2: begin
      copymemory(@ParameterArray[2],tempPDWvalue,4);
      result:=pointer(0);
    end;
    3: begin
      copymemory(@ParameterArray[2],tempPDWvalue,4);
      result:=pointer(0);
    end;
    else result:=pointer($FFFFFFFF);
  end;
end;

function GetParameter(pParam: pointer): pointer;
var
  tempSingle: single;
  tempDWvalue: dword;
begin
  tempSingle:=ParameterArray[integer(pParam)];
  copymemory(@tempDWvalue,@tempSingle,4);
  result:=pointer(tempDWvalue);
end;

function GetPluginCaps(pParam: pointer): pointer;
begin
  case integer(pParam) of
    0: result:=pointer(0);   // 0=16bit - not yet supported in this sample plugin
    1: result:=pointer(1);   // 1=24bit - supported
    2: result:=pointer(1);   // 2=32bit
    else result:=pointer($FFFFFFFF)   // unknown PluginCapsIndex
  end;
end;

end.
