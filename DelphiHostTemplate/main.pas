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
  StdCtrls, ExtCtrls, ComCtrls, inifiles, FreeFrame;

type
  TfmMain = class(TForm)
    bInit: TButton;
    bOpenAVI: TButton;
    ebAVIFilename: TEdit;
    PaintBox1: TPaintBox;
    GroupBox1: TGroupBox;
    lPluginMajorVersion: TLabel;
    lPluginMinorVersion: TLabel;
    lPluginName: TLabel;
    lPluginType: TLabel;
    lPluginUniqueID: TLabel;
    GroupBox2: TGroupBox;
    lVideoWidth: TLabel;
    lVideoHeight: TLabel;
    tbParam0: TTrackBar;
    tbParam1: TTrackBar;
    tbParam2: TTrackBar;
    tbParam3: TTrackBar;
    lParam0Name: TLabel;
    lParam1Name: TLabel;
    lParam2Name: TLabel;
    lParam3Name: TLabel;
    lParam0Value: TLabel;
    lParam1Value: TLabel;
    lParam2Value: TLabel;
    lParam3Value: TLabel;
    lParam0dword: TLabel;
    lParam1dword: TLabel;
    lParam2dword: TLabel;
    lParam3dword: TLabel;
    bBrowse: TButton;
    odAVI: TOpenDialog;
    lBitDepth: TLabel;
    gbPluginCaps: TGroupBox;
    l16bit: TLabel;
    l24bit: TLabel;
    l32bit: TLabel;
    bPlayAndProcess: TButton;
    bStop: TButton;
    tPlay: TTimer;
    FreeFrame1: TFreeFrame;
    FreeFrame2: TFreeFrame;
    cbEffect1: TComboBox;
    cbEffect2: TComboBox;
    cbEffect1Active: TCheckBox;
    cbEffect2Active: TCheckBox;
    procedure bInitClick(Sender: TObject);
    procedure bOpenAVIClick(Sender: TObject);
    procedure ebAVIFilenameChange(Sender: TObject);
    procedure bCloseAVIClick(Sender: TObject);
    procedure tbParam0Change(Sender: TObject);
    procedure bBrowseClick(Sender: TObject);
    procedure tbParam1Change(Sender: TObject);
    procedure tbParam2Change(Sender: TObject);
    procedure tbParam3Change(Sender: TObject);
    procedure bPlayAndProcessClick(Sender: TObject);
    procedure tPlayTimer(Sender: TObject);
    procedure bStopClick(Sender: TObject);
    procedure SetEffect(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    { Private declarations }
    lpBitmapInfoHeader: pBitmapInfoHeader;
    StartingApp: boolean;
    procedure DisplayFrame(lpbitmapinfoheader: pbitmapinfoheader);
  public
    { Public declarations }
    AHDC: HDC;                   // Handle
  end;

const
  AppVersion : string='0.1';
  APIversion : string='0.1050';

var
  fmMain: TfmMain;
  CurrentFrame: integer;
  NumFrames: integer;
  bits: pointer;
  lpBitmapInfoHeader: pBitmapInfoHeader;
  NumParams: integer;
  Currenteffect : integer = 1;

implementation

uses avi, utils;

var
  p32bitFrame: pointer;

{$R *.DFM}

procedure TfmMain.bInitClick(Sender: TObject);
begin
  try AVI.DeInit except end;
  AVI.Init;
  CurrentFrame:=0;
end;

procedure TfmMain.bOpenAVIClick(Sender: TObject);
begin
  FreeFrame1.VideoInfoStruct := FreeFrame.TVideoInfoStruct(AVI.OpenAVI(ebAVIfilename.text,FreeFrame1.VideoInfoStruct));
  FreeFrame2.VideoInfoStruct := FreeFrame.TVideoInfoStruct(AVI.OpenAVI(ebAVIfilename.text,FreeFrame1.VideoInfoStruct));
  lVideoWidth.caption        := inttostr(FreeFrame1.VideoInfoStruct.FrameWidth);
  lVideoHeight.caption       := inttostr(FreeFrame1.VideoInfoStruct.FrameHeight);
  case FreeFrame1.VideoInfoStruct.BitDepth of
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



procedure TfmMain.tbParam0Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
  TempFreeFrame : TFreeFrame;
begin
  if Currenteffect =1 then TempFreeFrame := FreeFrame1
  else  TempFreeFrame := FreeFrame2;
  tempInt:=tbParam0.position;
  tempSingle:=tempInt/100;
  TempFreeFrame.SetParameter(0,tempSingle);
  lParam0Value.caption:=TempFreeFrame.GetParameterDisplay(0);
  tempSingle:=TempFreeFrame.GetParameter(0);
  lParam0dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam1Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
  TempFreeFrame : TFreeFrame;
begin
  if Currenteffect =1 then TempFreeFrame := FreeFrame1
  else  TempFreeFrame := FreeFrame2;

  tempInt:=tbParam1.position;
  tempSingle:=tempInt/100;
  TempFreeFrame.SetParameter(1,tempSingle);
  lParam1Value.caption:=TempFreeFrame.GetParameterDisplay(1);
  tempSingle:=TempFreeFrame.GetParameter(1);
  lParam1dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam2Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
  TempFreeFrame : TFreeFrame;
begin
  if Currenteffect =1 then TempFreeFrame := FreeFrame1
  else  TempFreeFrame := FreeFrame2;

  tempInt:=tbParam2.position;
  tempSingle:=tempInt/100;
  TempFreeFrame.SetParameter(2,tempSingle);
  lParam2Value.caption:=TempFreeFrame.GetParameterDisplay(2);
  tempSingle:=TempFreeFrame.GetParameter(2);
  lParam2dword.caption:=floattostr(tempSingle);
end;

procedure TfmMain.tbParam3Change(Sender: TObject);
var
  tempInt: integer;
  tempSingle: single;
  TempFreeFrame : TFreeFrame;
begin
  if Currenteffect = 1 then TempFreeFrame := FreeFrame1
  else TempFreeFrame := FreeFrame2;
  
  tempInt:=tbParam3.position;
  tempSingle:=tempInt/100;
  TempFreeFrame.SetParameter(3,tempSingle);
  lParam3Value.caption:=TempFreeFrame.GetParameterDisplay(3);
  tempSingle:=TempFreeFrame.GetParameter(3);
  lParam3dword.caption:=floattostr(tempSingle);
end;


procedure TfmMain.bBrowseClick(Sender: TObject);
begin
  if odAVI.Execute then begin
    ebAVIfilename.text:=odAVI.FileName;
  end;
end;

procedure TfmMain.bPlayAndProcessClick(Sender: TObject);
begin
  currentFrame:=0;
  tPlay.Enabled:=true;
end;

procedure TfmMain.tPlayTimer(Sender: TObject);
var
  pBits: pointer;
  FrameSize: integer; // in bytes
begin
  // Get frame from AVI if effect - if source just pass on pointer to framebuffer ...
  inc(currentFrame);
  if currentFrame>(numFrames-1) then currentFrame:=1;

  lpbitmapinfoheader:=AVI.GetFrame(currentFrame);
  pBits:=Pointer(Integer(lpBitmapInfoHeader) + sizeof(TBITMAPINFOHEADER));

  // Process frame through plugin
  if ( (cbEffect1Active.Checked) and (FreeFrame1.currentPlug <> 0) ) then FreeFrame1.ProcessFrame(pBits); // lpbitmapinfoheader is the current decompressed frame from the mci in the host app
  if ( (cbEffect2Active.Checked) and (FreeFrame2.currentPlug <> 0) ) then FreeFrame2.ProcessFrame(pBits);
  // Display the frame
  DisplayFrame(lpbitmapinfoheader);
end;

procedure TfmMain.bStopClick(Sender: TObject);
begin
  tPlay.Enabled:=false;
end;


procedure TfmMain.SetEffect(Sender: TObject);
var
 TempFreeFrame : TFreeFrame;
begin
  Currenteffect := (sender as TComboBox).Tag;
  
  if (sender as TComboBox).Tag = 1 then TempFreeFrame := FreeFrame1
  else TempFreeFrame := FreeFrame2;

  TempFreeFrame.PluginIndex :=(sender as TComboBox).ItemIndex;

  lPluginMajorVersion.caption := inttostr(TempFreeFrame.PluginMajorVersion);
  lPluginMinorVersion.caption := inttostr(TempFreeFrame.PluginMinorVersion);
  lPluginUniqueID.caption     := TempFreeFrame.PluginUniqueID;

  lPluginName.caption         := TempFreeFrame.PluginName;
  lPluginType.caption         := TempFreeFrame.PluginType;

  if TempFreeFrame.Support16bit then l16bit.Caption:='16bit: yes' else l16bit.Caption:='16bit: no';
  if TempFreeFrame.Support24bit then l24bit.Caption:='24bit: yes' else l24bit.Caption:='24bit: no';
  if TempFreeFrame.Support32bit then l32bit.Caption:='32bit: yes' else l32bit.Caption:='32bit: no';
  //
  NumParams:=TempFreeFrame.GetNumParameters;
  tbParam0.Visible:=false;
  tbParam1.Visible:=false;
  tbParam2.Visible:=false;
  tbParam3.Visible:=false;
  if NumParams>0 then tbParam0.Visible:=true;
  if NumParams>1 then tbParam1.Visible:=true;
  if NumParams>2 then tbParam2.Visible:=true;
  if NumParams>3 then tbParam3.Visible:=true;
  //
  lParam0Name.caption:='';
  lParam1Name.caption:='';
  lParam2Name.caption:='';
  lParam3Name.caption:='';

  if NumParams>0 then lParam0Name.caption:=TempFreeFrame.GetParameterName(0);
  if NumParams>1 then lParam1Name.caption:=TempFreeFrame.GetParameterName(1);
  if NumParams>2 then lParam2Name.caption:=TempFreeFrame.GetParameterName(2);
  if NumParams>3 then lParam3Name.caption:=TempFreeFrame.GetParameterName(3);
  //
  if NumParams>0 then tbParam0.Position:=round(TempFreeFrame.GetParameterDefault(0)*100);
  if NumParams>1 then tbParam1.Position:=round(TempFreeFrame.GetParameterDefault(1)*100);
  if NumParams>2 then tbParam2.Position:=round(TempFreeFrame.GetParameterDefault(2)*100);
  if NumParams>3 then tbParam3.Position:=round(TempFreeFrame.GetParameterDefault(3)*100);
  //
  lParam0Value.caption:='';
  lParam1Value.caption:='';
  lParam2Value.caption:='';
  lParam3Value.caption:='';

  if NumParams>0 then lParam0Value.caption:=TempFreeFrame.GetParameterDisplay(0);
  if NumParams>1 then lParam1Value.caption:=TempFreeFrame.GetParameterDisplay(1);
  if NumParams>2 then lParam2Value.caption:=TempFreeFrame.GetParameterDisplay(2);
  if NumParams>3 then lParam3Value.caption:=TempFreeFrame.GetParameterDisplay(3);

  lParam0dword.caption:='';
  lParam1dword.caption:='';
  lParam2dword.caption:='';
  lParam3dword.caption:='';
end;

procedure TfmMain.FormShow(Sender: TObject);
var
 i : integer;
begin

 FreeFrame1.Directory := extractfilepath(application.exename) + 'plugins\';
 FreeFrame1.Autoloadplugin := true;
 FreeFrame1.Getplugins;
 if not FreeFrame1.Pluginsfound then showmessage('no plugins found');

 for i := 0 to FreeFrame1.Plugins.Count-1 do begin
  cbEffect1.Items.Add(FreeFrame1.Plugins.Strings[i]);
 end;

 FreeFrame2.Directory := extractfilepath(application.exename) + 'plugins\';
 FreeFrame2.Autoloadplugin := true;
 FreeFrame2.Getplugins;
 if not FreeFrame2.Pluginsfound then showmessage('no plugins found');

 for i := 0 to FreeFrame2.Plugins.Count-1 do begin
  cbEffect2.Items.Add(FreeFrame2.Plugins.Strings[i]);
 end;
 
end;

end.
