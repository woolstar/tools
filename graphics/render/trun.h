#include	"basic.h"
#include	"tscene.h"
#include	"tscenelink.h"
#include	"tfren.h"
#include	"wpdat.h"
#include	"rdef.h"

extern	double	epart_motionvalue(int, tlist_element *, int, ts_track *);
extern void	epart_motionvec(svector *ptr, int track, tlist_element *, int pos, ts_track *);
extern int	epart_setmotionvalue(float value, int offset, tlist_element * , int pos, ts_track *) ;

#ifdef WIN32
#	pragma warning( disable : 4244)
#endif

#define	TRENDER_THREAD	1
#define	TGEOM_GENERATE	1

