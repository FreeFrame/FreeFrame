unit utils;

interface

uses windows, pluginHost;

function Make32bitBuffer(VideoInfoStruct: TVideoInfoStruct): pointer;
function Convert24to32(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Convert32to24(pSourceFrame, pDestFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
function Free32bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;

implementation

uses main;

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

function Free32bitBuffer(pFrame: pointer; VideoInfoStruct: TVideoInfoStruct): dword;
begin
  FreeMem(pFrame,VideoInfoStruct.FrameWidth*VideoInfoStruct.FrameHeight*4);
  Result:=0;
end;

end.
