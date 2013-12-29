#include "rrun.h"

ray_element	*current;
unsigned short ray_num, ray_rdepth, ray_gdepth;
int 	ray_set;

extern	int		ct_grids;
extern	er_grid	*magic_grid;

void	rclear_facc(void);

void	rstart_count(void)
{
	ray_num= 0, ray_set= 0;
	rclear_facc();
}

void	rclear_facc(void)
{
	int istep, jstep;
	long k;
	er_grid *gstep;
	er_face *fstep;
	tr_object **tlist, *tstep;
	
	for (istep= ct_grids, gstep= magic_grid; (istep --); gstep ++)
	{
		for (jstep= gstep-> cn, tlist= (tr_object **) gstep-> contain; (jstep --); tlist ++)
		{
			tstep= *tlist;
			for (k= tstep-> ta.poly_c, fstep= (er_face *) tstep-> ta.flist;
				(k--); fstep++) fstep-> ray_count= 0;
		}
	}
}

