unit utils;

interface

uses windows, pluginmain;

function Fill24bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
function Fill32bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
function Make24bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Make32bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Convert24to32(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Convert32to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free24bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free32bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;

implementation

function Fill24bitBuffer(pFrame: pointer; Color : dword; VideoInfoStruct: TVideoInfoStruct): dword;
var
  temps: pw;
  tempd: pw;
  x,size: integer;
begin
  size := VideoInfoStruct.FrameWidth * VideoInfoStruct.FrameHeight;
  temps:=@color;
  tempd:=pw(pFrame);
  for x:=0 to (size-1) do begin
    tempd^:= temps^;
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
