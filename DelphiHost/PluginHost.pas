// FreeFrame Open Video Plugin Host Test Container Prototype
// Delphi Version
// FreeFrame v0.70

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

See specification.txt in the docs module on FreeFrame CVS at SourcdForge for uptodate function code table and function specifications

}

unit PluginHost;

interface

uses windows, sysutils;

type
  tPlugMainFunction = function(functionCode: dword; pParam: pointer; InstanceID: dword): pointer; stdcall;
  TPluginInfoStruct = record
    APIMajorVersion: dword;
    APIMinorVersion: dword;
    PluginUniqueID: array [0..3] of char;   // 4 characters = 1 dword
    PluginName: array [0..15] of char;       // 16 characters = 4 Dwords
    PluginType: dword; //(effect, source); // woz effect / synth - but could be other than synth eg. live video input
  end;
  TVideoInfoStruct = record
    FrameWidth: dword;
    FrameHeight: dword;
    BitDepth: dword;   // 0=16bit 1=24bit 2=32bit
  end;
  //TParameterNameStruct = array [0..2] of array [0..15] of char;
  pdw = ^dword;

  function GetInfo: dword;
  function InitialisePlugin: dword;
  function DeInitialisePlugin: dword;
  function ProcessFrame(pFrame: pointer; InstanceID: dword): dword;
  function GetNumParameters: dword;
  function GetParameterName(Param: dword): string;
  function GetParameterDefault(Param: dword): single;
  function GetParameterDisplay(Param: dword; InstanceID: dword): string;
  function SetParameter(Param: dword; Value: single; InstanceID: dword): dword;
  function GetParameter(Param: dword; InstanceID: dword): single;
  function GetPluginCaps(Param: dword): boolean;
  function InstantiatePlugin(VideoInfoStruct: TVideoInfoStruct): dword;
  function DeInstantiatePlugin(InstanceID: dword): dword;

var
  PluginInfoStruct: TPluginInfoStruct;
  VideoInfoStruct: TVideoInfoStruct;
  plugMain:tPlugMainFunction;

implementation

function GetInfo: dword;
var
  pPluginInfoStruct: pointer;
  pParam: PDword;
  tempPChar: PChar;
  i: integer;
begin
  pPluginInfoStruct:=plugMain(0,nil,0);
  with PluginInfoStruct do begin
    pParam:=pPluginInfoStruct;
    APIMajorVersion:=dword(pParam^);
    inc(pParam);
    APIMinorVersion:=dword(pParam^);
    inc(pParam);
    temppchar:=pchar(pParam);
    for i:=0 to 3 do begin
      PluginUniqueID[i]:=char(tempPchar^);
      inc(temppChar);
    end;
    for i:=0 to 15 do begin
      PluginName[i]:=char(tempPchar^);
      inc(tempPchar);
    end;
    pParam:=pDword(tempPchar);
    PluginType:=dword(pParam^);
  end;
  result:=dword(pPluginInfoStruct);
end;

function InitialisePlugin: dword;
begin
  result:=dword(plugMain(1,nil,0));
end;

function DeInitialisePlugin: dword;
begin
  try
    result:=dword(plugMain(2,nil,0));
  except
    result:=23;
  end;
end;

function ProcessFrame(pFrame: pointer; InstanceID: dword): dword;
begin
  result:=dword(plugMain(3,pFrame,InstanceID));
end;

function GetNumParameters: dword;
begin
  result:=dword(plugMain(4,nil,0));
end;

function GetParameterName(Param: dword): string;
var
  tempParamName: array [0..15] of char;
  tempSourcePointer: pdw;
  tempDestPointer: pdw;
  i: integer;
begin
  tempSourcePointer:=pdw(plugMain(5,pointer(Param),0));
  tempDestPointer:=pdw(@tempParamName);
  for i:=0 to 3 do begin
    tempDestPointer^:=tempSourcePointer^;
    inc(tempSourcePointer);
    inc(tempDestPointer);
  end;
  result:=string(tempParamName);
end;

function GetParameterDefault(Param: dword): single;
var
  tempSingle: single;
  tempDword: dword;
begin
  tempDword:=dword(plugMain(6,pointer(Param),0));
  copymemory(@tempSingle,@tempDword,4);
  result:=tempSingle;
end;

function GetParameterDisplay(Param: dword; InstanceID: dword): string;
var
  tempParamDisplay: array [0..15] of char;
  tempSourcePointer: pdw;
  tempDestPointer: pdw;
  i: integer;
begin
  tempSourcePointer:=pdw(plugMain(7,pointer(Param),InstanceID));
  tempDestPointer:=pdw(@tempParamDisplay);
  for i:=0 to 3 do begin
    tempDestPointer^:=tempSourcePointer^;
    inc(tempSourcePointer);
    inc(tempDestPointer);
  end;
  result:=string(tempParamDisplay);
end;

function SetParameter(Param: dword; Value: single; InstanceID: dword): dword;
type
  TSetParamStruct = array [0..1] of dword;
var
  SetParamStruct: TSetParamStruct;
  tempPdword: pdw;
begin
  SetParamStruct[0]:=Param;
  tempPdword:=@value;
  SetParamStruct[1]:=tempPdword^;
  result:=dword(plugMain(8,@SetParamStruct, InstanceID));
end;

function GetParameter(Param: dword; InstanceID: dword): single;
var
  tempDword: dword;
  tempPdword: pdw;
  tempSingle: single;
  tempPsingle: pointer;
begin
  tempdword:=dword(plugMain(9,pointer(Param),InstanceID));
  tempPdword:=@tempDword;
  tempPsingle:=@tempSingle;
  copymemory(tempPsingle,tempPdword,4);
  result:=tempSingle;
end;

function GetPluginCaps(Param: dword): boolean;
begin
  result := false;
  case dword(plugMain(10,pointer(Param),0)) of
    0: result:=false;
    1: result:=true;
  end;
end;

function InstantiatePlugin(VideoInfoStruct: TVideoInfoStruct): dword;
var
  pVideoInfoStruct: pointer;
begin
  pVideoInfoStruct:=pointer(@VideoInfoStruct);
  result:=dword(plugMain(11,pVideoInfoStruct,0));
end;

function DeInstantiatePlugin(InstanceID: dword): dword;
begin
  result:=dword(plugMain(12,nil,InstanceID));
end;

end.

