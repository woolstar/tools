#include "trun.h"

static int		mlfirst= 1;

extern int		global_time,
				pa_width, pa_height, pa_slot;

static void	mlrender_first(void)
{
	// create light storage areas
	// create light shadow buffers

	/** rmem_lights() **/
	/** rmem_boxlist() **/

	// create image buffer for phong

	mlfirst= 0;
}

static void	mlrender_slot(void)
{
	int i, iy;

	cphong_pfirst();

	printf(" sections: ");  fflush(stdout);
	for (i= 0; (i < pa_height); i+= pa_slot)
	{
		tcam_buildslot(i, i +pa_slot);
		w_tilerclear();
		tgeom_tile();
		cphong_part();
		printf(".");  fflush(stdout);
	}
	printf(" done\n");

	cphong_pfinal();
}

int	mlrender_frame(int framenumber)
{
	if (mlfirst) mlrender_first();
/*
	testopen();
*/
	dx_tileinit();

	rset_time(framenumber);
	cpmap_start();

	cto_load();
	if (cp_prepbuffer()) return 1;
	global_time= framenumber;

	w_tilerclear();
	tcam_buildview();

	tgeom_load();

	if (tlight_create()) return 1;
	rbuild_env();
	if (tgeom_ray()) RE
	if (tgeom_grid()) RE
	if (cpmap_load()) RE

	tcam_setview();
	rstart_count();
	tgeom_range();

	if (pa_slot == pa_height)
	{
		tgeom_tile();
		cphong_save();
	}
		else mlrender_slot();

	rbuild_release();
	cpmap_release();
	w_tilerrel();

	tgeom_dispose();
	tlight_destroy();

	cto_dispose();

/*
	rbuild_sphere(fopen)
	radjust_mapping();
*/

/*
	testclose();
*/

	return 0;
}

void	mlrender_finish(void)
{
	mlfirst= 1;

	cpmap_destroy();
}

