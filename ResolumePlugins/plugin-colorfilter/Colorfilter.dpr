// Open Video Plugin Host Prototype
// Delphi Version

// Russell Blakeborough
// russell@brightonart.org

// This code released under the LGPL

{
  ==========================
  Function Code Table
  ==========================
  Code           Function
  =============  ============
  0              GetInfo
  1              Initialize
  2              DeInitialize
  3              ProcessFrame
  4              GetNumParameters
  5              GetParameterName
  6              GetParameterDefault
  7              GetParameterDisplay
  8              SetParameter
  9              GetParameter // intended only for debugging & checking for now - the plugin is not currently supposed to alter the param values 
  =============  ============
}

library Colorfilter;

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
      result:=GetNumParamaters(pParam);
    end;
    5: begin
      result:=GetParamaterName(pParam);
    end;
    6: begin
      result:=GetParamaterDefault(pParam);
    end;
    7: begin
      result:=GetParamaterDisplay(pParam);
    end;
    8: begin
      result:=SetParamater(pParam);
    end;
    9: begin
      result:=GetParameter(pParam);
    end;
    else result:=pointer(8765);
  end;
end;

exports
  plugMain;
begin
  InitLibrary;
end.
