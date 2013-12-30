#include <general.h>
#include <aart.h>

#include "y_core.h"
#include	"rdef.h"
#include	"tfren.h"
#include	"wpdat.h"

#ifdef __cplusplus
	extern "C" {
#endif
	extern 	int	(*rbuild_idle)() ;

#ifdef __cplusplus
	} ;
#endif

#define	GM(aa, bb)	(bb *) malloc(aa * sizeof(bb))
#define	GF(aa)	{ free(aa); aa= NULL; }
#define	GFU(aa)	if (aa) GF(aa) ;

#define	TRENDER_THREAD	1

#ifdef WIN32
// #	pragma warning( disable : 4018)
#	pragma warning( disable : 4244)
#	pragma warning( disable : 4305)
#endif

