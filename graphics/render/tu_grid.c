#include "rrun.h"

	int	ct_grids;
	er_grid	*magic_grid;

static	er_grid	*rbase_grid= NULL;
static	er_cell	*rbase_cell;
static	er_face	**rbase_gridlist;

	extern void	rgrid_size(er_grid *, long *) ;
	extern void	rgrid_fill(er_grid *, er_face **, long *) ;

int	tgrid_build(int n, tr_set *list)
{
	int
		istep, jstep,
		gbuild_cx, gbuild_cy, gbuild_cz;
	long
		il, total_cells, total_dyne,
		calc_cellbound, calc_cellct;
		
	double dtmp, da, db;
	vector
		gsize, gdivis;
	er_face **face_fill;
	er_grid *gridstep;
	er_cell *cell_fill;

	tr_set *tstep;
	tr_object **oblist, *obstep;
	
	rbase_grid= EMEM(n, er_grid);
	if (NULL == rbase_grid) RE
	
	total_cells= 0;

	for (istep= n, gridstep= rbase_grid, tstep= list; (istep --); gridstep ++, tstep ++)
	{
		calc_cellbound= 0;
		for (jstep= tstep-> count, oblist= tstep-> list; (jstep --); oblist ++)
			{ obstep= *oblist, calc_cellbound += obstep-> ta.vector_c; }
		gridstep-> cn= tstep-> count,
		gridstep-> contain= (ev_ffile **) tstep-> list;
		da= calc_cellbound;
		if (calc_cellbound < 1100) db= da * 1.1;
			else if (calc_cellbound < 8000) db= pow(da, 1.1);
				else if (calc_cellbound < 26000) db= pow(da, 1.25);
					else db= pow(da, 1.41);
		calc_cellbound= db;
		
		M_sub(gsize, tstep-> jbound, tstep-> ibound)
		M_cpy(gdivis, gsize)
		gbuild_cx= 1, gbuild_cy= 1, gbuild_cz= 1, calc_cellct= 1;
		while (calc_cellct < calc_cellbound)
		{
			if ((gdivis.x > gdivis.y) && (gdivis.x > gdivis.z))
			{
				gbuild_cx++, calc_cellct+= gbuild_cy * gbuild_cz;
				gdivis.x= gsize.x / gbuild_cx;
			}
			else
			if (gdivis.y > gdivis.z)
			{
				gbuild_cy++, calc_cellct+= gbuild_cx * gbuild_cz;
				gdivis.y= gsize.y / gbuild_cy;
			}
			else
			{
				gbuild_cz++, calc_cellct+= gbuild_cy * gbuild_cx;
				gdivis.z= gsize.z / gbuild_cz;
			}
		}
		
		gridstep-> sx= gbuild_cx,
		gridstep-> sy= gbuild_cy,
		gridstep-> sz= gbuild_cz,
		gridstep-> cell_ct= calc_cellct,
		gridstep-> cell_subz= gbuild_cx * gbuild_cy;
		gridstep-> lb= tstep-> ibound, gridstep-> hb= tstep-> jbound;
		dtmp= 0.0000001 * ((db= tstep-> jbound.x) -(da= tstep-> ibound.x)) +0.01;
			dtmp= dtmp * gbuild_cx +0.0001, db+= dtmp, da-= dtmp;
			gridstep-> vcbase.x= da,
			gridstep-> vcsize.x= dtmp= (db -da) / gbuild_cx,
			gridstep-> vcscale.x= 1.0 / dtmp;
		dtmp= 0.0000001 * ((db= tstep-> jbound.y) -(da= tstep-> ibound.y)) +0.01;
			dtmp= dtmp * gbuild_cy +0.0001, db+= dtmp, da-= dtmp;
			gridstep-> vcbase.y= da,
			gridstep-> vcsize.y= dtmp= (db -da) / gbuild_cy,
			gridstep-> vcscale.y= 1.0 / dtmp;
		dtmp= 0.0000001 * ((db= tstep-> jbound.z) -(da= tstep-> ibound.z)) +0.01;
			dtmp= dtmp * gbuild_cz +0.0001, db+= dtmp, da-= dtmp;
			gridstep-> vcbase.z= da,
			gridstep-> vcsize.z= dtmp= (db -da) / gbuild_cz,
			gridstep-> vcscale.z= 1.0 / dtmp;

		total_cells+= calc_cellct;
	}

	if (! total_cells) RE
	rbase_cell= EMEM(total_cells, er_cell);
		if (NULL == rbase_cell) RE

	for (il= total_cells, cell_fill= rbase_cell; (il--); cell_fill++)
		{ cell_fill-> count_tiny= cell_fill-> count_large= 0; }

	total_dyne= 0;
	for (istep= n, gridstep= rbase_grid, cell_fill= rbase_cell; (istep --); gridstep ++)
	{
		il= gridstep-> cell_ct;
		gridstep-> cells= cell_fill, cell_fill += il;
		gridstep-> cellsmax= cell_fill;
		rgrid_size(gridstep, &il);
		total_dyne += il;
	}

	if (! total_dyne)
	{
		printf(" >> no cells created for grids\n");
		RE
	}
	rbase_gridlist= face_fill= EMEM(total_dyne, er_face *);
		if (NULL == rbase_gridlist) RE
	bzero((char *) rbase_gridlist, total_dyne * sizeof(er_face *));

	for (istep= n, gridstep= rbase_grid; (istep --); gridstep ++)
	{
		rgrid_fill(gridstep, face_fill, &il);
		face_fill += il;
	}

	magic_grid= rbase_grid,
	ct_grids= n;

	return 0;
}

void	tgrid_destroy(void)
{
	if (NULL == rbase_grid) return;
	
	EFREE(rbase_grid-> contain);	// partition

	EFREE(rbase_grid);  rbase_grid= NULL, magic_grid= NULL;
	EUFREE(rbase_cell);
	EUFREE(rbase_gridlist);

	rbase_cell= NULL ;
	rbase_gridlist= NULL ;

	ct_grids= 0 ;
}
