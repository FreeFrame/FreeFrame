// Open Video Plugin Host Prototype
// Delphi Version

// Russell Blakeborough
// russell@brightonart.org

// This code released under the LGPL

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
  NumParamaters: dword = 0;

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParamaterNameStruct: TParamaterNameStruct;
  pParamaterNameStruct: pointer;
  ParamaterArray: array [0..3] of single;
  ParamaterDisplayValue: array [0..15] of char; // this is the current transfer value for when a paramater display value is requested
  //
  YTblR, YTblG, YTblB: array[0..255] of Byte;

implementation

procedure InitLib;
var
 i : integer;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1018;
    PluginUniqueID:='PTST';
    PluginName:='Greyscale';
    PluginType:=0;
  end;
  ParamaterNameStruct.Paramater0Name:='unused param0   ';
  ParamaterNameStruct.Paramater1Name:='unused param1   ';
  ParamaterNameStruct.Paramater2Name:='unused param2   ';
  //
  for i:=0 to 255 do begin
    YTblR[i] := Trunc(0.3588*i);
    YTblG[i] := Trunc(0.4020*i);
    YTblB[i] := Trunc(0.2392*i);
  end;

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
  tempPdword : pdw;
  i,j : integer;
  Ptr : PByteArray;
  Pitch : integer;
  c: dword;
begin
  //pitch depends on the bitdepth
  Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit

  Ptr := PByteArray(Integer(pParam));
  for i:=0 to VideoInfoStruct.FrameHeight-1 do begin
   for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
     c := YTblB[Ptr^[j*3]] + YTblG[Ptr^[j*3+1]] + YTblR[Ptr^[j*3+2]];
     Ptr^[j*3]   := c;
     Ptr^[j*3+1] := c;
     Ptr^[j*3+2] := c;
   end;
   Ptr := PByteArray(Integer(pParam) + (i*Pitch));
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
      result:=pointer(70);
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
      ParamaterDisplayValue:='dummy value0    ';
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
