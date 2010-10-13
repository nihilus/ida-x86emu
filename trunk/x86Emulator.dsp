# Microsoft Developer Studio Project File - Name="x86Emulator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=x86Emulator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "x86Emulator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "x86Emulator.mak" CFG="x86Emulator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "x86Emulator - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "x86Emulator - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "x86Emulator - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "X86EMULATOR_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "__IDP__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__NT__" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ida.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\..\bin\plugins\x86emu.plw" /libpath:"..\..\..\lib\vc.w32" /export:PLUGIN

!ELSEIF  "$(CFG)" == "x86Emulator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "X86EMULATOR_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /I "..\..\..\include" /D "__NT__" /D MAXSTR=1024 /D "__IDP__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ida.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\..\bin\plugins\x86emu.plw" /libpath:"..\..\..\lib\vc.w32" /export:PLUGIN
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "x86Emulator - Win32 Release"
# Name "x86Emulator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\break.cpp
# End Source File
# Begin Source File

SOURCE=.\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\cpu.cpp
# End Source File
# Begin Source File

SOURCE=.\emu_script.cpp
# End Source File
# Begin Source File

SOURCE=.\emufuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\emuheap.cpp
# End Source File
# Begin Source File

SOURCE=.\emuthreads.cpp
# End Source File
# Begin Source File

SOURCE=.\hooklist.cpp
# End Source File
# Begin Source File

SOURCE=.\memmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\peutils.cpp
# End Source File
# Begin Source File

SOURCE=.\seh.cpp
# End Source File
# Begin Source File

SOURCE=.\x86emu.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\break.h
# End Source File
# Begin Source File

SOURCE=.\buffer.h
# End Source File
# Begin Source File

SOURCE=.\cpu.h
# End Source File
# Begin Source File

SOURCE=.\elf32.h
# End Source File
# Begin Source File

SOURCE=.\elf_common.h
# End Source File
# Begin Source File

SOURCE=.\emu_script.h
# End Source File
# Begin Source File

SOURCE=.\emufuncs.h
# End Source File
# Begin Source File

SOURCE=.\emuheap.h
# End Source File
# Begin Source File

SOURCE=.\emuthreads.h
# End Source File
# Begin Source File

SOURCE=.\hooklist.h
# End Source File
# Begin Source File

SOURCE=.\memmgr.h
# End Source File
# Begin Source File

SOURCE=.\peutils.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sdk_versions.h
# End Source File
# Begin Source File

SOURCE=.\seh.h
# End Source File
# Begin Source File

SOURCE=.\x86defs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\dialog.rc
# End Source File
# End Group
# End Target
# End Project
