# Microsoft Developer Studio Project File - Name="pkgSamples" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pkgSamples - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pkgSamples.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pkgSamples.mak" CFG="pkgSamples - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pkgSamples - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pkgSamples - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Visualight/vJo32/Effects/pkgSamples", CWGAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pkgSamples - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "%VJO_INCLUDE%" /I "..\MSVCPlugin" /I "C:\Program Files\visualJockey R3\vJo Plugin SDK v3.2\vJo32\Include" /I "..\Core" /I "C:\Program Files\visualJockey R3\vJo Plugin SDK v3.2\vJo32\Effects\common" /I "..\..\MSVCPlugin" /I "C:\Program Files\Microsoft SDK\Samples\Multimedia\DirectShow\BaseClasses" /I "C:\Program Files\Microsoft SDK\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /FD /QIfist /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib "c:\program files\microsoft sdk\samples\multimedia\directshow\baseclasses\debug\strmbasd.lib" /nologo /subsystem:windows /dll /debug /machine:I386 /out:"c:\Program Files\visualJockey\plugins\pkgPetes.fxp"

!ELSEIF  "$(CFG)" == "pkgSamples - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..MSVCPlugin" /I "..\..\VJoSDK\vJo Plugin SDK v3.2\vJo32\Include" /I "..\Core" /I "..\..\VJoSDK\vJo32\Effects\common" /I "..\..\MSVCPlugin" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib "c:\program files\microsoft sdk\samples\multimedia\directshow\baseclasses\debug\strmbasd.lib" /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"c:/Program Files/visualJockey R3/plugins/pkgPetes.fxp" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "pkgSamples - Win32 Release"
# Name "pkgSamples - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "VJoDfx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackDropKeyDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\BackLightDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\BloomDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourReduceDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourWarpDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourWarpDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeInterlaceDfx.cpp
# End Source File
# Begin Source File

SOURCE="C:\Program Files\visualJockey R3\vJo Plugin SDK v3.2\vJo32\Effects\common\DfxDlg.cpp"
# End Source File
# Begin Source File

SOURCE=.\DilateDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\DilateDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectionalBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\FishEyeDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\GaussianBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\GlowDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\HalfToneDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwiredMixer4Dfx.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwiredMixerDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\KaleidascopeDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelsDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelsDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LionelBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveFeedDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LumaCycleDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LumaKeyDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LumaMaskDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\LumaOffsetDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MetaImageDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MetaImageDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoireStewartDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiMirrorDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\PanSpinZoomDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\pkgSamples.cpp
# End Source File
# Begin Source File

SOURCE=.\pkgSamples.rc
# End Source File
# Begin Source File

SOURCE=.\PShopFiltersDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\PShopFiltersDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RadialBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\RectDistortDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\RefractionDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleMirrorDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SlideDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SmearDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SpiralBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SquashDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StretchDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TextOverlayDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TextOverlayDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TileDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeBlurDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSliceDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSliceDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeWarpDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeWarpDfxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TVPixelDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorizeDfx.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveDfx.cpp
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Core\BackDropKey.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\BackLight.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Bloom.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\BoxFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Burn.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ChannelFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Chromium.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ColourConversion.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ColourReduce.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ColourWarp.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\DeInterlace.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Dilate.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\DirectionalBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Dodge.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Exclusion.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\FishEye.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\GaussianBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Glow.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\HalfTone.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ImageMath.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ImageScale.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Kaleidascope.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Levels.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\LionelBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\LumaCycle.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\LumaKey.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\LumaMask.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\LumaOffset.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\MetaImage.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\MoireStewart.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\PanSpinZoom.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Pete2dMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Posterize.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\RadialBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\RectDistort.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Refraction.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\SimpleMirror.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Slide.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Smear.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Solarize.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\SpiralBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Squash.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Static.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Stretch.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\TimeBlur.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\TimeSlice.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\TimeWarp.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\TVPixel.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Vectorize.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Wave.cpp
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\lionelblairsmall.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pkgSamples.rc2
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Group "VJoDfxInclude"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColourWarpDfxDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\DfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\DilateDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\LevelsDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\MetaImageDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\pkgSamples.h
# End Source File
# Begin Source File

SOURCE=.\PShopFIltersDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextOverlayDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeSliceDfxDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeWarpDfxDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\WrapDynamicVars.h
# End Source File
# End Group
# Begin Group "CoreInclude"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Core\BackDropKey.h
# End Source File
# Begin Source File

SOURCE=..\Core\BackLight.h
# End Source File
# Begin Source File

SOURCE=..\Core\Bloom.h
# End Source File
# Begin Source File

SOURCE=..\Core\BoxFilter.h
# End Source File
# Begin Source File

SOURCE=..\Core\Burn.h
# End Source File
# Begin Source File

SOURCE=..\Core\ChannelFunction.h
# End Source File
# Begin Source File

SOURCE=..\Core\Chromium.h
# End Source File
# Begin Source File

SOURCE=..\Core\ColourConversion.h
# End Source File
# Begin Source File

SOURCE=..\Core\ColourReduce.h
# End Source File
# Begin Source File

SOURCE=..\Core\ColourWarp.h
# End Source File
# Begin Source File

SOURCE=..\Core\DeInterlace.h
# End Source File
# Begin Source File

SOURCE=..\Core\Dilate.h
# End Source File
# Begin Source File

SOURCE=..\Core\DirectionalBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\Dodge.h
# End Source File
# Begin Source File

SOURCE=..\Core\Exclusion.h
# End Source File
# Begin Source File

SOURCE=..\Core\GaussianBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\Glow.h
# End Source File
# Begin Source File

SOURCE=..\Core\HalfTone.h
# End Source File
# Begin Source File

SOURCE=..\Core\ImageMath.h
# End Source File
# Begin Source File

SOURCE=..\Core\ImageScale.h
# End Source File
# Begin Source File

SOURCE=..\Core\Kaleidascope.h
# End Source File
# Begin Source File

SOURCE=..\Core\Levels.h
# End Source File
# Begin Source File

SOURCE=..\Core\LionelBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\LiveFeed.h
# End Source File
# Begin Source File

SOURCE=..\Core\LumaCycle.h
# End Source File
# Begin Source File

SOURCE=..\Core\LumaKey.h
# End Source File
# Begin Source File

SOURCE=..\Core\LumaMask.h
# End Source File
# Begin Source File

SOURCE=..\Core\LumaOffset.h
# End Source File
# Begin Source File

SOURCE=..\Core\MetaImage.h
# End Source File
# Begin Source File

SOURCE=..\Core\MoireStewart.h
# End Source File
# Begin Source File

SOURCE=..\Core\Pete2dMatrix.h
# End Source File
# Begin Source File

SOURCE=..\Core\PeteHelpers.h
# End Source File
# Begin Source File

SOURCE=..\Core\Posterize.h
# End Source File
# Begin Source File

SOURCE=..\Core\RadialBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\RectDistort.h
# End Source File
# Begin Source File

SOURCE=..\Core\Refraction.h
# End Source File
# Begin Source File

SOURCE=..\Core\SimpleMirror.h
# End Source File
# Begin Source File

SOURCE=..\Core\Slide.h
# End Source File
# Begin Source File

SOURCE=..\Core\Smear.h
# End Source File
# Begin Source File

SOURCE=..\Core\Solarize.h
# End Source File
# Begin Source File

SOURCE=..\Core\SpiralBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\Squash.h
# End Source File
# Begin Source File

SOURCE=..\Core\Static.h
# End Source File
# Begin Source File

SOURCE=..\Core\Stretch.h
# End Source File
# Begin Source File

SOURCE=..\Core\Tile.h
# End Source File
# Begin Source File

SOURCE=..\Core\TimeBlur.h
# End Source File
# Begin Source File

SOURCE=..\Core\TimeSlice.h
# End Source File
# Begin Source File

SOURCE=..\Core\TimeWarp.h
# End Source File
# Begin Source File

SOURCE=..\Core\TVPixel.h
# End Source File
# Begin Source File

SOURCE=..\Core\Vectorize.h
# End Source File
# Begin Source File

SOURCE=..\VisualsFX\VisualsFX.h
# End Source File
# Begin Source File

SOURCE=..\Core\Wave.h
# End Source File
# End Group
# End Group
# End Target
# End Project
