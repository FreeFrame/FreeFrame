// FreeFrame Open Video Plugin Prototype
// Delphi/Kylix Version
// For win32 and linux

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

library PascalSamplePlugin;

{ Important note about DLL memory management: ShareMem must be the
  first unit in your library's USES clause AND your project's (select
  Project-View Source) USES clause if your DLL exports any procedures or
  functions that pass strings as parameters or function results. This
  applies to all strings passed to and from your DLL--even those that
  are nested in records and classes. ShareMem is the interface unit to
  the BORLNDMM.DLL shared memory manager, which must be deployed along
  with your DLL. To avoid using BORLNDMM.DLL, pass string information
  using PChar or ShortString parameters. }

uses
  SysUtils,
  Classes,
{$IFDEF LINUX}
  Types,
{$ENDIF}
{$IFDEF WIN32}
  windows,
{$ENDIF}
  plugin in 'plugin.pas';

{$R *.res}

var
  PluginInfoStruct: TPluginInfoStruct;
  PluginExtendedInfoStruct: TPluginExtendedInfoStruct;
  pPluginInfoStruct: pointer;
  pPluginExtendedInfoStruct: pointer;
  FreeFramePlugin: TFreeFramePlugin;

procedure InitLibrary;
begin
  // DLL / SO open call ...
  // Do nothing - create object on freeframe initialise plugin
end;

//  todo: do all the casting here so these funcions take and return the right types
//  todo: get paramater display value processing sorted in this sample plugin

{$IFDEF WIN32}
  function plugMain(functionCode: dword; pParam: pointer; instanceID: dword): Pointer; stdcall
{$ENDIF}

{$IFDEF LINUX}
  procedure plugMain(var Result: Pointer; functionCode: dword; pParam: pointer; instanceID: dword); cdecl;
{$ENDIF}

var
  PluginInstance, NewPluginInstance: TFreeFramePlugin;
  i: integer;

begin
  PluginInstance:= pointer(instanceID);
  case functionCode of
    0: begin
      with PluginInfoStruct do begin
        APIMajorVersion:=1;
        APIMinorVersion:=000;
        PluginUniqueID:='PTST';
        PluginName:='PascalTestPlugin';
        PluginType:=0;   // effect plugin
      end;
      pPluginInfoStruct:=@pluginInfoStruct;
      result:=pointer(pPluginInfoStruct);
    end;
    1: begin    // Plugin Initislise
      result:=nil;
    end;
    2: begin    // Plugin DeInitislise
      result:=nil;
    end;
    3: begin
      if assigned(PluginInstance) then result:=PluginInstance.ProcessFrame(pParam)
       else result:=pointer($FFFFFFFF);
    end;
    4: begin
      result:=GetNumParameters(pParam);
    end;
    5: begin
      result:=GetParameterName(pParam);
    end;
    6: begin
      result:=GetParameterDefault(pParam);
    end;
    7: begin
      if assigned(PluginInstance) then result:=PluginInstance.GetParameterDisplay(pParam)
       else result:=pointer($FFFFFFFF);
    end;
    8: begin
      if assigned(PluginInstance) then result:=PluginInstance.SetParameter(pParam)
       else result:=pointer($FFFFFFFF);
    end;
    9: begin
      if assigned(PluginInstance) then result:=PluginInstance.GetParameter(pParam)
       else result:=pointer($FFFFFFFF);
    end;
    10: begin
      result:=GetPluginCaps(pParam);
    end;
    11: begin    // Instantiate Plugin
      result:=pointer($FFFFFFFF);
      try
        NewPluginInstance:=TFreeFramePlugin.create;
        NewPluginInstance.InitialiseInstance(pParam);
        result:=pointer(NewPluginInstance);
      except
        result:=pointer($FFFFFFFF);
      end;
    end;
    12: begin     // Destroy Plugin Instance
      result:=pointer($FFFFFFFF);
      try
        if assigned(PluginInstance) then PluginInstance.destroy;
        result:=pointer(0);
      except
        result:=pointer($FFFFFFFF);
      end;
    end;
    13: begin // GetExtendedInfo
         // IN:  Nothing
         // OUT: Pointer to PluginExtendedInfoStruct
      with PluginExtendedInfoStruct do begin
        PluginMajorVersion:=1;
        PluginMinorVersion:=001;
        pDescription:= nil;
        pAbout:= nil;
        FreeFrameExtendedDataSize:= 0;
        FreeFrameExtendedDataBlock:= nil;
      end;
      pPluginExtendedInfoStruct:=@pluginExtendedInfoStruct;
      result:=pointer(pPluginExtendedInfoStruct);
    end;
    14: begin // ProcessFrameCopy
         // IN: Pointer to ProcessFrameCopyStruct
         // OUT: Success/Error Code
         // not yet supported in PTST
      result:=pointer($FFFFFFFF);
    end;
    15: begin // GetParamaterType
         // IN: ParameterNumber
         // OUT: Parameter Type
      result:=pointer(10); // all parameters on this plugin will be 'standard' for the moment
    end;
  end;
end;

exports
  plugMain;

begin
  InitLibrary;
end.
