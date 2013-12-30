# Microsoft Developer Studio Project File - Name="aart" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (ALPHA) Static Library" 0x0604
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=aart - Win32 AlphaDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "aart.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "aart.mak" CFG="aart - Win32 AlphaDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "aart - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "aart - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "aart - Win32 AlphaDebug" (based on "Win32 (ALPHA) Static Library")
!MESSAGE "aart - Win32 AlphaRelease" (based on "Win32 (ALPHA) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "aart - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"aar.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX"aar.h" /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "aart___Win32_AlphaDebug"
# PROP BASE Intermediate_Dir "aart___Win32_AlphaDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ADlib"
# PROP Intermediate_Dir "ADebug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"aar.h" /FD /GZ /c
# ADD CPP /nologo /Gt0 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"aar.h" /FD /GZ /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "aart___Win32_AlphaRelease"
# PROP BASE Intermediate_Dir "aart___Win32_AlphaRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ARlib"
# PROP Intermediate_Dir "ARelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"aar.h" /FD /c
# ADD CPP /nologo /Gt0 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"aar.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "aart - Win32 Release"
# Name "aart - Win32 Debug"
# Name "aart - Win32 AlphaDebug"
# Name "aart - Win32 AlphaRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aartbase.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartcamera.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartcomponent.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartcore.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aarteffect.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartelement.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartgen.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartgeometry.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartlight.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartlink.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartnode.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartoutput.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartpost.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartscene.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartsetup.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aartsurface.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aarttexture.cpp

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\angle.c

!IF  "$(CFG)" == "aart - Win32 Release"

!ELSEIF  "$(CFG)" == "aart - Win32 Debug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaDebug"

!ELSEIF  "$(CFG)" == "aart - Win32 AlphaRelease"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\aart.h
# End Source File
# Begin Source File

SOURCE=.\aart_base.h
# End Source File
# Begin Source File

SOURCE=.\aart_core.h
# End Source File
# Begin Source File

SOURCE=.\aart_element.h
# End Source File
# Begin Source File

SOURCE=.\aart_gen.h
# End Source File
# Begin Source File

SOURCE=.\aart_link.h
# End Source File
# Begin Source File

SOURCE=.\aart_node.h
# End Source File
# Begin Source File

SOURCE=.\aart_output.h
# End Source File
# Begin Source File

SOURCE=.\aart_post.h
# End Source File
# Begin Source File

SOURCE=.\aart_scene.h
# End Source File
# End Target
# End Project
