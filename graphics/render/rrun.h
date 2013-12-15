#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

   /***** portability stuff */

#include "basic.h"
#include	"wpdat.h"
#include	"tfren.h"
#include	"rdef.h"

#ifdef DOS
#	define	visT_nut()
#	endif
#ifdef RAELUNIK
#include <sys/types.h>
#	define	SIZEOF		sizeof
#	define	visT_nut()
#	endif


#define	rfilter_start()
#define	rfilter_row()
#define	rfilter_final()

#ifdef WIN32
#	pragma warning( disable : 4244)
#endif

#define	TRENDER_THREAD	1
