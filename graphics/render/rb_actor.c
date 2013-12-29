#include "trun.h"

   /***/

extern int	global_time,
		val_rdepth, val_gdepth;

#if ENVIR_SUPPORT
	extern vector	fogmath;
	int	ct_fog, ct_env0, ct_env1;
#endif

   /***/

int		rmem_lights() { return 0; }
void	rfree_lights(void) { }
void	rbuild_cam(void) { }

void		rbuild_env(void)
{
#if ENVIR_SUPPORT
	ct_fog= 0;
	fogmath.z= 1e304;
#endif
}

