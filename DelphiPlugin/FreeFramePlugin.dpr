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

  =============================================================================================
  Function Code Table
  =============================================================================================
  Code   Function		Input Value			Output Value
  =====  =====================  ==============================  ===============================
  0      GetInfo		Unused				Pointer to PluginInfoStruct
  1      Initialize		Pointer to VideoInfoStruct	Success/Fail
  2      DeInitialize		Unused				Success/Fail
  3      ProcessFrame		Pointer to a frame of video	Success/Fail
  4      GetNumParameters	Unused				NumParameters
  5      GetParameterName	ParameterNumber			Pointer to ParameterName
  6      GetParameterDefault	ParameterNumber			ParameterDefaultValue
  7      GetParameterDisplay	ParameterNumber			Pointer to ParameterDisplayValue
  8      SetParameter		Pointer to SetParameterStruct	Success/Fail
  9      GetParameter		Parameter number		ParameterValue	
  10     GetPluginCaps		PluginCapsIndex			Supported/Unsupported
  =====  =====================  ==============================  ================================       
}


library FreeFramePlugin;

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
  windows,
  pluginMain in 'pluginMain.pas';

{$R *.RES}

procedure InitLibrary;
begin
  InitLib;
end;

//  todo: do all the casting here so these funcions take and return the right types

function plugMain(functionCode: dword; pParam: pointer): Pointer; stdcall;
begin
  case functionCode of
    0: begin
      result:=GetInfo(pParam);
    end;
    1: begin
      result:=InitPlugin(pParam);
    end;
    2: begin
      result:=DeInitPlugin(pParam);
    end;
    3: begin
      result:=ProcessFrame(pParam);
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
      result:=GetParameterDisplay(pParam);
    end;
    8: begin
      result:=SetParameter(pParam);
    end;
    9: begin
      result:=GetParameter(pParam);
    end;
    10: begin
      result:=GetPluginCaps(pParam);
    end;
    else result:=pointer(8765);
  end;
end;

exports
  plugMain;
begin
  InitLibrary;
end.
