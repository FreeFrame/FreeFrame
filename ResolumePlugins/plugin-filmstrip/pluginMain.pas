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
function MakeRects(w,h : integer) : boolean;

procedure InitLib;

const
  NumParameters: dword = 3;

var
  PluginInfoStruct: TPluginInfoStruct;
  pPluginInfoBlock: pdw;
  VideoInfoStruct: TVideoInfoStruct;
  pVideoInfoStruct: pDw;
  ParameterNameStruct: TParameterNameStruct;
  pParameterNameStruct: pointer;
  ParameterArray: array [0..3] of single;
  ParameterDisplayValue: array [0..15] of char; // this is the current transfer value for when a parameter display value is requested
  Bitmap,Bitmap2,TempBitmap : TBitmap;
  //
  rects : array of TRect;
  Widthlookup  : array[0..6] of integer = (16,20,32,40,64,80,160);
  Heightlookup : array[0..8] of integer = (16,20,24,30,40,48,60,80,120);
  StripWidth,StripHeight : integer;
  Direction : integer = 0;
  //

implementation

uses Classes;

procedure InitLib;
begin
  with PluginInfoStruct do begin
    APIMajorVersion:=0;
    APIMinorVersion:=1050;
    PluginUniqueID:='FILM';
    PluginName:='Filmstrip';
    PluginType:=0;
  end;
  ParameterNameStruct.Parameter0Name:='strip width     ';
  ParameterNameStruct.Parameter1Name:='strip height    ';
  ParameterNameStruct.Parameter2Name:='direction       ';
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
  //
  Bitmap := TBitmap.Create;
  Bitmap := TBitmap.Create;
  Bitmap.PixelFormat := pf24Bit;
  Bitmap.Width  := VideoInfoStruct.FrameWidth;
  Bitmap.Height := VideoInfoStruct.FrameHeight;
  Bitmap2 := TBitmap.Create;
  Bitmap2 := TBitmap.Create;
  case VideoInfoStruct.BitDepth of
   0,1 : Bitmap2.PixelFormat := pf24Bit;
   2   : Bitmap2.PixelFormat := pf32Bit;
  end;

  Bitmap2.Width  := VideoInfoStruct.FrameWidth;
  Bitmap2.Height := VideoInfoStruct.FrameHeight;

  //
  TempBitmap := TBitmap.Create;
  TempBitmap := TBitmap.Create;
  case VideoInfoStruct.BitDepth of
   0,1 : TempBitmap.PixelFormat := pf24Bit;
   2   : TempBitmap.PixelFormat := pf32Bit;
  end;

  TempBitmap.Width  := VideoInfoStruct.FrameWidth;
  TempBitmap.Height := VideoInfoStruct.FrameHeight;

  //
  MakeRects(Widthlookup[6],Heightlookup[6]);
end;

function DeInitialise(pParam: pointer): pointer;
begin
  Bitmap.Free;
  Bitmap2.Free;
  TempBitmap.Free;
  result:=pointer(0);
end;

function MakeRects(w,h : integer) : boolean;
var
  rows,cols,i,j : integer;
begin
  cols  := (VideoInfoStruct.FrameWidth div w); //amount of columns
  rows  := (VideoInfoStruct.FrameHeight div h); //amount of rows

  for j:=0 to rows-1 do begin
   for i:=0 to cols-1 do begin
     setlength(rects,length(rects)+1);
     rects[length(rects)-1] := rect(i*w,j*h,(i+1)*w,(j+1)*h);
   end;
  end;

  StripWidth  := w;
  StripHeight := h;
  
end;

function ProcessFrame(pParam: pointer): pointer;
var
 i : integer;
begin
    try

      copyem(TempBitmap,pParam); //copy framebuffer to bitmap
      Bitmap2.Canvas.Draw(0,0,Bitmap);

      if direction = 0 then begin
        for i := 0 to length(rects)-2 do begin
         Bitmap.canvas.CopyRect(rects[i+1],
                                Bitmap2.Canvas,
                                rects[i]
                                );
        end;
        Bitmap.Canvas.StretchDraw(Rect(0,0,StripWidth,StripHeight),TempBitmap);
      end else if direction = 1 then begin
        for i := length(rects)-2 downto 0 do begin
         Bitmap.canvas.CopyRect(rects[i-1],
                                Bitmap2.Canvas,
                                rects[i]
                                );
        end;
        Bitmap.Canvas.StretchDraw(Rect(0,0,StripWidth,StripHeight),TempBitmap);
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
  Ptr := PByteArray(Integer(pParam));
  if VideoInfoStruct.BitDepth = 1 then begin
   Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit
    for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
     PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
     Ptr := PByteArray(Integer(pParam) + (i*Pitch));
     for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
       PtrDest[j*3]   := Ptr^[j*3];
       PtrDest[j*3+1] := Ptr^[j*3+1];
       PtrDest[j*3+2] := Ptr^[j*3+2];
     end;
    end;
  end else if VideoInfoStruct.BitDepth = 2 then begin
   Pitch := VideoInfoStruct.FrameWidth * 4; // because 4 bytes (32 / 8) are reserved for each pixel when bitdepth is 24 bit
    for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
     PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
     Ptr := PByteArray(Integer(pParam) + (i*Pitch));
     for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
       PtrDest[j shl 2]    := Ptr^[j shl 2];
       PtrDest[j shl 2 +1] := Ptr^[j shl 2 +1];
       PtrDest[j shl 2 +2] := Ptr^[j shl 2 +2];
     end;
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
  Ptr := PByteArray(Integer(pParam));
  if VideoInfoStruct.BitDepth = 1 then begin
   Pitch := VideoInfoStruct.FrameWidth * 3; // because 3 bytes (24 / 8) are reserved for each pixel when bitdepth is 24 bit
    for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
     PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
     Ptr := PByteArray(Integer(pParam) + (i*Pitch));
     for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
       Ptr[j*3]   := PtrDest^[j*3];
       Ptr[j*3+1] := PtrDest^[j*3+1];
       Ptr[j*3+2] := PtrDest^[j*3+2];
     end;
    end;
  end else if VideoInfoStruct.BitDepth = 2 then begin
   Pitch := VideoInfoStruct.FrameWidth * 4; // because 4 bytes (32 / 8) are reserved for each pixel when bitdepth is 24 bit
    for i := 0 to VideoInfoStruct.FrameHeight-1 do begin
     PtrDest := Bitmap.ScanLine[(VideoInfoStruct.FrameHeight-1)-i];
     Ptr := PByteArray(Integer(pParam) + (i*Pitch));
     for j:=0 to VideoInfoStruct.FrameWidth-1 do begin
       Ptr[j shl 2]    := PtrDest^[j shl 2];
       Ptr[j shl 2 +1] := PtrDest^[j shl 2 +1];
       Ptr[j shl 2 +2] := PtrDest^[j shl 2 +2];
     end;
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
        result:=pointer(inttostr(StripWidth));
       end;
    1: begin
        result:=pointer(inttostr(StripHeight));
       end;
    2: begin
        result:=pointer(inttostr(Direction));
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
  index : integer;
begin
  tempPDWparam:=pdw(pParam);
  tempPDWvalue:=pdw(pParam);
  inc(tempPDWvalue);
  case integer(tempPDWparam^) of
    0: begin
      copymemory(@ParameterArray[0],tempPDWvalue,4);
      index := round( (length(Widthlookup) / 10) * (ParameterArray[0]*10));
      if index = length(Widthlookup) then index := length(Widthlookup)-1;
      MakeRects(Widthlookup[index],StripHeight);
      result:=pointer(0);
    end;
    1: begin
      copymemory(@ParameterArray[1],tempPDWvalue,4);
      index := round( (length(Heightlookup) / 10) * (ParameterArray[1]*10));
      if index = length(Heightlookup) then index := length(Heightlookup)-1;
      MakeRects(StripWidth,HeightLookUp[index]);
      result:=pointer(0);
    end;
    2: begin
      copymemory(@ParameterArray[2],tempPDWvalue,4);
      if ParameterArray[2] < 0.5 then Direction := 0;
      if ParameterArray[2] >= 0.5 then Direction := 1;
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
