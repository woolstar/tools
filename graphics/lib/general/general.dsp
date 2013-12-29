# Microsoft Developer Studio Project File - Name="general" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (ALPHA) Static Library" 0x0604
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=general - Win32 AlphaDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "general.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "general.mak" CFG="general - Win32 AlphaDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "general - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "general - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "general - Win32 AlphaDebug" (based on "Win32 (ALPHA) Static Library")
!MESSAGE "general - Win32 AlphaRelease" (based on "Win32 (ALPHA) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/Adspace/Prototypes/Lib/General/general"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "general - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_LIB" /YX"general.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "general_"
# PROP BASE Intermediate_Dir "general_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"general.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"dgeneral.lib"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "general___Win32_AlphaDebug"
# PROP BASE Intermediate_Dir "general___Win32_AlphaDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ADlib"
# PROP Intermediate_Dir "ADebug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Z7 /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"general.h" /FD /c
# ADD CPP /nologo /Gt0 /W3 /GX /Z7 /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"general.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"dgeneral.lib"
# ADD LIB32 /nologo /out:"dgeneral.lib"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "general___Win32_AlphaRelease"
# PROP BASE Intermediate_Dir "general___Win32_AlphaRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ARlib"
# PROP Intermediate_Dir "ARelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_LIB" /YX"general.h" /FD /c
# ADD CPP /nologo /Gt0 /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_LIB" /YX"general.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "general - Win32 Release"
# Name "general - Win32 Debug"
# Name "general - Win32 AlphaDebug"
# Name "general - Win32 AlphaRelease"
# Begin Group "Source"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\generalbag.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generaldata.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalexception.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalfileoutput.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalhash.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generallinkable.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generallist.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generallistiterator.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generallog.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalobject.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalrandom.c

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalstring.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalstringlist.cpp

!IF  "$(CFG)" == "general - Win32 Release"

!ELSEIF  "$(CFG)" == "general - Win32 Debug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "general - Win32 AlphaRelease"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\general.h
# End Source File
# Begin Source File

SOURCE=.\generalbag.h
# End Source File
# Begin Source File

SOURCE=.\generaldata.h
# End Source File
# Begin Source File

SOURCE=.\generalexception.h
# End Source File
# Begin Source File

SOURCE=.\generalhash.h
# End Source File
# Begin Source File

SOURCE=.\generallinkable.h
# End Source File
# Begin Source File

SOURCE=.\generallist.h
# End Source File
# Begin Source File

SOURCE=.\generallog.h
# End Source File
# Begin Source File

SOURCE=.\generalobject.h
# End Source File
# Begin Source File

SOURCE=.\generaloutput.h
# End Source File
# Begin Source File

SOURCE=.\generalrandom.h
# End Source File
# Begin Source File

SOURCE=.\generalstring.h
# End Source File
# Begin Source File

SOURCE=.\generalstringlist.h
# End Source File
# End Target
# End Project
