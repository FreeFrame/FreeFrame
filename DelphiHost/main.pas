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

//  v1.003
//  source plugin shutdown VFW for testing PeteLiveFeed
//    - and trying to get petelivefeed composite source live effects plugins working

// I think this is the v1.002 additions ...
// trap AVI disappeared
// trap auto loading plugins when no avi loaded
// trap auto loading plugins when there is no /plugins dir there or no plugins in it
// clear Param names before loading new ones
// trap pressing play when there is no plugin loaded / no avi loaded
// Few more comments

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
    lFFMajorVersion: TLabel;
    lFFMinorVersion: TLabel;
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
    cbPluginProcessFrames: TCheckBox;
    GroupBox3: TGroupBox;
    lPluginMajorVersion: TLabel;
    lPluginMinorVersion: TLabel;
    lOrientation: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    lParam4Name: TLabel;
    lParam5Name: TLabel;
    lParam6Name: TLabel;
    lParam7Name: TLabel;
    lParam4Value: TLabel;
    lParam5Value: TLabel;
    lParam6value: TLabel;
    lParam7Value: TLabel;
    lParam4Dword: TLabel;
    lParam5Dword: TLabel;
    lParam6Dword: TLabel;
    lParam7Dword: TLabel;
    tbParam4: TTrackBar;
    tbParam5: TTrackBar;
    tbParam6: TTrackBar;
    tbParam7: TTrackBar;
    gbSecondInstance: TGroupBox;
    PaintBox2: TPaintBox;
    ebSIfilename: TEdit;
    bSIbrowse: TButton;
    bSIrunUpAVI: TButton;
    bSIshutDownAVI: TButton;
    lSIParam0Name: TLabel;
    lSIParam0value: TLabel;
    tbSISlider: TTrackBar;
    lSIparam0dword: TLabel;
    bRunUpSI: TButton;
    bShutDownSI: TButton;
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
    procedure tbParamChange(Sender: TObject);                 // going general
    procedure bGetParamActualValuesClick(Sender: TObject);
    procedure bGetAndProcessClick(Sender: TObject);
    procedure bBrowseClick(Sender: TObject);
    procedure bPlayAndProcessClick(Sender: TObject);
    procedure tPlayTimer(Sender: TObject);
    procedure bStopClick(Sender: TObject);
    procedure bRunIn32bitClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure FormCreate(Sender: TObject);
    procedure bSIbrowseClick(Sender: TObject);
    procedure bSIrunUpAVIClick(Sender: TObject);
    procedure bSIshutDownAVIClick(Sender: TObject);
    procedure bRunUpSIClick(Sender: TObject);
    procedure bShutDownSIClick(Sender: TObject);
    procedure tbSISliderChange(Sender: TObject);
  private
    { Private declarations }
    lpBitmapInfoHeader: pBitmapInfoHeader;
    StartingApp: boolean;
    Sliders: array [0..7] of TTrackbar;
    ParamNames: array [0..7] of TLabel;
    ParamValues: array [0..7] of TLabel;
    ParamDwords: array [0..7] of TLabel;
    procedure GetPlugins;
    procedure DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader; channel: integer);
    procedure ProfileAndProcessFrame(pFrame: pointer; PluginInstance: dword);     // This is the main frame processing procedure
    procedure LoadPlugin;
  public
    { Public declarations }
    AHDC: HDC;                   // Handle
  end;

const
  AppVersion: string='1.003';
  APIversion: string='1.000';

var
  fmMain: TfmMain;
  CurrentFrame: array [0..1] of integer;
  NumFrames: array [0..1] of integer;
  CurrentPlug: thandle = 0;
  bits: pointer;
  lpBitmapInfoHeader: pBitmapInfoHeader;
  NumParams: integer;
  PluginLoaded: boolean;
  PluginInstance: array [0..1] of dword; // Plugin Instance Identifier
  InstanceReady: array [0..1] of boolean;
  //AVIready: array [0..1] of boolean;

implementation

uses pluginHost, avi, utils;

var
  p32bitFrame: pointer;

{$R *.DFM}

procedure TfmMain.bInitClick(Sender: TObject);
begin
  AVI.Init;
  CurrentFrame[0]:=0;
end;

procedure TfmMain.bDeInitClick(Sender: TObject);
begin
  AVI.DeInit;
end;

procedure TfmMain.bOpenAVIClick(Sender: TObject);
begin
  PluginHost.VideoInfoStruct[0]:=AVI.OpenAVI(ebAVIfilename.text,0);
  // Display VideoInfoStruct ...
  lVideoWidth.caption:=inttostr(PluginHost.VideoInfoStruct[0].FrameWidth);
  lVideoHeight.caption:=inttostr(PluginHost.VideoInfoStruct[0].FrameHeight);
  case PluginHost.VideoInfoStruct[0].BitDepth of
    0: lBitDepth.Caption:='16 bit';
    1: lBitDepth.Caption:='24 bit';
    2: lBitDepth.Caption:='32 bit';
  end;
  case VideoInfoStruct[0].orientation of
    1: lOrientation.caption:='Right Way Up';
    2: lOrientation.caption:='Upside Down';
  end;
end;

procedure TfmMain.ebAVIFilenameChange(Sender: TObject);
begin
  AVI.AVIfilename:=ebAVIfilename.Text;
end;

procedure TfmMain.bCloseAVIClick(Sender: TObject);
begin
  if tPlay.Enabled then tPlay.Enabled:=false;
  //AVIready[0]:=false;
  AVI.CloseAVI(0);
end;

procedure TfmMain.bGetFrameClick(Sender: TObject);
begin
  inc(currentFrame[0]);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame[0],0);
  displayframe(lpbitmapinfoheader,0);
end;

procedure TfmMain.bgetInfoClick(Sender: TObject);
var
  tempPluginCaps: array [0..2] of boolean;
  i: integer;
begin
  // Get PluginInfoStruct and display its data
  lGetPluginInfo.caption:=inttostr(PluginHost.GetInfo);
  lFFMajorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMajorVersion);
  lFFMinorVersion.caption:=inttostr(PluginHost.PluginInfoStruct.APIMinorVersion);
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
    VideoInfoStruct[0].BitDepth:=2;
    lBitDepth.Caption:='32 bit';
    p32bitFrame:=Utils.Make32bitBuffer(VideoInfoStruct[0]);
  end;
  // Set Video orientation to upside down (VFW)
  VideoInfoStruct[0].orientation:=2;
  lOrientation.caption:='Upside Down';
  // Get PluginExtendedInfoStruct and display its data
  PluginHost.getExtendedInfo;
  lPluginMajorVersion.caption:=inttostr(PluginHost.PluginExtendedInfoStruct.PluginMajorVersion);
  lPluginMinorVersion.caption:=inttostr(PluginHost.PluginExtendedInfoStruct.PluginMinorVersion);
end;

procedure TfmMain.FormShow(Sender: TObject);
var
  inifile: TInifile;
  tempFilename: string;
begin
  fmMain.Caption:='FreeFrame Plugin Tester v'+AppVersion;
  lAPIversion.Caption:='for FreeFrame API v'+APIversion;
  // Get current AVI filename from freeframe.ini
  inifile:=Tinifile.Create('FreeFrame.ini');
  with inifile do begin
    tempFilename:=ReadString('Filenames','CurrentAVI','');
    cbAutoLoadAVI.Checked:=ReadBool('HostTestContainerSettings','AutoLoadAVI',true);
    cbAutoLoadPlugin.Checked:=ReadBool('HostTestContainerSettings','AutoLoadPlugin',true);
  end;
  inifile.Free;
  if fileExists(tempFilename) then begin
    ebAVIfilename.Text:=tempFilename;
    if cbAutoLoadAVI.Checked then begin
      // InitAVI ....
      AVI.Init;
      CurrentFrame[0]:=0;
      // OpenAVI ....
      PluginHost.VideoInfoStruct[0]:=AVI.OpenAVI(ebAVIfilename.text,0);
      lVideoWidth.caption:=inttostr(PluginHost.VideoInfoStruct[0].FrameWidth);
      lVideoHeight.caption:=inttostr(PluginHost.VideoInfoStruct[0].FrameHeight);
      case PluginHost.VideoInfoStruct[0].BitDepth of
        0: lBitDepth.Caption:='16 bit';
        1: lBitDepth.Caption:='24 bit';
        2: lBitDepth.Caption:='32 bit';
      end;
      // GetFrame ....
      inc(currentFrame[0]);
      lpbitmapinfoheader:=AVI.GetFrame(currentFrame[0],0);
      bGetInfo.SetFocus;
    end;
  end;
  getPlugins;
  if (cbAutoLoadPlugin.Checked) and avi.AVIopen[0] and (cbPlugins.Items.Count>0) then begin
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
  lInitPlugin.caption:=inttostr(PluginHost.InitialisePlugin);
  PluginInstance[0]:=PluginHost.InstantiatePlugin(VideoInfoStruct[0]);
  PluginLoaded:=true; // todo: when plugins are returning correct init responses we could only set pluginLoaded to true when we get a success response from the plugin
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
  InstanceReady[0]:=true;
end;

procedure TfmMain.bInitPluginClick(Sender: TObject);
begin
  lInitPlugin.caption:=inttostr(PluginHost.InitialisePlugin);
  PluginInstance[0]:=PluginHost.InstantiatePlugin(VideoInfoStruct[0]);
  PluginLoaded:=true; // todo: when plugins are returning correct init responses we could only set pluginLoaded to true when we get a success response from the plugin
  InstanceReady[0]:=true; // todo: not sure if this is totally sorted yet
end;

procedure TfmMain.bDeInitPluginClick(Sender: TObject);
begin
  if tPlay.Enabled then tPlay.Enabled:=false;
  DeInstantiatePlugin(PluginInstance[0]);
  InstanceReady[0]:=false;
  lDeInitPlugin.caption:=inttostr(PluginHost.DeInitialisePlugin);
end;

procedure TfmMain.bProcessFrameClick(Sender: TObject);
begin
  ProfileAndProcessFrame(Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER)),PluginInstance[0]);
  DisplayFrame(lpbitmapinfoheader,0);
end;

procedure TfmMain.DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader; channel: integer);
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
      case channel of
        0: with PaintBox1 do Canvas.StretchDraw(rect(0,0,width,height),tempBitmap);
        1: with PaintBox2 do Canvas.StretchDraw(rect(0,0,width,height),tempBitmap);
      end;
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

procedure TfmMain.cbPluginsChange(Sender: TObject);     // to do: sort out this vs loadnew plugin
var
  inifile: TInifile;
begin
  if cbAutoLoadPlugin.Checked and not startingApp then begin
    instanceReady[0]:=false;
    // Run down this instance 0 of this plugin
    PluginHost.DeInstantiatePlugin(PluginInstance[0]);
    // DeInit Plugin
    lDeInitPlugin.caption:=inttostr(PluginHost.DeInitialisePlugin);
  end;
  StartingApp:=false;
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
    // Load new plugin
    LoadPlugin;
    //bPlayAndProcess.SetFocus;
  end;
end;

procedure TfmMain.bGetNumParametersClick(Sender: TObject);
var
  i: integer;
begin
  NumParams:=PluginHost.GetNumParameters;
  lNumParameters.caption:='Num Params: '+inttostr(NumParams);
  // Clear Sliders ...
  for i:=0 to 7 do Sliders[i].SliderVisible:=false;
  // Enable Sliders implemented by this plugin ...
  for i:=0 to 7 do if NumParams>i then Sliders[i].SliderVisible:=true;
end;

procedure TfmMain.bGetParameterNamesClick(Sender: TObject);
var
  i: integer;
begin
  // Clear Param Names ...
  for i:=0 to 7 do ParamNames[i].caption:='';
  // Load new Param Names ...
  for i:=0 to 7 do if NumParams>i then ParamNames[i].caption:=GetParameterName(i);
end;

procedure TfmMain.bGetParamDefaultsClick(Sender: TObject);
var
  i: integer;
begin
  // Get Parameter Default Values ...
  for i:=0 to 7 do if NumParams>i then Sliders[i].Position:=round(PluginHost.GetParameterDefault(i)*100);
end;

procedure TfmMain.bGetParamDisplayValuesClick(Sender: TObject);
var
  i: integer;
begin
  // Get Parameter Display Values ...
  for i:=0 to 7 do if NumParams>i then ParamValues[i].Caption:=GetParameterDisplay(i,PluginInstance[0]);
end;

procedure TfmMain.tbParamChange(Sender: TObject);    // the OnChange procedure for all the param sliders
var
  tempInt: integer;
  tempSingle: single;
  tempTrackbar: Ttrackbar;
  tempParam: integer;
begin
  if not (sender is Ttrackbar) then exit;
  tempTrackbar:=(sender as TTrackbar);
  tempParam:=tempTrackbar.Tag;
  tempInt:=tempTrackbar.position;
  tempSingle:=tempInt/100;
  PluginHost.SetParameter(tempParam,tempSingle,PluginInstance[0]);
  ParamValues[tempParam].caption:=GetParameterDisplay(tempParam,PluginInstance[0]);
  tempSingle:=pluginHost.GetParameter(tempParam,PluginInstance[0]);
  ParamDwords[tempParam].caption:=floattostr(tempSingle);
end;

procedure TfmMain.bGetParamActualValuesClick(Sender: TObject);
var
  tempSingle: single;
  i: integer;
begin
  for i:=0 to 7 do begin
    if NumParams>i then begin
      // Get Parameter actual Values ...
      tempSingle:=pluginHost.GetParameter(i,PluginInstance[0]);
      ParamDwords[i].caption:=floattostr(tempSingle);
    end;
  end;
end;

procedure TfmMain.bGetAndProcessClick(Sender: TObject);
begin
  // Get frame
  inc(currentFrame[0]);
  lpbitmapinfoheader:=AVI.GetFrame(currentFrame[0],0);
  // Display it unprocessed ...
  // displayframe(lpbitmapinfoheader);
  // process frame and display it again ...
  ProfileAndProcessFrame(Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER)),PluginInstance[0]);
  DisplayFrame(lpbitmapinfoheader,0);
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

procedure TfmMain.ProfileAndProcessFrame(pFrame: pointer; PluginInstance: dword);     // This is the main frame processing procedure
var
  before: integer;
  pFrameToProcess: pointer;
begin

  pFrameToProcess:=pFrame;

  // Convert to 32bit if plugin only does 32bit
  if VideoInfoStruct[0].BitDepth=2 then begin
    Utils.Convert24to32(pFrameToProcess, p32bitFrame, VideoInfoStruct[0]);
    pFrameToProcess:=p32bitFrame;
  end;

  // Profile Process the Frame ...
  before:=gettickcount;
  lProcessFrame.caption:=inttostr(PluginHost.ProcessFrame(pFrameToProcess, PluginInstance)); // lpbitmapinfoheader is the current decompressed frame from the mci in the host app
  lProfile.Caption:=inttostr(gettickcount-before)+' msec/frame';

  // Convert it back again if we're running in 32bit plugin
  if VideoInfoStruct[0].BitDepth=2 then Utils.Convert32to24(p32bitFrame, pFrame, VideoInfoStruct[0]);

end;

procedure TfmMain.bPlayAndProcessClick(Sender: TObject);
begin
  if not AVI.AVIopen[0] then exit;
  if not PluginLoaded then exit;
  currentFrame[0]:=0;
  tPlay.Enabled:=true;
  cbPlugins.SetFocus;
end;

procedure TfmMain.tPlayTimer(Sender: TObject);
var
  pFrameToProcess, pBits: pointer;
  FrameSize: integer; // in bytes
begin
  if InstanceReady[0] then begin
    // Get frame from AVI if effect - if source just pass on pointer to framebuffer ...
    inc(currentFrame[0]);
    if currentFrame[0]>(numFrames[0]-1) then currentFrame[0]:=1;
    case plugininfostruct.PluginType of
      0: begin // effect
        lpbitmapinfoheader:=AVI.GetFrame(currentFrame[0],0);
        pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
        pFrameToProcess:=pBits;
      end;
      1: begin // source
        pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
        pFrameToProcess:=pBits;
      end;
    end;
    // Process frame through plugin
    if cbPluginProcessFrames.Checked then ProfileAndProcessFrame(pFrameToProcess, PluginInstance[0]);
    // Display the frame
    DisplayFrame(lpbitmapinfoheader,0);
  end;
  if InstanceReady[1] then begin
    // Get frame from AVI if effect - if source just pass on pointer to framebuffer ...
    inc(currentFrame[1]);
    if currentFrame[1]>(numFrames[1]-1) then currentFrame[1]:=1;
    case plugininfostruct.PluginType of
      0: begin // effect
        lpbitmapinfoheader:=AVI.GetFrame(currentFrame[1],1);      // channel 1
        pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
        pFrameToProcess:=pBits;
      end;
      1: begin // source
        pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));
        pFrameToProcess:=pBits;
      end;
    end;
    // Process frame through plugin
    ProfileAndProcessFrame(pFrameToProcess, PluginInstance[1]);
    // Display the frame
    DisplayFrame(lpbitmapinfoheader,1);
  end;
end;

procedure TfmMain.bStopClick(Sender: TObject);
begin
  tPlay.Enabled:=false;
end;

procedure TfmMain.bRunIn32bitClick(Sender: TObject);
begin
  VideoInfoStruct[0].BitDepth:=2;
  lBitDepth.Caption:='32 bit';
  p32bitFrame:=Utils.Make32bitBuffer(VideoInfoStruct[0]);
end;

procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
var
  inifile: TInifile;
  i: integer;
begin
  // stop playing ... 
  if tPlay.Enabled then tPlay.Enabled:=false;
  // Free the 32 bit framebuffer if we're in 32 bit mode ...
  if VideoInfoStruct[0].bitdepth=2 then utils.free32bitBuffer(p32bitFrame, VideoInfoStruct[0]);
  // free plugin
  if pluginloaded then begin
    for i:=0 to 1 do if instanceReady[i] then DeInstantiatePlugin(PluginInstance[i]);
    DeInitialisePlugin;
  end;
  // Save Settings ...
  inifile:=Tinifile.Create('FreeFrame.ini');
  inifile.WriteBool('HostTestContainerSettings','AutoLoadAVI',cbAutoLoadAVI.checked);
  inifile.WriteBool('HostTestContainerSettings','AutoLoadPlugin',cbAutoLoadPlugin.Checked);
  inifile.Free;
  // freeAVI(s)
  for i:=0 to 1 do if avi.AVIopen[i] then avi.CloseAVI(i); // this is making things crash at the moment - don't know why yet
  // CloseDown ...
  CanClose:=true;
end;

procedure TfmMain.FormCreate(Sender: TObject);
begin
  // fill parameter component arrays ...
  Sliders[0]:=tbParam0;
  Sliders[1]:=tbParam1;
  Sliders[2]:=tbParam2;
  Sliders[3]:=tbParam3;
  Sliders[4]:=tbParam4;
  Sliders[5]:=tbParam5;
  Sliders[6]:=tbParam6;
  Sliders[7]:=tbParam7;
  ParamNames[0]:=lParam0Name;
  ParamNames[1]:=lParam1Name;
  ParamNames[2]:=lParam2Name;
  ParamNames[3]:=lParam3Name;
  ParamNames[4]:=lParam4Name;
  ParamNames[5]:=lParam5Name;
  ParamNames[6]:=lParam6Name;
  ParamNames[7]:=lParam7Name;
  ParamValues[0]:=lParam0value;
  ParamValues[1]:=lParam1value;
  ParamValues[2]:=lParam2value;
  ParamValues[3]:=lParam3value;
  ParamValues[4]:=lParam4value;
  ParamValues[5]:=lParam5value;
  ParamValues[6]:=lParam6value;
  ParamValues[7]:=lParam7value;
  ParamDwords[0]:=lParam0Dword;
  ParamDwords[1]:=lParam1Dword;
  ParamDwords[2]:=lParam2Dword;
  ParamDwords[3]:=lParam3Dword;
  ParamDwords[4]:=lParam4Dword;
  ParamDwords[5]:=lParam5Dword;
  ParamDwords[6]:=lParam6Dword;
  ParamDwords[7]:=lParam7Dword;
  StartingApp:=true;
  PluginLoaded:=false;
end;

procedure TfmMain.bSIbrowseClick(Sender: TObject);
begin
  if odAVI.Execute then ebSIfilename.text:=odAVI.FileName;
end;

procedure TfmMain.bSIrunUpAVIClick(Sender: TObject);
var
  tempFilename: string;
begin
  tempFilename:=ebSIfilename.Text;
  if not fileExists(tempFilename) then exit;

    // InitAVI should already have been done by the primary instance controls ...
    CurrentFrame[1]:=0;

    // OpenAVI ....
    PluginHost.VideoInfoStruct[1]:=AVI.OpenAVI(ebSIfilename.text,1);

end;

procedure TfmMain.bSIshutDownAVIClick(Sender: TObject);
begin

  instanceReady[1]:=false;

  // close AVI
  if tPlay.Enabled then tPlay.Enabled:=false;
  AVI.CloseAVI(1);

end;

procedure TfmMain.bRunUpSIClick(Sender: TObject);
var
  tempSingle: single;
begin
  PluginInstance[1]:=InstantiatePlugin(VideoInfoStruct[1]);

  // Setup Second Instance Single Parameter control ...

  lSIparam0Name.Caption:='';
  if NumParams>0 then begin
    tbSIslider.SliderVisible:=true;
    // Get Param Name ...
    lSIparam0Name.Caption:=GetParameterName(0);

    // Get Parameter Default Value ...
    tbSIslider.Position:=round(PluginHost.GetParameterDefault(0)*100);

    // Get Parameter Display Value ...
    lSIParam0value.Caption:=GetParameterDisplay(0,PluginInstance[1]);

    // Get Param actual value ...
    tempSingle:=pluginHost.GetParameter(0,PluginInstance[1]);
    lSIparam0dword.caption:=floattostr(tempSingle);
  end;

  InstanceReady[1]:=true;
end;

procedure TfmMain.bShutDownSIClick(Sender: TObject);
begin
  instanceReady[1]:=false;
  DeInstantiatePlugin(PluginInstance[1]);
end;

procedure TfmMain.tbSISliderChange(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
  tempTrackbar: Ttrackbar;
begin

  tempInt:=tbSIslider.position;
  tempSingle:=tempInt/100;

  PluginHost.SetParameter(0,tempSingle,PluginInstance[1]);
  lSIparam0value.caption:=GetParameterDisplay(0,PluginInstance[1]);
  tempSingle:=pluginHost.GetParameter(0,PluginInstance[1]);
  lSIparam0dword.caption:=floattostr(tempSingle);
end;

end.
