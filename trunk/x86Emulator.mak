# Microsoft Developer Studio Generated NMAKE File, Based on x86Emulator.dsp
!IF "$(CFG)" == ""
CFG=x86Emulator - Win32 Debug
!MESSAGE No configuration specified. Defaulting to x86Emulator - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "x86Emulator - Win32 Release" && "$(CFG)" != "x86Emulator - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "x86Emulator - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\bin\plugins\x86emu.plw"


CLEAN :
	-@erase "$(INTDIR)\break.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\context.obj"
	-@erase "$(INTDIR)\cpu.obj"
	-@erase "$(INTDIR)\dialog.res"
	-@erase "$(INTDIR)\emu_script.obj"
	-@erase "$(INTDIR)\emufuncs.obj"
	-@erase "$(INTDIR)\emuheap.obj"
	-@erase "$(INTDIR)\emuthreads.obj"
	-@erase "$(INTDIR)\hooklist.obj"
	-@erase "$(INTDIR)\memmgr.obj"
	-@erase "$(INTDIR)\peutils.obj"
	-@erase "$(INTDIR)\seh.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x86emu.obj"
	-@erase "$(OUTDIR)\x86emu.exp"
	-@erase "$(OUTDIR)\x86emu.lib"
	-@erase "..\..\bin\plugins\x86emu.plw"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../../include" /D "NDEBUG" /D "__IDP__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "__NT__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dialog.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\x86Emulator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ida.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\bin\plugins\x86emu.plw" /implib:"$(OUTDIR)\x86emu.lib" /libpath:"..\..\libvc.w32" /export:PLUGIN 
LINK32_OBJS= \
	"$(INTDIR)\break.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\context.obj" \
	"$(INTDIR)\cpu.obj" \
	"$(INTDIR)\emu_script.obj" \
	"$(INTDIR)\emufuncs.obj" \
	"$(INTDIR)\emuheap.obj" \
	"$(INTDIR)\emuthreads.obj" \
	"$(INTDIR)\hooklist.obj" \
	"$(INTDIR)\memmgr.obj" \
	"$(INTDIR)\peutils.obj" \
	"$(INTDIR)\seh.obj" \
	"$(INTDIR)\x86emu.obj" \
	"$(INTDIR)\dialog.res"

"..\..\bin\plugins\x86emu.plw" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "x86Emulator - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\bin\plugins\x86emu.plw"


CLEAN :
	-@erase "$(INTDIR)\break.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\context.obj"
	-@erase "$(INTDIR)\cpu.obj"
	-@erase "$(INTDIR)\dialog.res"
	-@erase "$(INTDIR)\emu_script.obj"
	-@erase "$(INTDIR)\emufuncs.obj"
	-@erase "$(INTDIR)\emuheap.obj"
	-@erase "$(INTDIR)\emuthreads.obj"
	-@erase "$(INTDIR)\hooklist.obj"
	-@erase "$(INTDIR)\memmgr.obj"
	-@erase "$(INTDIR)\peutils.obj"
	-@erase "$(INTDIR)\seh.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x86emu.obj"
	-@erase "$(OUTDIR)\x86emu.exp"
	-@erase "$(OUTDIR)\x86emu.lib"
	-@erase "..\..\bin\plugins\x86emu.plw"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Gz /MT /W3 /GX /O2 /I "..\..\include" /D "__NT__" /D MAXSTR=1024 /D "__IDP__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dialog.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\x86Emulator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ida.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\bin\plugins\x86emu.plw" /implib:"$(OUTDIR)\x86emu.lib" /libpath:"..\..\libvc.w32" /export:PLUGIN 
LINK32_OBJS= \
	"$(INTDIR)\break.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\context.obj" \
	"$(INTDIR)\cpu.obj" \
	"$(INTDIR)\emu_script.obj" \
	"$(INTDIR)\emufuncs.obj" \
	"$(INTDIR)\emuheap.obj" \
	"$(INTDIR)\emuthreads.obj" \
	"$(INTDIR)\hooklist.obj" \
	"$(INTDIR)\memmgr.obj" \
	"$(INTDIR)\peutils.obj" \
	"$(INTDIR)\seh.obj" \
	"$(INTDIR)\x86emu.obj" \
	"$(INTDIR)\dialog.res"

"..\..\bin\plugins\x86emu.plw" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("x86Emulator.dep")
!INCLUDE "x86Emulator.dep"
!ELSE 
!MESSAGE Warning: cannot find "x86Emulator.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "x86Emulator - Win32 Release" || "$(CFG)" == "x86Emulator - Win32 Debug"
SOURCE=.\break.cpp

"$(INTDIR)\break.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\buffer.cpp

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\context.cpp

"$(INTDIR)\context.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cpu.cpp

"$(INTDIR)\cpu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\emu_script.cpp

"$(INTDIR)\emu_script.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\emufuncs.cpp

"$(INTDIR)\emufuncs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\emuheap.cpp

"$(INTDIR)\emuheap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\emuthreads.cpp

"$(INTDIR)\emuthreads.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hooklist.cpp

"$(INTDIR)\hooklist.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\memmgr.cpp

"$(INTDIR)\memmgr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\peutils.cpp

"$(INTDIR)\peutils.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\seh.cpp

"$(INTDIR)\seh.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\x86emu.cpp

"$(INTDIR)\x86emu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dialog.rc

"$(INTDIR)\dialog.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

