# Microsoft Developer Studio Project File - Name="VectorizeAE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VectorizeAE - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VectorizeAE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VectorizeAE.mak" CFG="VectorizeAE - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VectorizeAE - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VectorizeAE - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "VectorizeAE"
# PROP Scc_LocalPath "..\..\.."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VectorizeAE - Win32 Debug"

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
# ADD CPP /nologo /Zp4 /MDd /W3 /WX /Gm /Gi /Zi /Od /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /I "..\..\..\..\core" /I "..\..\..\AfterEffects" /I "..\..\.." /D "_DEBUG" /D "MSWindows" /D "WIN32" /D "_WINDOWS" /D "PETE_NATIVE_AE" /FAs /FR /YX"AE_Effect.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\VectorizeAE.aex"
# SUBTRACT LINK32 /pdb:none /incremental:no /map

!ELSEIF  "$(CFG)" == "VectorizeAE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "VectorizeAE___Win32_Release"
# PROP BASE Intermediate_Dir "VectorizeAE___Win32_Release"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W3 /WX /Gm /Gi /Zi /O2 /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /D "MSWindows" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAs /FR /YX"AE_Effect.h" /FD /c
# ADD CPP /nologo /Zp4 /MDd /W3 /WX /Gi /O2 /Ob2 /I "..\..\..\AfterEffects" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\ADM" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\PICA" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Headers\AS" /I "C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Util" /I "..\..\..\..\core" /I "..\..\.." /D "MSWindows" /D "WIN32" /D "_WINDOWS" /D "PETE_NATIVE_AE" /FAs /YX"AE_Effect.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\VectorizeAE.aex"
# SUBTRACT BASE LINK32 /pdb:none /incremental:no /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msvcrt.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"C:\Program Files\Adobe\After Effects 5.5\Plug-ins\VectorizeAE.aex"
# SUBTRACT LINK32 /pdb:none /incremental:no /map /debug

!ENDIF 

# Begin Target

# Name "VectorizeAE - Win32 Debug"
# Name "VectorizeAE - Win32 Release"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\..\AfterEffects\AEGP_SuiteHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\AfterEffects\AETemplate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\BoxFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\ChannelFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\ColourReduce.h
# End Source File
# Begin Source File

SOURCE=..\..\..\AfterEffects\PF_Suite_Helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\Vectorize.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\VectorizeAEPiPL.r

!IF  "$(CFG)" == "VectorizeAE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling the PiPL
IntDir=.\Debug
ProjDir=.
InputPath=..\VectorizeAEPiPL.r
InputName=VectorizeAEPiPL

"$(ProjDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /D "MSWindows" /EP ..\$(InputName).r > $(IntDir)\$(InputName).rr 
	"C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Resources\PiPLTool.exe" $(IntDir)\$(InputName).rr                          $(IntDir)\$(InputName).rrc 
	cl /D "MSWindows" /EP $(IntDir)\$(InputName).rrc >                          $(ProjDir)\$(InputName).rc 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VectorizeAE - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling the PiPL
IntDir=.\Release
ProjDir=.
InputPath=..\VectorizeAEPiPL.r
InputName=VectorizeAEPiPL

"$(ProjDir)\$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /D "MSWindows" /EP ..\$(InputName).r > $(IntDir)\$(InputName).rr 
	"C:\Program Files\Adobe\After Effects 5.5r2 SDK\Examples\Resources\PiPLTool.exe" $(IntDir)\$(InputName).rr                          $(IntDir)\$(InputName).rrc 
	cl /D "MSWindows" /EP $(IntDir)\$(InputName).rrc >                          $(ProjDir)\$(InputName).rc 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VectorizeAEPiPL.rc
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Core\BoxFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\ChannelFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\ColourReduce.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Core\Vectorize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\VectorizeVFX.cpp
# End Source File
# End Target
# End Project
