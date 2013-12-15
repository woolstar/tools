#include "rrun.h"
#include "tscene.h"
#include "tscenelink.h"

   /***/

extern tlist_scene	tfocus;
extern shurt	etr_log;

extern void	visW_err(), visW_bad(), visW_message(),
		radjust_mapping(),
		rfree_lights();
extern char	*mem_name();

extern ev_ffile	*rbase_ffile;

int	(*rbuild_idle)()= NULL;

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

er_camera	magic_cam;

#if	SPHERE_SUPPORT
	er_smot		*magic_spot;
	er_sform	*magic_deform;
	er_smear	*magic_spheres;
#endif

er_smap		*magic_im= NULL;

   /***/

static char	ixpress[SLINE];

#define	RIDLE	if ((NULL != rbuild_idle) && (*rbuild_idle) ()) return 1;

void	rset_pref(void)
{
	val_ambient= tfocus.setup.ambient.g;
	val_rdepth= tfocus.setup.rdepth,
	val_gdepth= tfocus.setup.gdepth;
}

void	rset_time(int Time)
{
	global_time= Time;
}

void	rbuild_release(void)
{
	int i;
	ev_ffile *clean_fstep;
	er_smear *clean_bstep;

/*
	if (ct_grids)
		{ rfree_boxlist(); }
*/

	for (i= ct_acts, clean_fstep= rbase_ffile; (i--); clean_fstep++)
	{
		if (!clean_fstep-> stat_v) continue;
		clean_fstep-> global_v++;
		EFREE(clean_fstep-> global_v);
		EUFREE(clean_fstep-> local_mv);

		EFREE(clean_fstep-> mem_side);
		if (clean_fstep-> bits & RFACE_BUMP)
		{ EFREE(clean_fstep-> use_face.u); } else { EFREE(clean_fstep-> use_face.a); }
		EFREE(clean_fstep-> use_surf);
		EUFREE(clean_fstep-> use_block);
		EFREE(clean_fstep-> use_basis);
		EFREE(clean_fstep-> mem_insides);
		EUFREE(clean_fstep-> mem_xtrasi);
	}

#if	SPHERE_SUPPORT
	if (ct_ents)
	{
		for (i= ct_ents, clean_bstep= magic_spheres; (i--); clean_bstep++)
		{
			EFREE(clean_bstep-> bshad);
			EFREE(clean_bstep-> bsurf);
			EUFREE(clean_bstep-> bsolid);
		}
		EFREE(magic_spheres);
		EFREE(magic_spot);
		if (ct_deform) EFREE(magic_deform);
	}
#endif

	if (ct_acts)
		{ EFREE(rbase_ffile); }
}

void	rbuild_clean()
{
	er_smap *mimage_step;

	/* /// cleanup texture list */
	for(mimage_step= magic_im; (mimage_step); mimage_step= mimage_step-> next)
	{
		if (!mimage_step-> marked) continue;
		EFREE(mimage_step-> vdat);
		*(mimage_step-> imname)= '\0', mimage_step-> vdat= NULL;
		mimage_step-> marked= 0;
	}
}

