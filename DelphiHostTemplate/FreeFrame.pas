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
unit FreeFrame;

interface

uses
  windows, Classes, Controls, SysUtils, dialogs;

type
  TPlugMainFunction = function(functionCode: dword; pParam: pointer; reserved: dword): pointer; stdcall;
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
  pdw = ^dword;

type
  TFreeFrame = class(TComponent)
  private
    FDirectory: string;
    FPluginIndex: integer;
    FPluginsfound: boolean;
    FPlugins: TStrings;
    FAutoloadplugin: boolean;
    FPluginMajorVersion: integer;
    FPluginMinorVersion: integer;
    FPluginInfo: integer;
    FPluginUniqueID: string;
    FPluginName: string;
    FPluginType: string;
    FSupport16bit : boolean;
    FSupport24bit : boolean;
    FSupport32bit : boolean;
    FNumParameters: integer;
    procedure SetDirectory( Value: string);
    procedure SetPluginIndex( Value: integer);
    procedure SetAutoloadplugin( Value: boolean);
    { Private declarations }
  protected
    { Protected declarations }
  public
    currentPlug : thandle;
    plugMain : tPlugMainFunction;
    PluginInfoStruct: TPluginInfoStruct;
    VideoInfoStruct: TVideoInfoStruct;
    function Getplugins : boolean;
    function Loadplugin : boolean;
    //
    function GetInfo: dword;
    function InitialisePlugin: dword;
    function DeInitialisePlugin: dword;
    function ProcessFrame(pFrame: pointer): dword;
    function GetNumParameters: dword;
    function GetParameterName(Param: dword): string;
    function GetParameterDefault(Param: dword): single;
    function GetParameterDisplay(Param: dword): string;
    function SetParameter(Param: dword; Value: single): dword;
    function GetParameter(Param: dword): single;
    function GetPluginCaps(Param: dword): boolean;
    //
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
  published
    property Pluginsfound : boolean read FPluginsfound;
    property Directory : string read FDirectory write SetDirectory;
    property Plugins : TStrings read FPlugins;
    property PluginIndex : integer read FPluginIndex write SetPluginIndex;
    property Autoloadplugin : boolean read FAutoloadplugin write SetAutoloadplugin default true;
    //
    property PluginMajorVersion : integer read FPluginMajorVersion;
    property PluginInfo : integer read FPluginInfo;
    property PluginMinorVersion : integer read FPluginMinorVersion;
    property PluginUniqueID : string read FPluginUniqueID;
    property PluginName : string read FPluginName;
    property PluginType : string read FPluginType;
    //
    property Support16bit : boolean read FSupport16bit;
    property Support24bit : boolean read FSupport24bit;
    property Support32bit : boolean read FSupport32bit;
    //
    property NumParameters : integer read FNumParameters write FNumParameters;

    { Published declarations }
  end;

procedure Register;

implementation

uses Math;


procedure Register;
begin
  RegisterComponents('FreeFrame', [TFreeFrame]);
end;

{ TFreeFrame }

constructor TFreeFrame.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FPlugins := TStringList.Create;
  currentPlug := 0;
  //
end;

function TFreeFrame.DeInitialisePlugin: dword;
begin
  result:=dword(plugMain(2,nil,0));
end;

destructor TFreeFrame.Destroy;
begin
  FPlugins.Free;
  inherited Destroy;
end;

function TFreeFrame.GetInfo: dword;
var
  dllReturnValue: pointer;
  functionCode: dword;
  pPluginInfoStruct: pointer;
  pParam: PDword;
  tempPChar: PChar;
  i,j: integer;
  tempDword: dword;
  tempString: string;
  tempFCC: array [0..3] of char;
  tempName: array [0..15] of char;
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

function TFreeFrame.GetNumParameters: dword;
begin
  result:=dword(plugMain(4,nil,0));
end;

function TFreeFrame.GetParameter(Param: dword): single;
var
  tempDword: dword;
  tempPdword: pdw;
  tempSingle: single;
  tempPsingle: pointer;
begin
  tempdword:=dword(plugMain(9,pointer(Param),0));
  tempPdword:=@tempDword;
  tempPsingle:=@tempSingle;
  copymemory(tempPsingle,tempPdword,4);
  result:=tempSingle;
end;

function TFreeFrame.GetParameterDefault(Param: dword): single;
var
  tempSingle: single;
  tempDword: dword;
begin
  tempDword:=dword(plugMain(6,pointer(Param),0));
  copymemory(@tempSingle,@tempDword,4);
  result:=tempSingle;
end;

function TFreeFrame.GetParameterDisplay(Param: dword): string;
var
  tempParamDisplay: array [0..15] of char;
  tempSourcePointer: pdw;
  tempDestPointer: pdw;
  i: integer;
begin
  tempSourcePointer:=pdw(plugMain(7,pointer(Param),0));
  tempDestPointer:=pdw(@tempParamDisplay);
  for i:=0 to 3 do begin
    tempDestPointer^:=tempSourcePointer^;
    inc(tempSourcePointer);
    inc(tempDestPointer);
  end;
  result:=string(tempParamDisplay);
end;

function TFreeFrame.GetParameterName(Param: dword): string;
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

function TFreeFrame.GetPluginCaps(Param: dword): boolean;
begin
  case dword(plugMain(10,pointer(Param),0)) of
    0: result:=false;
    1: result:=true;
  else result:=false;  
  end;
end;

function TFreeFrame.InitialisePlugin: dword;
var
  pVideoInfoStruct: pointer;
begin
  pVideoInfoStruct:=pointer(@VideoInfoStruct);
  result:=dword(plugMain(1,pVideoInfoStruct,0));
end;

function TFreeFrame.ProcessFrame(pFrame: pointer): dword;
begin
  result:=dword(plugMain(3,pFrame,0));
end;

function TFreeFrame.GetPlugins: boolean;
  function checkValid(name: string):boolean;
  const
    functionName = 'plugMain';
  var
    h: thandle;
    proc: pointer;
  begin
    result:=false;
    if not (compareText(copy(name,length(name)-3,4),'.dll')=0) then exit; //findfirst returns filenames containing dll, not just ending
    h := LoadLibrary(pchar(name));
    if h <> 0 then begin //its a dll!
      Proc := GetProcAddress(h, functionName);
      if Proc <> nil then result:=true;
      FreeLibrary(h);
    end;
  end;
var
  t: TSearchRec;
begin
  FPlugins.clear;

  if findfirst(FDirectory+'*.dll', faAnyFile, t) = 0 then begin
    if checkValid(FDirectory+t.name) then FPlugins.add(t.name);
    while findnext(t) = 0 do if checkValid(FDirectory+t.name) then FPlugins.add(t.name);
    findclose(t);
  end;
  if FPlugins.Count>0 then begin
    FPluginsfound := true;
    FPluginIndex := 0;
  end else FPluginsfound := False;

end;


procedure TFreeFrame.SetDirectory( Value: string);
begin
  FDirectory := Value;
end;

function TFreeFrame.SetParameter(Param: dword; Value: single): dword;
type
  TSetParamStruct = array [0..1] of dword;
var
  SetParamStruct: TSetParamStruct;
  tempPdword: pdw;
begin
  SetParamStruct[0]:=Param;
  tempPdword:=@value;
  SetParamStruct[1]:=tempPdword^;
  result:=dword(plugMain(8,@SetParamStruct,0));
end;

procedure TFreeFrame.SetPluginIndex( Value: integer);
begin
  If not FPluginsfound then EXIT;

  //if FPluginIndex = Value then Exit;
  FPluginIndex := Value;

  if FPluginIndex < 0 then exit;

  if currentPlug<>0 then begin
   DeInitialisePlugin;
   freeLibrary(currentPlug);
  end;

  plugMain:=nil;

  currentPlug := LoadLibrary(pchar(FDirectory +'/'+FPlugins.Strings[PluginIndex]));
  if currentPlug <> 0 then begin //its a dll!
    plugMain := GetProcAddress(currentPlug, 'plugMain');
    if @plugMain = nil then begin
      FreeLibrary(currentPlug);
      currentPlug:=0;
    end;
  end;
  if not assigned(plugMain) then showmessage('problem loading plugin');

  if AutoLoadPlugin then begin
    // Load new plugin
    LoadPlugin;
  end;

end;

procedure TFreeFrame.SetAutoloadplugin( Value: boolean);
begin
  FAutoloadplugin := Value;
end;

function TFreeFrame.Loadplugin: boolean;
var
  tempPluginCaps: array [0..2] of boolean;
  i: integer;
begin
  FPluginInfo         := GetInfo;
  FPluginMajorVersion := PluginInfoStruct.APIMajorVersion;
  FPluginMinorVersion := PluginInfoStruct.APIMinorVersion;
  FPluginUniqueID     := PluginInfoStruct.PluginUniqueID;
  FPluginName         := PluginInfoStruct.PluginName;
  case PluginInfoStruct.PluginType of
    0: FPluginType := 'effect';
    1: FPluginType := 'source';
  end;
  // Call GetPluginCaps to see which bitdepths it can manage
  for i:=0 to 2 do tempPluginCaps[i] := GetPluginCaps(i);
  FSupport16bit := tempPluginCaps[0];
  FSupport24bit := tempPluginCaps[1];
  FSupport32bit := tempPluginCaps[2];
  //
  FNumParameters := GetNumParameters;
  InitialisePlugin;
end;

end.
