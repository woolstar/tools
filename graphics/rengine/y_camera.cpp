#include "ye.h"

#include	"y_camera.h"

	static vector	camera_dir, camera_at;
	static double	camera_zpos;

extern "C"
{
	// camera

	tr_camera	rendercam, tmpcam, *curcam;

		// external
		// dx_post
	extern int	aa_samp, aa_width, aa_stretch, aa_window ;
} ;

void	yzor::setview()
{
	curcam= & rendercam ;
}

bool	yzor::docamera(void)
{
	const aart_post * postref ;
	aart::camera * tmpcam ;
	aart_node::cameraset * cptr ;

	if (mcamera) 
		return docamera(mcamera) ;
	cptr= mnode-> cameraref ;
	if (! cptr) cptr= mbase-> cameraref ;

		// retrieve current camera
	postref= aart_post::getpost() ;
	tmpcam= postref-> getactive(cptr-> getstep()) ;
	if (tmpcam != mcamera)
	{
		if (! docamera(tmpcam))
			return false ;
	}

	return true ;
}

bool	yzor::docamera(aart::camera * acam)
{
	double dtmp, wasr;
	int cmx, cmy ;
	aart::setup * setupref ;
	float fov ;
	vector	tmpPos ;
	quaternion	tmpQ ;
	ymotioncam	* ymo= NULL ;

	ASSERTPOINTER(acam) ;

	mcamera= acam ;
	if (mcamera-> mlink && mcamera-> mimport)
		ymo= (ymotioncam *) mcamera-> mlink ;

	wasr= 1.0;
	setupref= & (mscene-> setup_data) ;
	wasr= setupref-> pixel_aspect ;

	fov= mcamera-> fov ;
	if (ymo) ymo-> getFOV(t, fov) ;
	rendercam.cfov= fov ;
	dtmp= 1.0 / tan(M_PI * rendercam.cfov / 360.0) ;

	if (aa_samp)
	{
		cmx= aa_samp * setupref-> x,
		cmy= aa_samp * setupref-> y;
		rendercam.offset_x= 0.5 * (1.0 + cmx + 2* aa_stretch),
		rendercam.offset_y= 0.5 * (1.0 + cmy),

		rendercam.view_ixl= 0,
		rendercam.view_ixr= cmx + 2* aa_stretch;
		rendercam.view_iyb= - aa_stretch,
		rendercam.view_iyt= cmy +aa_stretch;

		rendercam.shadrate= (int) (0.75 * aa_samp);
	}
	else
	{
		cmx= setupref-> x,
		cmy= setupref-> y;
		rendercam.offset_x= 0.5 * (1.0 + cmx),
		rendercam.offset_y= 0.5 * (1.0 + cmy);
		rendercam.view_ixl= 0, rendercam.view_ixr= cmx;
		rendercam.view_iyb= 0, rendercam.view_iyt= cmy;

		rendercam.shadrate= 1;
	}

	rendercam.scale_x= 0.5 * dtmp * cmy / wasr,
	rendercam.scale_y= 0.5 * dtmp * cmy;
	rendercam.scale_z= 1048560.0 / (1000.0 - rendercam.offset_z);
	rendercam.offset_z= 0.02,

	tmpPos= mcamera-> position ;
	tmpQ= mcamera-> direction ;
	if (ymo) ymo->getMotion(t, tmpPos, tmpQ) ;

	M_cpy(rendercam.cpos, tmpPos) ;
	Qaxis_x(rendercam.cside, tmpQ) ;
	Qaxis_y(rendercam.ctop, tmpQ) ;
	Qaxis_z(rendercam.cview, tmpQ) ;

	rendercam.basev.x= M_dot(rendercam.cpos, rendercam.cside) ;
	rendercam.basev.y= M_dot(rendercam.cpos, rendercam.ctop) ;
	rendercam.basev.z= M_dot(rendercam.cpos, rendercam.cview) ;

	return true ;
}

	///

ymotioncam::ymotioncam(aart_element * acam, unsigned int aindex) : aart_link(acam), index(aindex)
{
	max= 0, range= 0 ;
	list.append(this) ;
}

ymotioncam:: ~ ymotioncam()
{
	if (max && dataptr) free(dataptr) ;
	list.remove(this) ;
	if (list.isempty()) depth= 0 ;
}

	//

void	ymotioncam::setRange(unsigned int amax)
{
	max= amax ;
	dataptr= malloc(amax * 8 * sizeof(float)) ;
	if (amax > depth) depth= amax ;
}

void	ymotioncam::setBlock(unsigned int apoint, float * ablock)
{
	float * fdest ;

	if ((apoint < 0) || (apoint >= max)) return ;
	fdest= ((float *) dataptr) + (8 * apoint) ;
	memcpy(fdest, ablock, 8 * sizeof(float)) ;
	range= apoint +1;
}

void	ymotioncam::getFOV(unsigned int atime, float & fov)
{
	float * fptr ;
	if ((atime < 1) || (atime > range)) return ;
	fptr= ((float *) dataptr) + (8 * atime) ;
	fov= fptr[7] ;
}

void	ymotioncam::getMotion(unsigned int atime, vector & apos, quaternion & adir)
{
	float * fptr ;
	if ((atime < 1) || (atime > range)) return ;
	fptr= ((float *) dataptr) + (8 * atime) ;
	apos.x= fptr[0], apos.y= fptr[1], apos.z= fptr[2] ;
	adir.s= fptr[3], adir.i= fptr[4], adir.j= fptr[5], adir.k= fptr[6] ;
}

unsigned int	ymotioncam::depth = 0 ;
generalbag	ymotioncam::list ;