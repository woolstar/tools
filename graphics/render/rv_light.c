#include "rrun.h"

	extern int	ct_lights ;
	extern ev_light	* magic_light ;

void	rvm_safe(void) ;
void	rvm_destroy(void) ;


void	rvm_safe(void)
{
	int count ;
	ev_light * step = magic_light;

	for (count= ct_lights ; (count --); step ++)
	{
		step-> cache_1= NULL,
		step-> cache_2= NULL;
	}
}

void	rvm_destroy(void)
{
	int i;
	ev_light *lstep;
	ev_lightshadowbuffer *lbck;

	if (! ct_lights || ! magic_light) return;
	for (i= ct_lights, lstep= magic_light; (i --); lstep ++)
	{
		if (NULL == lstep-> lshadmap) continue;

			/*** free shadow buffers */
		for (lbck= lstep-> lshadmap; 1; lbck ++)
		{
			EFREE(lbck-> buffer);
			if (lbck-> plast) break;
		}
		EFREE(lstep-> lshadmap);
	}

	EFREE(magic_light);
	ct_lights= 0,
	magic_light= NULL;
}
