// FreeFrame Open Video Plugin Host Test Container Prototype
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

// C:\gig av\03Lavalamp A\gloup1bottomviewx.avi

unit main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, ComCtrls;

type
  TfmMain = class(TForm)
    bInit: TButton;
    bDeInit: TButton;
    bOpenAVI: TButton;
    ebAVIFilename: TEdit;
    bCloseAVI: TButton;
    bGetFrame: TButton;
    PaintBox1: TPaintBox;
    bgetInfo: TButton;
    lTestContainerVersion: TLabel;
    GroupBox1: TGroupBox;
    lPluginMajorVersion: TLabel;
    lPluginMinorVersion: TLabel;
    lPluginName: TLabel;
    lPluginType: TLabel;
    lPluginUniqueID: TLabel;
    GroupBox2: TGroupBox;
    lVideoWidth: TLabel;
    lVideoHeight: TLabel;
    bInitPlugin: TButton;
    bProcessFrame: TButton;
    bDeInitPlugin: TButton;
    lInitPlugin: TLabel;
    lDeInitPlugin: TLabel;
    lGetPluginInfo: TLabel;
    lProcessFrame: TLabel;
    cbPlugins: TComboBox;
    Label1: TLabel;
    bGetNumParameters: TButton;
    lNumParamaters: TLabel;
    tbParam0: TTrackBar;
    tbParam1: TTrackBar;
    tbParam2: TTrackBar;
    tbParam3: TTrackBar;
    bGetParamaterNames: TButton;
    lParam0Name: TLabel;
    lParam1Name: TLabel;
    lParam2Name: TLabel;
    lParam3Name: TLabel;
    bGetParamDefaults: TButton;
    lParam0Value: TLabel;
    lParam1Value: TLabel;
    lParam2Value: TLabel;
    lParam3Value: TLabel;
    bGetParamDisplayValues: TButton;
    bGetParamActualValues: TButton;
    lParam0dword: TLabel;
    lParam1dword: TLabel;
    lParam2dword: TLabel;
    lParam3dword: TLabel;
    bGetAndProcess: TButton;
    bBrowse: TButton;
    odAVI: TOpenDialog;
    Label2: TLabel;
    procedure bInitClick(Sender: TObject);
    procedure bDeInitClick(Sender: TObject);
    procedure bOpenAVIClick(Sender: TObject);
    procedure ebAVIFilenameChange(Sender: TObject);
    procedure bCloseAVIClick(Sender: TObject);
    procedure bGetFrameClick(Sender: TObject);
    procedure bgetInfoClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure bInitPluginClick(Sender: TObject);
    procedure bDeInitPluginClick(Sender: TObject);
    procedure bProcessFrameClick(Sender: TObject);
    procedure cbPluginsChange(Sender: TObject);
    procedure bGetNumParametersClick(Sender: TObject);
    procedure bGetParamaterNamesClick(Sender: TObject);
    procedure bGetParamDefaultsClick(Sender: TObject);
    procedure bGetParamDisplayValuesClick(Sender: TObject);
    procedure tbParam0Change(Sender: TObject);
    procedure bGetParamActualValuesClick(Sender: TObject);
    procedure bGetAndProcessClick(Sender: TObject);
    procedure bBrowseClick(Sender: TObject);
  private
    { Private declarations }
    lpBitmapInfoHeader: pBitmapInfoHeader;
    procedure GetPlugins;
    procedure DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader);
  public
    { Public declarations }
    AHDC: HDC;                   // Handle
  end;

const
  version: string='0.1021';

var
  fmMain: TfmMain;
  CurrentFrame: integer;
  CurrentPlug: thandle = 0;
  bits: pointer;
  lpBitmapInfoHeader: pBitmapInfoHeader;
  NumParams: integer;

implementation

uses pluginHost, avi;

{$R *.DFM}

procedure TfmMain.bInitClick(Sender: TObject);
begin
  AVI.Init;
  CurrentFrame:=0;
end;

procedure TfmMain.bDeInitClick(Sender: TObject);
begin
  AVI.DeInit;
end;

procedure TfmMain.bOpenAVIClick(Sender: TObject);
begin
  PluginHost.VideoInfoStruct:=AVI.OpenAVI(ebAVIfilename.text);
  lVideoWidth.caption:=inttostr(PluginHost.VideoInfoStruct.FrameWidth);
  lVideoHeight.caption:=inttostr(PluginHost.VideoInfoStruct.FrameHeight);
end;

procedure TfmMain.ebAVIFilenameChange(Sender: TObject);
begin
  AVI.AVIfilename:=ebAVIfilename.Text;
end;

procedure TfmMain.bCloseAVIClick(Sender: TObject);
begin
  AVI.CloseAVI;
end;

procedure TfmMain.bGetFrameClick(Sender: TObject);
begin
  inc(currentFrame);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
  displayframe(lpbitmapinfoheader);
end;

procedure TfmMain.bgetInfoClick(Sender: TObject);
begin
  lGetPluginInfo.caption:=inttostr(PluginHost.GetPluginInfoStruct);
  lPluginMajorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMajorVersion);
  lPluginMinorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMinorVersion);
  lPluginUniqueID.caption:=PluginHost.PluginInfoStruct.PluginUniqueID;
  lPluginName.caption:=PluginHost.PluginInfoStruct.PluginName;
  case PluginHost.PluginInfoStruct.PluginType of
    0: lPluginType.caption:='effect';
    1: lPluginType.caption:='source';
  end;
end;

procedure TfmMain.FormShow(Sender: TObject);
begin
  lTestContainerVersion.Caption:='v'+version;
  getPlugins;
end;

procedure TfmMain.bInitPluginClick(Sender: TObject);
begin
  lInitPlugin.caption:=inttostr(PluginHost.InitPlugin);
end;

procedure TfmMain.bDeInitPluginClick(Sender: TObject);
begin
  lDeInitPlugin.caption:=inttostr(PluginHost.DeInitPlugin);
end;

procedure TfmMain.bProcessFrameClick(Sender: TObject);
begin
  bits := Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
  lProcessFrame.caption:=inttostr(PluginHost.ProcessFrame(bits)); // lpbitmapinfoheader is the current decompressed frame from the mci in the host app
  DisplayFrame(lpbitmapinfoheader);
end;

procedure TfmMain.DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader);
type
  pdw = ^dword;
var
  hbmp:thandle;
  bits:pdw;
  tempBitmap: TBitmap;
begin
  AHDC := getdc(fmMain.handle);
  try
    bits := Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
    hBmp := CreateDIBitmap(ahdc,                  // handle of device context
               lpBitmapInfoHeader^,               // address of bitmap size and format data
               CBM_INIT,                          // initialization flag
               pointer(bits),                     // address of initialization data
               PBITMAPINFO(lpBitmapInfoHeader)^,  // address of bitmap color-format data
               DIB_RGB_COLORS );                  // color-data usage
    tempBitmap:=TBitmap.create;
    try
      tempBitmap.Handle:=hBmp;
      with PaintBox1 do Canvas.StretchDraw(rect(0,0,width,height),tempBitmap);
    finally
      tempBitmap.free;
    end;
  finally
    releaseDC(fmMain.handle,AHDC);
  end;
end;

procedure TfmMain.GetPlugins;
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
  plugins: string;
  t: TSearchRec;
begin
  cbPlugins.items.clear;
  plugins := extractfilepath(application.exename) + 'plugins\';
  if findfirst(plugins+'*.dll', faAnyFile, t) = 0 then begin
    if checkValid(plugins+t.name) then cbPlugins.items.add(t.name);
    while findnext(t) = 0 do if checkValid(plugins+t.name) then cbPlugins.items.add(t.name);
    findclose(t);
  end;
end;


procedure TfmMain.cbPluginsChange(Sender: TObject);
begin
  if cbPlugins.itemindex<0 then exit;
  plugMain:=nil;
  if currentPlug<>0 then freeLibrary(currentPlug);
  currentPlug := LoadLibrary(pchar(extractfilepath(application.exename)+'/plugins/'+cbPlugins.items[cbPlugins.itemindex]));
  if currentPlug <> 0 then begin //its a dll!
    plugMain := GetProcAddress(currentPlug, 'plugMain');
    if @plugMain = nil then begin
      FreeLibrary(currentPlug);
      currentPlug:=0;
    end;
  end;
  if not assigned(plugMain) then showmessage('problem loading plugin');
end;

procedure TfmMain.bGetNumParametersClick(Sender: TObject);
begin
  NumParams:=PluginHost.GetNumParamaters;
  lNumParamaters.caption:='Num Params: '+inttostr(NumParams);
  tbParam0.SliderVisible:=false;
  tbParam1.SliderVisible:=false;
  tbParam2.SliderVisible:=false;
  tbParam3.SliderVisible:=false;
  if NumParams>0 then tbParam0.SliderVisible:=true;
  if NumParams>1 then tbParam1.SliderVisible:=true;
  if NumParams>2 then tbParam2.SliderVisible:=true;
  if NumParams>3 then tbParam3.SliderVisible:=true;
end;

procedure TfmMain.bGetParamaterNamesClick(Sender: TObject);
begin
  if NumParams>0 then lParam0Name.caption:=GetParamaterName(0);
  if NumParams>1 then lParam1Name.caption:=GetParamaterName(1);
  if NumParams>2 then lParam2Name.caption:=GetParamaterName(2);
  if NumParams>3 then lParam3Name.caption:=GetParamaterName(3);
end;

procedure TfmMain.bGetParamDefaultsClick(Sender: TObject);
begin
  if NumParams>0 then tbParam0.Position:=round(PluginHost.GetParamaterDefault(0)*100);
  if NumParams>1 then tbParam1.Position:=round(PluginHost.GetParamaterDefault(1)*100);
  if NumParams>2 then tbParam2.Position:=round(PluginHost.GetParamaterDefault(2)*100);
  if NumParams>3 then tbParam3.Position:=round(PluginHost.GetParamaterDefault(3)*100);
end;

procedure TfmMain.bGetParamDisplayValuesClick(Sender: TObject);
begin
  if NumParams>0 then lParam0Value.caption:=GetParamaterDisplay(0);
  if NumParams>1 then lParam1Value.caption:=GetParamaterDisplay(1);
  if NumParams>2 then lParam2Value.caption:=GetParamaterDisplay(2);
  if NumParams>3 then lParam3Value.caption:=GetParamaterDisplay(3);
end;

procedure TfmMain.tbParam0Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
begin
  tempInt:=tbParam0.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(0,tempSingle);
end;

procedure TfmMain.bGetParamActualValuesClick(Sender: TObject);
var
  tempSingle: single;
begin
  tempSingle:=pluginHost.GetParameter(0);
  if NumParams>0 then lParam0dword.caption:=floattostr(tempSingle);
  //if NumParams>0 then lParam0dword.caption:=inttostr(round(pluginHost.GetParameter(0)));
  //if NumParams>1 then lParam1dword.caption:=floattostr(single(pluginHost.GetParameter(1)));
  //if NumParams>2 then lParam2dword.caption:=floattostr(single(pluginHost.GetParameter(2)));
  //if NumParams>3 then lParam3dword.caption:=floattostr(single(pluginHost.GetParameter(3)));
end;

procedure TfmMain.bGetAndProcessClick(Sender: TObject);
begin
  // Get frame
  inc(currentFrame);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
  // Display it unprocessed
  // displayframe(lpbitmapinfoheader);
  // process frame and display it again
  bits := Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
  lProcessFrame.caption:=inttostr(PluginHost.ProcessFrame(bits)); // lpbitmapinfoheader is the current decompressed frame from the mci in the host app
  DisplayFrame(lpbitmapinfoheader);
end;

procedure TfmMain.bBrowseClick(Sender: TObject);
begin
  if odAVI.Execute then ebAVIfilename.text:=odAVI.FileName;
end;

end.
