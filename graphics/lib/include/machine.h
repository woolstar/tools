   /****
      Copyright 1994 Video Bits
      All Rights Reserved

      machine.h -- OS portability definitions
      */

#define	A_MACHINE	1

#ifdef THINK_C
#       define	MAC
#       endif
#ifdef MPW
#       define	MAC
#       endif
#ifdef __DUMP__
#       define	MAC
#       endif
#ifdef mpwgcc
#       define	MAC
#       endif

#ifdef	COMMAND
#	undef	MAC
#	define	MPW
#	endif

   /**** force definition */
#define	DOS 1

#ifndef	M_PI
#	define	M_PI		3.14159265359
#	endif

#ifdef	DOS
#	define	bzero(XX, SS)	memset(XX, 0, SS)
#	endif

#ifdef	UNIX
#	define	RAELUNIK	1
#	define	HWND	void
#	define	strnicmp	strncasecmp
#	endif

#define	BUGG
/* #define	HARD */
/* #define	SAMPLE */
