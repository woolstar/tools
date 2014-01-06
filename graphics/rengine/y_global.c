#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"

   /***/

// int	(*rbuild_idle)()= NULL;

int	global_time,
	val_rdepth, val_gdepth,
	ct_acts, ct_ents,
	ct_ball, ct_deform;
long	mem_phase1, mem_phase2, mem_phase3;

double		val_ambient= 0.33;

#if ENVIR_SUPPORT
vekU		fogmath;
colU		fogcol,
		ev0_topc, ev0_upperc, ev0_lowerc, ev0_bottomc,
		ev1_topc, ev1_upperc, ev1_lowerc, ev1_bottomc;
#endif

