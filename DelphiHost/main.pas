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

unit main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, ComCtrls, inifiles;

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
    lNumParameters: TLabel;
    tbParam0: TTrackBar;
    tbParam1: TTrackBar;
    tbParam2: TTrackBar;
    tbParam3: TTrackBar;
    bGetParameterNames: TButton;
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
    lBitDepth: TLabel;
    gbPluginCaps: TGroupBox;
    l16bit: TLabel;
    l24bit: TLabel;
    l32bit: TLabel;
    lProfile: TLabel;
    bPlayAndProcess: TButton;
    bStop: TButton;
    tPlay: TTimer;
    Label3: TLabel;
    bRunIn32bit: TButton;
    cbAutoLoadAVI: TCheckBox;
    lAPIversion: TLabel;
    cbAutoLoadPlugin: TCheckBox;
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
    procedure bGetParameterNamesClick(Sender: TObject);
    procedure bGetParamDefaultsClick(Sender: TObject);
    procedure bGetParamDisplayValuesClick(Sender: TObject);
    procedure tbParam0Change(Sender: TObject);
    procedure bGetParamActualValuesClick(Sender: TObject);
    procedure bGetAndProcessClick(Sender: TObject);
    procedure bBrowseClick(Sender: TObject);
    procedure tbParam1Change(Sender: TObject);
    procedure tbParam2Change(Sender: TObject);
    procedure tbParam3Change(Sender: TObject);
    procedure bPlayAndProcessClick(Sender: TObject);
    procedure tPlayTimer(Sender: TObject);
    procedure bStopClick(Sender: TObject);
    procedure bRunIn32bitClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
  private
    { Private declarations }
    lpBitmapInfoHeader: pBitmapInfoHeader;
    procedure GetPlugins;
    procedure DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader);
    procedure ProfileAndProcessFrame(pFrame: pointer);     // This is the main frame processing procedure
    procedure LoadPlugin;
  public
    { Public declarations }
    AHDC: HDC;                   // Handle
  end;

const
  AppVersion: string='0.52';
  APIversion: string='0.1050';

var
  fmMain: TfmMain;
  CurrentFrame: integer;
  NumFrames: integer;
  CurrentPlug: thandle = 0;
  bits: pointer;
  lpBitmapInfoHeader: pBitmapInfoHeader;
  NumParams: integer;

implementation

uses pluginHost, avi, utils;

var
  p32bitFrame: pointer;

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
  case PluginHost.VideoInfoStruct.BitDepth of
    0: lBitDepth.Caption:='16 bit';
    1: lBitDepth.Caption:='24 bit';
    2: lBitDepth.Caption:='32 bit';
  end;
end;

procedure TfmMain.ebAVIFilenameChange(Sender: TObject);
begin
  AVI.AVIfilename:=ebAVIfilename.Text;
end;

procedure TfmMain.bCloseAVIClick(Sender: TObject);
begin
  if tPlay.Enabled then tPlay.Enabled:=false;
  AVI.CloseAVI;
end;

procedure TfmMain.bGetFrameClick(Sender: TObject);
begin
  inc(currentFrame);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
  displayframe(lpbitmapinfoheader);
end;

procedure TfmMain.bgetInfoClick(Sender: TObject);
var
  tempPluginCaps: array [0..2] of boolean;
  i: integer;
begin
  // Get PluginInfoStruct and display its data
  lGetPluginInfo.caption:=inttostr(PluginHost.GetInfo);
  lPluginMajorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMajorVersion);
  lPluginMinorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMinorVersion);
  lPluginUniqueID.caption:=PluginHost.PluginInfoStruct.PluginUniqueID;
  lPluginName.caption:=PluginHost.PluginInfoStruct.PluginName;
  case PluginHost.PluginInfoStruct.PluginType of
    0: lPluginType.caption:='effect';
    1: lPluginType.caption:='source';
  end;
  // Call GetPluginCaps to see which bitdepths it can manage      todo: make use of this data to decide what to do on process frame etc.
  for i:=0 to 2 do tempPluginCaps[i]:=PluginHost.GetPluginCaps(i);
  if tempPluginCaps[0] then l16bit.Caption:='16bit: yes' else l16bit.Caption:='16bit: no';
  if tempPluginCaps[1] then l24bit.Caption:='24bit: yes' else l24bit.Caption:='24bit: no';
  if tempPluginCaps[2] then l32bit.Caption:='32bit: yes' else l32bit.Caption:='32bit: no';
  if not tempPluginCaps[1] and tempPluginCaps[2] then begin
    VideoInfoStruct.BitDepth:=2;
    lBitDepth.Caption:='32 bit';
    p32bitFrame:=Utils.Make32bitBuffer(VideoInfoStruct);
  end;
end;

procedure TfmMain.FormShow(Sender: TObject);
var
  inifile: TInifile;
begin
  fmMain.Caption:='Delphi FreeFrame Test Container v'+AppVersion;
  lAPIversion.Caption:='for FreeFrame API v'+APIversion;
  // Get current AVI filename from freeframe.ini
  inifile:=Tinifile.Create('FreeFrame.ini');
  with inifile do begin
    ebAVIfilename.Text:=ReadString('Filenames','CurrentAVI','');
    cbAutoLoadAVI.Checked:=ReadBool('HostTestContainerSettings','AutoLoadAVI',true);
    cbAutoLoadPlugin.Checked:=ReadBool('HostTestContainerSettings','AutoLoadPlugin',true);
  end;
  inifile.Free;
  if cbAutoLoadAVI.Checked then begin
    // InitAVI ....
    AVI.Init;
    CurrentFrame:=0;
    // OpenAVI ....
    PluginHost.VideoInfoStruct:=AVI.OpenAVI(ebAVIfilename.text);
    lVideoWidth.caption:=inttostr(PluginHost.VideoInfoStruct.FrameWidth);
    lVideoHeight.caption:=inttostr(PluginHost.VideoInfoStruct.FrameHeight);
    case PluginHost.VideoInfoStruct.BitDepth of
      0: lBitDepth.Caption:='16 bit';
      1: lBitDepth.Caption:='24 bit';
      2: lBitDepth.Caption:='32 bit';
    end;
    // GetFrame ....
    inc(currentFrame);
    lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
    bGetInfo.SetFocus;
  end;
  getPlugins;
  if cbAutoLoadPlugin.Checked then begin
    LoadPlugin;
    bPlayAndProcess.SetFocus;
  end;
  // ...........................................
end;

procedure TfmMain.LoadPlugin;
begin
  // Get Info ...
  bgetInfoClick(nil);
  // Init Plugin ...
  lInitPlugin.caption:=inttostr(PluginHost.Initialise);
  // Get Num Params ...
  bGetNumParametersClick(nil);
  // Get Param Names ...
  bGetParameterNamesClick(nil);
  // Get Param Defaults ...
  bGetParamDefaultsClick(nil);
  // Get Param Display Values ...
  bGetParamDisplayValuesClick(nil);
  // Get Param Actual Values ...
  bGetParamActualValuesClick(nil);
end;


procedure TfmMain.bInitPluginClick(Sender: TObject);
begin
  lInitPlugin.caption:=inttostr(PluginHost.Initialise);
end;

procedure TfmMain.bDeInitPluginClick(Sender: TObject);
begin
  lDeInitPlugin.caption:=inttostr(PluginHost.DeInitialise);
end;

procedure TfmMain.bProcessFrameClick(Sender: TObject);
begin
  ProfileAndProcessFrame(Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER)));
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
    // bits is the pointer to the frame of video - the image data starts immediately after the BitmapInfoHeader in a bitmap
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
  if cbPlugins.Items.Count>0 then begin
    cbPlugins.ItemIndex:=0;
    cbPluginsChange(nil);
  end;
end;

procedure TfmMain.cbPluginsChange(Sender: TObject);
var
  inifile: TInifile;
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
  if cbAutoLoadPlugin.Checked then begin
    LoadPlugin;
    bPlayAndProcess.SetFocus;
  end;
end;

procedure TfmMain.bGetNumParametersClick(Sender: TObject);
begin
  NumParams:=PluginHost.GetNumParameters;
  lNumParameters.caption:='Num Params: '+inttostr(NumParams);
  tbParam0.SliderVisible:=false;
  tbParam1.SliderVisible:=false;
  tbParam2.SliderVisible:=false;
  tbParam3.SliderVisible:=false;
  if NumParams>0 then tbParam0.SliderVisible:=true;
  if NumParams>1 then tbParam1.SliderVisible:=true;
  if NumParams>2 then tbParam2.SliderVisible:=true;
  if NumParams>3 then tbParam3.SliderVisible:=true;
end;

procedure TfmMain.bGetParameterNamesClick(Sender: TObject);
begin
  if NumParams>0 then lParam0Name.caption:=GetParameterName(0);
  if NumParams>1 then lParam1Name.caption:=GetParameterName(1);
  if NumParams>2 then lParam2Name.caption:=GetParameterName(2);
  if NumParams>3 then lParam3Name.caption:=GetParameterName(3);
end;

procedure TfmMain.bGetParamDefaultsClick(Sender: TObject);
begin
  if NumParams>0 then tbParam0.Position:=round(PluginHost.GetParameterDefault(0)*100);
  if NumParams>1 then tbParam1.Position:=round(PluginHost.GetParameterDefault(1)*100);
  if NumParams>2 then tbParam2.Position:=round(PluginHost.GetParameterDefault(2)*100);
  if NumParams>3 then tbParam3.Position:=round(PluginHost.GetParameterDefault(3)*100);
end;

procedure TfmMain.bGetParamDisplayValuesClick(Sender: TObject);
begin
  if NumParams>0 then lParam0Value.caption:=GetParameterDisplay(0);
  if NumParams>1 then lParam1Value.caption:=GetParameterDisplay(1);
  if NumParams>2 then lParam2Value.caption:=GetParameterDisplay(2);
  if NumParams>3 then lParam3Value.caption:=GetParameterDisplay(3);
end;

procedure TfmMain.tbParam0Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
begin
  tempInt:=tbParam0.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(0,tempSingle);
  lParam0Value.caption:=GetParameterDisplay(0);
  tempSingle:=pluginHost.GetParameter(0);
  lParam0dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam1Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
begin
  tempInt:=tbParam1.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(1,tempSingle);
  lParam1Value.caption:=GetParameterDisplay(1);
  tempSingle:=pluginHost.GetParameter(1);
  lParam1dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam2Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
begin
  tempInt:=tbParam2.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(2,tempSingle);
  lParam2Value.caption:=GetParameterDisplay(2);
  tempSingle:=pluginHost.GetParameter(2);
  lParam2dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam3Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
begin
  tempInt:=tbParam3.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(3,tempSingle);
  lParam3Value.caption:=GetParameterDisplay(3);
  tempSingle:=pluginHost.GetParameter(3);
  lParam3dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.bGetParamActualValuesClick(Sender: TObject);
var
  tempSingle: single;
begin
  if NumParams>0 then begin
    tempSingle:=pluginHost.GetParameter(0);
    lParam0dword.caption:=floattostr(tempSingle);
  end;
  if NumParams>1 then begin
    tempSingle:=pluginHost.GetParameter(1);
    lParam1dword.caption:=floattostr(tempSingle);
  end;
  if NumParams>2 then begin
    tempSingle:=pluginHost.GetParameter(2);
    lParam2dword.caption:=floattostr(tempSingle);
  end;
  if NumParams>3 then begin
    tempSingle:=pluginHost.GetParameter(3);
    lParam3dword.caption:=floattostr(tempSingle);
  end;
end;

procedure TfmMain.bGetAndProcessClick(Sender: TObject);
begin
  // Get frame
  inc(currentFrame);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
  // Display it unprocessed ...
  // displayframe(lpbitmapinfoheader);
  // process frame and display it again ...
  ProfileAndProcessFrame(Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER)));
  DisplayFrame(lpbitmapinfoheader);
end;

procedure TfmMain.bBrowseClick(Sender: TObject);
var
  inifile: TInifile;
begin
  if odAVI.Execute then begin
    ebAVIfilename.text:=odAVI.FileName;
    inifile:=Tinifile.Create('FreeFrame.ini');
    inifile.WriteString('Filenames','CurrentAVI',odAVI.filename);
    inifile.Free;
  end;
end;

procedure TfmMain.ProfileAndProcessFrame(pFrame: pointer);     // This is the main frame processing procedure
var
  before: integer;
  pFrameToProcess: pointer;
begin

  pFrameToProcess:=pFrame;

  // Convert to 32bit if plugin only does 32bit
  if VideoInfoStruct.BitDepth=2 then begin
    Utils.Convert24to32(pFrameToProcess, p32bitFrame, VideoInfoStruct);
    pFrameToProcess:=p32bitFrame;
  end;

  // Profile Process the Frame ... 
  before:=gettickcount;
  lProcessFrame.caption:=inttostr(PluginHost.ProcessFrame(pFrameToProcess)); // lpbitmapinfoheader is the current decompressed frame from the mci in the host app
  lProfile.Caption:=inttostr(gettickcount-before)+' msec/frame';

  // Convert it back again if we're running in 32bit plugin
  if VideoInfoStruct.BitDepth=2 then Utils.Convert32to24(p32bitFrame, pFrame, VideoInfoStruct);

end;

procedure TfmMain.bPlayAndProcessClick(Sender: TObject);
begin
  currentFrame:=0;
  tPlay.Enabled:=true;
end;

procedure TfmMain.tPlayTimer(Sender: TObject);
var
  pFrameToProcess, pBits: pointer;
  FrameSize: integer; // in bytes
begin
  // Get frame from AVI if effect - if source just pass on pointer to framebuffer ...
  inc(currentFrame);
  if currentFrame>(numFrames-1) then currentFrame:=1;
  case plugininfostruct.PluginType of
    0: begin // effect
      lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
      pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
      pFrameToProcess:=pBits;
    end;
    1: begin // source
      pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
      pFrameToProcess:=pBits;
    end;
  end;
  // Process frame through plugin
  ProfileAndProcessFrame(pFrameToProcess);
  // Display the frame
  DisplayFrame(lpbitmapinfoheader);
end;

procedure TfmMain.bStopClick(Sender: TObject);
begin
  tPlay.Enabled:=false;
end;

procedure TfmMain.bRunIn32bitClick(Sender: TObject);
begin
  VideoInfoStruct.BitDepth:=2;
  lBitDepth.Caption:='32 bit';
  p32bitFrame:=Utils.Make32bitBuffer(VideoInfoStruct);
end;

procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
var
  inifile: TInifile;
begin
  // Free the 32 bit framebuffer if we're in 32 bit mode ...
  if VideoInfoStruct.bitdepth=2 then utils.free32bitBuffer(p32bitFrame, VideoInfoStruct);
  // Save Settings ...
  inifile:=Tinifile.Create('FreeFrame.ini');
  inifile.WriteBool('HostTestContainerSettings','AutoLoadAVI',cbAutoLoadAVI.checked);
  inifile.WriteBool('HostTestContainerSettings','AutoLoadPlugin',cbAutoLoadPlugin.Checked);
  inifile.Free;
  // CloseDown ...
  CanClose:=true;
end;

end.
