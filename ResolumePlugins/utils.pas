unit utils;

interface

uses windows, pluginmain;

function Fill16bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
function Fill24bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
function Fill32bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;


function Make16bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Make24bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Make32bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Convert16to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Convert24to16(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Convert24to32(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Convert32to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free16bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free24bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free32bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;

implementation

function Fill16bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
var
  r,g,b: byte;
  temps: word;
  tempd: pw;
  x,size: integer;
begin
  size := VideoInfoStruct.FrameWidth * VideoInfoStruct.FrameHeight;

  r := Byte(color);
  g := Byte(color shr 8);
  b := Byte(color shr 16);
  temps := ((r shl 8) and 63488) or ((g shl 3) and 2016) or ((b shr 3) and 31);

  tempd:=pw(pFrame);
  for x:=0 to (size-1) do begin
    tempd^:= temps;
    inc(tempd);
  end;
  result:=0;
end;

function Fill24bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
var
  r,g,b: byte;
  tempd: pbyte;
  x,size: integer;
begin
  size := VideoInfoStruct.FrameWidth * VideoInfoStruct.FrameHeight;

  r := Byte(color);
  g := Byte(color shr 8);
  b := Byte(color shr 16);

  tempd:=pbyte(pFrame);
  for x:=0 to (size-1) do begin
    tempd^:= b;
    inc(tempd);
    tempd^:= g;
    inc(tempd);
    tempd^:= r;

    inc(tempd);
  end;
  result:=0;
end;

function Fill32bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
var
  temps: pdw;
  tempd: pdw;
  x,size: integer;
begin
  size := VideoInfoStruct.FrameWidth * VideoInfoStruct.FrameHeight;
  temps:=@color;
  tempd:=pdw(pFrame);
  for x:=0 to (size-1) do begin
    tempd^:= temps^;
    inc(tempd);
  end;
  result:=0;
end;

function Make16bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
var
  tempPointer: pointer;
  FrameSize: integer;
begin
  FrameSize:=VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameWidth;
  GetMem(tempPointer,FrameSize*2);
  Result:=tempPointer;
end;

function Make24bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
var
  tempPointer: pointer;
  FrameSize: integer;
begin
  FrameSize:=VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameWidth;
  GetMem(tempPointer,FrameSize*3);
  Result:=tempPointer;
end;

function Make32bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
var
  tempPointer: pointer;
  FrameSize: integer;
begin
  FrameSize:=VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameWidth;
  GetMem(tempPointer,FrameSize*4);
  Result:=tempPointer;
end;

function Convert16to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
var
  i: integer;
  pSource: pWord;
  pDest: pByte;
  FrameSize: integer;  // in pixels
  R,G,B : byte;
begin
  FrameSize := VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight;
  pSource := pWord(pSourceFrame);
  pDest := pByte(pDestFrame);
  for i:=0 to FrameSize-2 do begin
    r := (pSource^ and 63488) shr 8;
    r := r or (r shr 5);

    g := (pSource^ and 2016) shr 3;
    g := g or (g shr 6);
    
    b := (pSource^ and 31) shr 3;
    b := b or (b shr 5);

    pDest^ := b;
    inc(pDest);
    pDest^ := g;
    inc(pDest);
    pDest^ := r;

    inc(pSource);
    inc(pDest);
  end;
  Result:=0;
end;

function Convert24to16(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
var
  i: integer;
  pSource: pByte;
  pDest: pWord;
  FrameSize: integer;  // in pixels
  R,G,B : byte;
begin
  FrameSize := VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight;
  pSource := pByte(pSourceFrame);
  pDest := pWord(pDestFrame);
  for i:=0 to FrameSize-2 do begin
    b := pSource^;
    inc(pSource);
    g := pSource^;
    inc(pSource);
    r := pSource^;

    pDest^ := ((r shl 8) and 63488) or ((g shl 3) and 2016) or ((b shr 3) and 31);

    inc(pSource);
    inc(pDest);
  end;
  Result:=0;

end;


function Convert24to32(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
var
  i: integer;
  pSource, pDest: pByte;
  FrameSize: integer;  // in pixels
begin
  FrameSize:=VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight;
  pSource:=pByte(pSourceFrame);
  pDest:=pByte(pDestFrame);
  for i:=0 to FrameSize-2 do begin
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    inc(pDest);
  end;
  Result:=0;
end;

function Convert32to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
var
  i: integer;
  pSource, pDest: pByte;
  FrameSize: integer;  // in pixels
begin
  FrameSize:=VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight;
  pSource:=pByte(pSourceFrame);
  pDest:=pByte(pDestFrame);
  for i:=0 to FrameSize-2 do begin
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    pDest^:=pSource^;
    inc(pSource);
    inc(pDest);
    inc(pSource);
  end;
  Result:=0;
end;

function Free16bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
begin
  FreeMem(pFrame,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*2);
  Result:=0;
end;

function Free24bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
begin
  FreeMem(pFrame,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*3);
  Result:=0;
end;

function Free32bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
begin
  FreeMem(pFrame,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*4);
  Result:=0;
end;



end.
