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
  Types,qgraphics;
{$ENDIF}

{$IFDEF WIN32}
uses
  sysutils,
  windows,graphics;
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
    Parameter3Name: array [0..15] of char;
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
function copyem(bitmap : tbitmap; pParam : pointer) : boolean;
function copyemback(bitmap : tbitmap; pParam : pointer) : boolean;
//
function makegrid(size : integer) : boolean;

procedure InitLib;

const
  NumParameters: dword = 4;
  EllipseLookUp : array[0..5] of integer = (8,10,16,20,40,80);

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParameterNameStruct: TParameterNameStruct;
  pParameterNameStruct: pointer;
  ParameterArray: array [0..3] of single;
  ParameterDisplayValue: array [0..15] of char; // this is the current transfer value for when a parameter display value is requested
  Bitmap : TBitmap;
  ellipsesize : integer = 16;
  
implementation

uses Classes;

procedure InitLib;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1050;
    PluginUniqueID:='BLOG';
    PluginName:='block gen.';
    PluginType:=1; // 0 = effect 1 = source
  end;
  ParameterNameStruct.Parameter0Name:='R       ';
  ParameterNameStruct.Parameter1Name:='G       ';
  ParameterNameStruct.Parameter2Name:='B       ';
  ParameterNameStruct.Parameter3Name:='blocks  ';
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
  //
  makegrid(16);
  //
end;
function makegrid(size : integer) : boolean;
var
  i,j,cols,rows : integer;
  color : dword;
  DestRect : TRect;
begin
  if Bitmap = nil then begin
   Bitmap := TBitmap.Create;
   Bitmap := TBitmap.Create;
   Bitmap.PixelFormat := pf24Bit;
   Bitmap.Width  := VideoInfoStruct.FrameWidth;
   Bitmap.Height := VideoInfoStruct.FrameHeight;
  end;
  //

  Bitmap.canvas.Brush.Style := bsSolid;
  Bitmap.canvas.pen.Style := psClear;
  cols  := (VideoInfoStruct.FrameWidth div size); //amount of columns
  rows  := (VideoInfoStruct.FrameHeight div size); //amount of rows

  color := clGreen;
  for i:=0 to cols do begin
   for j:=0 to rows do begin
    DestRect := Rect(i*size,j*size,(i+1)*size,(j+1)*size);
    Bitmap.canvas.Brush.Color := clBlack;
    Bitmap.canvas.FillRect(DestRect);

    Bitmap.canvas.Brush.Color := color;
    InflateRect(DestRect,-2,-2);
    Bitmap.canvas.FillRect(DestRect);
   end;
  end;
  ellipsesize := size;

end;

function DeInitialise(pParam: pointer): pointer;
begin
  result:=pointer(0);
end;

function ProcessFrame(pParam: pointer): pointer;
var
  DestRect : TRect;
  i,cols,rows,colsr,rowsr : integer;
  mul : integer;
  color : dword;
  r,g,b : byte;
begin
    try
      //copyem(Bitmap,pParam); //copy framebuffer to bitmap

      Bitmap.canvas.Brush.Style := bsSolid;
      Bitmap.canvas.pen.Style := psClear;
      cols  := (VideoInfoStruct.FrameWidth div ellipsesize); //amount of columns
      rows  := (VideoInfoStruct.FrameHeight div ellipsesize); //amount of rows
                      //random(cols*rows)
      r := round((255/100)*(ParameterArray[0]*100));
      g := round((255/100)*(ParameterArray[1]*100));
      b := round((255/100)*(ParameterArray[2]*100));

      for i := 0 to round(ParameterArray[3]*100) do begin
       mul := random(50);
       color := rgb(r,g,b) - rgb(round(r*(mul/100)),round(g*(mul/100)),round(b*(mul/100)));
       colsr := random(cols);
       rowsr := random(rows);
       DestRect := Rect(colsr*ellipsesize,rowsr*ellipsesize,(colsr+1)*ellipsesize,(rowsr+1)*ellipsesize);
       InflateRect(DestRect,-2,-2);
       Bitmap.canvas.Brush.Color := color;
       Bitmap.canvas.FillRect(DestRect);

       //Bitmap.canvas.Ellipse(DestRect);
      end;

      copyemback(Bitmap,pParam); //copy bitmap to framebuffer
    finally
      result:=pointer(VideoInfoStruct.FrameWidth);
    end;
end;

function copyem(bitmap : tbitmap; pParam : pointer) : boolean;
var
  tempPdword : pdw;
  i,j : integer;
  Ptr,PtrDest : PByteArray;
  Pitch : integer;
  c: dword;
begin
  //pitch depends on the bitdepth
  Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit

  Ptr := PByteArray(Integer(pParam));
  for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
   PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
   Ptr := PByteArray(Integer(pParam) + (i*Pitch));
   for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
     //bitmap.canvas.Pixels[j,VideoInfoStruct.FrameHeight-i] := rgb(Ptr^[j*3+2],Ptr^[j*3+1],Ptr^[j*3]);
     PtrDest[j*3]   := Ptr^[j*3];
     PtrDest[j*3+1] := Ptr^[j*3+1];
     PtrDest[j*3+2] := Ptr^[j*3+2];
   end;
  end;

  result:= true;
end;

function copyemback(bitmap : tbitmap; pParam : pointer) : boolean;
var
  tempPdword : pdw;
  i,j : integer;
  Ptr,PtrDest : PByteArray;
  Pitch : integer;
  c: dword;
begin
  //pitch depends on the bitdepth
  Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit

  Ptr := PByteArray(Integer(pParam));
  for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
   PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
   Ptr := PByteArray(Integer(pParam) + (i*Pitch));
   for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
     //bitmap.canvas.Pixels[j,VideoInfoStruct.FrameHeight-i] := rgb(Ptr^[j*3+2],Ptr^[j*3+1],Ptr^[j*3]);
     Ptr[j*3]   := PtrDest^[j*3];
     Ptr[j*3+1] := PtrDest^[j*3+1];
     Ptr[j*3+2] := PtrDest^[j*3+2];
   end;
  end;

  result:= true;
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
      tempSingle:=0.0;
      result:=pointer(tempSingle);
    end;
    1: begin
      tempSingle:=1.0;
      result:=pointer(tempSingle);
    end;
    2: begin
      tempSingle:=0.0;
      result:=pointer(tempSingle);
    end;
    3: begin
      tempSingle:=0.20;
      result:=pointer(tempSingle);
    end;
    else result:=pointer($FFFFFFFF);
  end;
end;

function GetParameterDisplay(pParam: pointer): pointer;
begin
  case integer(pParam) of
    0: begin
        result:=pointer(inttostr(round((255/100)*(ParameterArray[0]*100))));
    end;
    1: begin
        result:=pointer(inttostr(round((255/100)*(ParameterArray[1]*100))));
    end;
    2: begin
       result:=pointer(inttostr(round((255/100)*(ParameterArray[2]*100))));
    end;
    3: begin
      result:=pointer(inttostr(round(ParameterArray[3]*100)));
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
      copymemory(@ParameterArray[3],tempPDWvalue,4);
      //makegrid( round( (ParameterArray[3] * 20 )+1) );
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
