#ifndef	GENERALSYSTEM_
#	define	GENERALSYSTEM_	1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <sys/types.h>

#ifdef IRIX
#	include <bstring.h>
#endif

#ifndef	M_PI
#	define	M_PI	3.14159265359
#endif

#ifdef __cplusplus

#	include	<generalexception.h>
#	include	<generallog.h>
#	include	<generalstring.h>

#	define	GL	generallog
#	define	GS	generalstring

#	endif

#endif
