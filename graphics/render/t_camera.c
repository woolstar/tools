#include "trun.h"

extern int		global_time,
				aa_samp, aa_width, aa_stretch, aa_window;
extern tlist_scene	tfocus;

tr_camera	rendercam, tmpcam, *curcam;

#if 0
AV_WINDOW
AV_WIDEVIEW

	tfocus.setup.curcam
	thing
	{
		shurt fwarp_trak;
		short window_lx, window_ly, window_hx, window_hy;
	} ep_camera;

#endif

void	tcam_buildview(void)
{
	int j, tk, cmx, cmy;
	double dtmp, wfov, wasr;
	svector c_at, c_dir, c_up,
		c_across, cmix;
	tscene_camera *camptr;

	rendercam.attrib= 0;
	camptr= tfocus.setup.curcam;
	j= camptr-> attr, tk= camptr-> motion;

	if (j & TSC_IMPORT)
	{
		tsimport_motionvec(& c_at, tk, global_time +1, camptr-> import);  tk+= 3;
		tsimport_motionvec(& c_dir, tk, global_time +1, camptr-> import);  tk+= 3;
		tsimport_motionvec(& c_up, tk, global_time +1, camptr-> import);  tk+= 3;

		wfov= 0.5 * tsimport_motionvalue(tk, global_time +1, camptr-> import);
	}
	else
	{
		epart_motionvec(& c_at, tk, NULL, global_time +1, & (tfocus.tracks));  tk+= 3;
		epart_motionvec(& c_dir, tk, NULL, global_time +1, & (tfocus.tracks));  tk+= 3;
		epart_motionvec(& c_up, tk, NULL, global_time +1, & (tfocus.tracks));  tk+= 3;

		wfov= 0.5 * epart_motionvalue(tk, NULL, global_time +1, & (tfocus.tracks));
	}

	M_sub(c_dir, c_dir, c_at)  M_selfnorm(c_dir)
	M_cross(c_across, c_dir, c_up)  M_selfnorm(c_across)
	M_cross(c_up, c_across, c_dir)  M_selfnorm(c_up)
	M_neg(c_dir)
	cmix.x= M_dot(c_at, c_across),
	cmix.y= M_dot(c_at, c_up),
	cmix.z= M_dot(c_at, c_dir);

	M_cpy(rendercam.basev, cmix)

	wasr= 1.0;
	if (j & TSC_ASPECT)
	{
		if (! camptr-> y || (camptr-> gaspect < 1e-5)) wasr= 0.0;
		else
			wasr= camptr-> x / (camptr-> gaspect * camptr-> y);
	}

		/*** install ***/
	rendercam.cfov= wfov;
	dtmp= 1.0 / tan(M_PI * wfov / 180.0);

	if (aa_samp)
	{
		cmx= aa_samp * camptr-> x,
		cmy= aa_samp * camptr-> y;
		rendercam.scale_x= 0.5 * dtmp * wasr * cmy,
		rendercam.scale_y= 0.5 * dtmp * cmy;
		rendercam.offset_x= 0.5 * (1.0 + cmx + 2* aa_stretch),
		rendercam.offset_y= 0.5 * (1.0 + cmy),

		rendercam.view_ixl= 0,
		rendercam.view_ixr= cmx + 2* aa_stretch;
		rendercam.view_iyb= - aa_stretch,
		rendercam.view_iyt= cmy +aa_stretch;

		rendercam.shadrate= 1.5 * aa_samp;
	}
	else
	{
		cmx= camptr-> x,
		cmy= camptr-> y;
		rendercam.scale_x= 0.5 * dtmp * wasr * cmy,
		rendercam.scale_y= 0.5 * dtmp * cmy;
		rendercam.offset_x= 0.5 * (1.0 + cmx),
		rendercam.offset_y= 0.5 * (1.0 + cmy),
		rendercam.view_ixl= 0, rendercam.view_ixr= cmx;
		rendercam.view_iyb= 0, rendercam.view_iyt= cmy;

		rendercam.shadrate= 1;
	}
	rendercam.offset_z= 0.02,
	rendercam.scale_z= 1048560.0 / (1000.0 - rendercam.offset_z);

	M_cpy(rendercam.cpos, c_at)
	rendercam.cview= c_dir,
	rendercam.ctop= c_up,
	rendercam.cside= c_across;
}

void	tcam_buildslot(int yl, int yh)
{
	if (aa_samp)
	{
		rendercam.view_iyb= aa_samp * yl - aa_stretch,
		rendercam.view_iyt= aa_samp * yh + aa_stretch;
	}
	else
	{
		rendercam.view_iyb= yl,
		rendercam.view_iyt= yh;
	}
}

void	tcam_buildlight(void)
{
	tmpcam.attrib= 0;

	curcam= & tmpcam;
}

void	tcam_setview(void)
{
	curcam= & rendercam;
}

