// FreeFrame Open Video Plugin Prototype
// Delphi Version

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
   * Neither the name of the <ORGANIZATION> nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}

unit pluginMain;

interface

uses
  windows, sysutils;

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
  end;
  TParamaterNameStruct = record
    Paramater0Name: array [0..15] of char;
    Paramater1Name: array [0..15] of char;
    Paramater2Name: array [0..15] of char;
  end;

  pdw = ^Dword;
  pw = ^word;
  pb = ^byte;

function GetInfo(pParam: pointer): pointer;
function InitPlugin(pParam: pointer): pointer;
function DeInitPlugin(pParam: pointer): pointer;
function ProcessFrame(pParam: pointer): pointer;
function GetNumParamaters(pParam: pointer): pointer;
function GetParamaterName(pParam: pointer): pointer;
function GetParamaterDefault(pParam: pointer): pointer;
function GetParamaterDisplay(pParam: pointer): pointer;
function SetParamater(pParam: pointer): pointer;
function GetParameter(pParam: pointer): pointer;

procedure InitLib;

const
  NumParamaters: dword = 3;

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParamaterNameStruct: TParamaterNameStruct;
  pParamaterNameStruct: pointer;
  ParamaterArray: array [0..3] of single;
  ParamaterDisplayValue: array [0..15] of char; // this is the current transfer value for when a paramater display value is requested

implementation

procedure InitLib;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1021;
    PluginUniqueID:='PTST';
    PluginName:='PascalTestPlugin';
    PluginType:=0;
  end;
  ParamaterNameStruct.Paramater0Name:='Brightness      ';
  ParamaterNameStruct.Paramater1Name:='unused param1   ';
  ParamaterNameStruct.Paramater2Name:='unused param2   ';
  //ParamaterNameStruct[3]:='sdkYYYYYYYYefwke';
end;

function GetInfo(pParam: pointer): pointer;
begin
  pPluginInfoBlock:=@pluginInfoStruct;
  result:=pointer(pPluginInfoBlock);
end;

function InitPlugin(pParam: pointer):pointer;
var
  tempPointer: pDw;
begin
  tempPointer:=pDw(pParam);
  pVideoInfoStruct:=@VideoInfoStruct;
  pVideoInfoStruct^:=tempPointer^;
  inc(tempPointer);
  inc(pVideoInfoStruct);
  pVideoInfoStruct^:=tempPointer^;
  result:=pointer(95);
end;

function DeInitPlugin(pParam: pointer): pointer;
begin
  result:=pointer(894);
end;

function ProcessFrame(pParam: pointer): pointer;
var
  tempPbyte: pb;
  x: integer;
begin
  tempPbyte:= pb(pParam);
  for x:=0 to (VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*3-1) do begin
    tempPbyte^:=byte(round(cardinal(tempPbyte^)*ParamaterArray[0]));
    inc(tempPbyte);
  end;
  result:=pointer(VideoInfoStruct.FrameWidth);
end;

function GetNumParamaters(pParam: pointer): pointer;
begin
  result:=pointer(NumParamaters);
end;

function GetParamaterName(pParam: pointer): pointer;
begin
  pParamaterNameStruct:=@ParamaterNameStruct;
  case integer(pParam) of
    0: result:=pParamaterNameStruct;
    1: result:=pointer(integer(pParamaterNameStruct)+16);
    2: result:=pointer(integer(pParamaterNameStruct)+32);
    3: result:=pointer(integer(pParamaterNameStruct)+48);
    else result:=pointer(999);
  end;
end;

function GetParamaterDefault(pParam: pointer): pointer;
var
  tempSingle: single;
begin
  case integer(pParam) of
    0: begin
      tempSingle:=0.4;
      result:=pointer(tempSingle);
    end;
    1: begin
      result:=pointer(80);
    end;
    2: begin
      result:=pointer(10);
    end;
    3: begin
      result:=pointer(90);
    end;
    else result:=pointer(9999);
  end;
end;

function GetParamaterDisplay(pParam: pointer): pointer;
begin
  case integer(pParam) of
    0: begin
      ParamaterDisplayValue:='Brightness Value';
      result:=@ParamaterDisplayValue;
    end;
    1: begin
      ParamaterDisplayValue:='dummy value1    ';
      result:=@ParamaterDisplayValue;
    end;
    2: begin
      ParamaterDisplayValue:='dummy value2    ';
      result:=@ParamaterDisplayValue;
    end;
    3: begin
      ParamaterDisplayValue:='dummy value3    ';
      result:=@ParamaterDisplayValue;
    end;
    else begin
      ParamaterDisplayValue:='9999999999999999';
      result:=@ParamaterDisplayValue;
    end;
  end;
end;

function SetParamater(pParam: pointer): pointer;
var
  tempPDWparam, tempPDWvalue: pdw;
begin
  tempPDWparam:=pdw(pParam);
  tempPDWvalue:=pdw(pParam);
  inc(tempPDWvalue);
  case integer(tempPDWparam^) of
    0: begin
      copymemory(@ParamaterArray[0],tempPDWvalue,4);
      //ParamaterArray[0]:=tempPDWvalue^;
      result:=pointer(439);
    end;
    1: begin
      ParamaterArray[1]:=tempPDWvalue^;
      result:=pointer(751);
    end;
    2: begin
      ParamaterArray[2]:=tempPDWvalue^;
      result:=pointer(752);
    end;
    3: begin
      ParamaterArray[3]:=tempPDWvalue^;
      result:=pointer(753);
    end;
    else result:=pointer(4368);
  end;
end;

function GetParameter(pParam: pointer): pointer;
var
  tempSingle: single;
  tempDWvalue: dword;
begin
  tempSingle:=ParamaterArray[integer(pParam)];
  copymemory(@tempDWvalue,@tempSingle,4);
  result:=pointer(tempDWvalue);
end;

end.
