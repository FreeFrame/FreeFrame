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
  NumParameters: dword = 1;

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParameterNameStruct: TParameterNameStruct;
  pParameterNameStruct: pointer;
  ParameterArray: array [0..3] of single;
  ParameterDisplayValue: array [0..15] of char; // this is the current transfer value for when a parameter display value is requested

implementation

procedure InitLib;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1032;
    PluginUniqueID:='BLOW';
    PluginName:='Blow';
    PluginType:=0;
  end;
  ParameterNameStruct.Parameter0Name:='Blow            ';
  ParameterNameStruct.Parameter1Name:='unused param1   ';
  ParameterNameStruct.Parameter2Name:='unused param2   ';
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
end;

function DeInitialise(pParam: pointer): pointer;
begin
  result:=pointer(0);
end;

function ProcessFrame(pParam: pointer): pointer;
var
  tempPdword : pdw;
  i,j,blow : integer;
  Ptr : PByteArray;
  Pitch : integer;
  c: dword;
  r,g,b : byte;
begin
  //pitch depends on the bitdepth
  Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit
  blow := Round(VideoInfoStruct.FrameWidth * ParameterArray[0]);

  Ptr := PByteArray(Integer(pParam));
  for i:=0 to VideoInfoStruct.FrameHeight-1 do begin
   for j := blow to VideoInfoStruct.FrameWidth-1 do begin
     if j = blow then begin
      b := Ptr^[j*3];
      g := Ptr^[j*3+1];
      r := Ptr^[j*3+2];
     end else begin
      Ptr^[j*3]   := b;
      Ptr^[j*3+1] := g;
      Ptr^[j*3+2] := r;
     end;
   end;
   Ptr := PByteArray(Integer(pParam) + (i*Pitch));
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
      tempSingle:=0.5;
      result:=pointer(tempSingle);
    end;
    1: begin
      // Not yet implemented
      result:=pointer(80);
    end;
    2: begin
      // Not yet implemented
      result:=pointer(10);
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
      //tempstring:=copy(inttostr(round(parameterarray[0]*10)),0,2);
      //copy(inttostr(round(parameterarray[0]*10)),0,2);   //'Brightness Value';
      ParameterDisplayValue:='Blow value';
      result:=@ParameterDisplayValue;
    end;
    1: begin
      ParameterDisplayValue:='dummy value1    ';
      result:=@ParameterDisplayValue;
    end;
    2: begin
      ParameterDisplayValue:='dummy value2    ';
      result:=@ParameterDisplayValue;
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
      ParameterArray[1]:=tempPDWvalue^;
      result:=pointer(0);
    end;
    2: begin
      ParameterArray[2]:=tempPDWvalue^;
      result:=pointer(0);
    end;
    3: begin
      ParameterArray[3]:=tempPDWvalue^;
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
    2: result:=pointer(0);   // 2=32bit
    else result:=pointer($FFFFFFFF)   // unknown PluginCapsIndex
  end;
end;

end.
