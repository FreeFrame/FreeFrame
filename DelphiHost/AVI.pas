unit AVI;

interface

uses VFW, pluginHost, winprocs; 

var
  AVIfilename: string;

procedure Init;
procedure DeInit;
function OpenAVI(Filename: string): Tvideoinfostruct;
procedure CloseAVI;
function GetFrame(FrameNumber: integer): pointer;

implementation

uses main;

var
  pAnAviFile: PAviFile;          //The Avi File
  pVideoStream: pAviStream;      //Pointer to Video Stream
  AGetframe: pGetFrame;          //Pointer to GetFrame struct
  HDrawDibDC: HDrawDib;          //HDrawdib

procedure Init;
begin
  AVIfileInit;
end;

procedure DeInit;
begin
  AviFileExit;
end;

function OpenAVI(Filename: string): Tvideoinfostruct;
var
  i: integer;
  pFileInfo: TAviFileInfo;
  pAnAviStream: pAviStream;
  StreamInfo: TAviStreamInfo;
  tempVideoinfostruct: Tvideoinfostruct;
begin
  AVIFileOpen(pAnAviFile, pchar(Filename), OF_READ, Nil);
  AVIFileInfo(pAnAviFile, @pFileInfo, sizeof(TAviFileInfo));
  for i := 0 to pFileInfo.dwstreams -1 do begin
    AVIFileGetStream(pAnAviFile, pAnAviStream, 0, i);
    AVIStreamInfo(pAnAviStream, @StreamInfo, sizeof(TAviStreamInfo));
    if (StreamInfo.fccType = streamtypeVIDEO) then begin
      pVideoStream := pAnAviStream;
      //fps := round(StreamInfo.dwRate /StreamInfo.dwScale);
      hDrawDibDC := DrawDibOpen();
      AGetFrame := AVIStreamGetFrameOpen(pVideoStream,nil);
      tempVideoInfoStruct.Framewidth:=streaminfo.rcFrame.Right;
      tempVideoInfoStruct.FrameHeight:=streaminfo.rcFrame.Bottom;
      //tempVideoInfoStruct.BitDepth:=1; // 1=24bit packed MCI standard // video info struct is now what we're gonna run the plugin in  - we know the MCI is 24bit
      if not VideoInfoStruct.BitDepth=2 then tempVideoInfoStruct.BitDepth:=2 else tempVideoInfoStruct.BitDepth:=1;
      result:=tempVideoInfoStruct;
      numframes := AVIStreamEnd(pVideoStream);
      exit;
    end;
  end;
end;

procedure CloseAVI;
begin
  if AGetframe <> nil then AVIStreamGetFrameClose(AGetFrame);
  if HDrawDibDC <> 0 then DrawDibClose(hDrawDibDC);
  if pVideoStream <> nil then AVIStreamRelease(pVideoStream);
  if pAnAviFile <> nil then AviFileRelease(pAnAviFile);
end;

function GetFrame(FrameNumber: integer): pointer;
begin
  result := AVIStreamGetFrame(AGetframe, FrameNumber);
end;

end.
