# Microsoft Developer Studio Project File - Name="zbot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=zbot - Win32 Debug Alpha
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zbot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zbot.mak" CFG="zbot - Win32 Debug Alpha"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zbot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zbot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zbot - Win32 Debug Alpha" (based on "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE "zbot - Win32 Release Alpha" (based on "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "zbot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "BUILDING_REF_GL" /D "STDC_HEADERS" /D "REGEX_MALLOC" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release/gamex86.dll"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "BUILDING_REF_GL" /D "STDC_HEADERS" /D "REGEX_MALLOC" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"..\..\release\gamex86.dll"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Alpha"
# PROP BASE Intermediate_Dir "Debug Alpha"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\DebugAxp"
# PROP Intermediate_Dir ".\DebugAxp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /MTd /c
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /QA21164 /MTd /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x20000000" /subsystem:windows /dll /debug /machine:ALPHA
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x20000000" /subsystem:windows /dll /debug /machine:ALPHA /out:"..\DebugAxp/gameaxp.dll"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "game___W"
# PROP BASE Intermediate_Dir "game___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\ReleaseAXP"
# PROP Intermediate_Dir ".\ReleaseAXP"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /Gt0 /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gt0 /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /QA21164 /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x20000000" /subsystem:windows /dll /machine:ALPHA /out:"..\Release/gamex86.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /base:"0x20000000" /subsystem:windows /dll /machine:ALPHA /out:"..\ReleaseAXP/gameaxp.dll"

!ENDIF 

# Begin Target

# Name "zbot - Win32 Release"
# Name "zbot - Win32 Debug"
# Name "zbot - Win32 Debug Alpha"
# Name "zbot - Win32 Release Alpha"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\g_main.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

DEP_CPP_G_MAI=\
	".\g_local.h"\
	".\game.h"\
	
NODEP_CPP_G_MAI=\
	"..\qcommon\qcommon.h"\
	"..\qcommon\qfiles.h"\
	"..\server\server.h"\
	

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_ban.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_clib.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_cmd.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_disable.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_flood.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_init.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_log.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_lrcon.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_msgqueue.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_spawn.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_util.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_vote.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_zbot.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zb_zbotcheck.c

!IF  "$(CFG)" == "zbot - Win32 Release"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug"

!ELSEIF  "$(CFG)" == "zbot - Win32 Debug Alpha"

!ELSEIF  "$(CFG)" == "zbot - Win32 Release Alpha"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zbot.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\g_local.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\q_shared.h
# End Source File
# Begin Source File

SOURCE=.\regex.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Configuration Files"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\q2admin.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminban.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminflood.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminlog.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminlrcon.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminspawn.txt
# End Source File
# Begin Source File

SOURCE=.\q2adminvote.txt
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Group
# End Target
# End Project
