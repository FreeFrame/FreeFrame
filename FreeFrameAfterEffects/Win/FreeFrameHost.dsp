# Microsoft Developer Studio Project File - Name="FreeFrameHost" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FreeFrameHost - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FreeFrameHost.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FreeFrameHost.mak" CFG="FreeFrameHost - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FreeFrameHost - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FreeFrameHost - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FreeFrameHost"
# PROP Scc_LocalPath "..\..\.."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeFrameHost - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp4 /MDd /W3 /WX /Gm /Gi /Zi /O2 /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /I "..\..\MSVCPlugin\\" /D "MSWindows" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAs /FR /YX"AE_Effect.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\FreeFrameHost.aex"
# SUBTRACT LINK32 /pdb:none /incremental:no /map

!ELSEIF  "$(CFG)" == "FreeFrameHost - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FreeFrameHost___Win32_Release"
# PROP BASE Intermediate_Dir "FreeFrameHost___Win32_Release"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W3 /WX /Gm /Gi /Zi /O2 /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /D "MSWindows" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAs /FR /YX"AE_Effect.h" /FD /c
# ADD CPP /nologo /Zp4 /MDd /W3 /WX /Gi /O2 /Ob2 /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /I "..\..\MSVCPlugin\\" /D "MSWindows" /D "WIN32" /D "_WINDOWS" /FAs /YX"AE_Effect.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\FreeFrameHost.aex"
# SUBTRACT BASE LINK32 /pdb:none /incremental:no /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msvcrt.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\FreeFrameHost.aex"
# SUBTRACT LINK32 /pdb:none /incremental:no /map /debug

!ENDIF 

# Begin Target

# Name "FreeFrameHost - Win32 Debug"
# Name "FreeFrameHost - Win32 Release"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "PICA"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPAccess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPAccess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPAdapts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPAdapts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPBasic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPBasic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPCaches.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPCaches.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPFiles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPFiles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPMData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPMData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPPiPL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPPiPL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPPlugs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPPlugs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPProps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPProps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPStrngs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPStrngs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPSuites.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPSuites.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\PICA\SPTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\SPTypes.h
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\..\..\..\devstudio\Include\BASETSD.H
# End Source File
# End Group
# Begin Group "AE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Headers\AE_Effect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\AE_EffectCB.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\AE_EffectCBSuites.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\AE_EffectSuites.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\AE_EffectUI.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\AE_Macros.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Util\entry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Util\Param_Utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Util\PF_Suite_Helper.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\FreeFrameHost.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\projects\PetesPlugins\Core\Pete2dMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Util\String_Utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\FreeFrameHostPiPL.r

!IF  "$(CFG)" == "FreeFrameHost - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling the PiPL
IntDir=.\Debug
ProjDir=.
InputPath=..\FreeFrameHostPiPL.r
InputName=FreeFrameHostPiPL

"$(ProjDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /D "MSWindows" /EP ..\$(InputName).r > $(IntDir)\$(InputName).rr 
	"C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Resources\PiPLTool.exe" $(IntDir)\$(InputName).rr                          $(IntDir)\$(InputName).rrc 
	cl /D "MSWindows" /EP $(IntDir)\$(InputName).rrc >                          $(ProjDir)\$(InputName).rc 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeFrameHost - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling the PiPL
IntDir=.\Release
ProjDir=.
InputPath=..\FreeFrameHostPiPL.r
InputName=FreeFrameHostPiPL

"$(ProjDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /D "MSWindows" /EP ..\$(InputName).r > $(IntDir)\$(InputName).rr 
	"C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Resources\PiPLTool.exe" $(IntDir)\$(InputName).rr                          $(IntDir)\$(InputName).rrc 
	cl /D "MSWindows" /EP $(IntDir)\$(InputName).rrc >                          $(ProjDir)\$(InputName).rc 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeFrameHostPiPL.rc
# End Source File
# End Group
# Begin Group "Supporting code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\PF_Suite_Helper.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\AEGP_SuiteHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\FreeFrameHost.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\..\projects\PetesPlugins\Core\Pete2dMatrix.cpp
# End Source File
# End Target
# End Project
