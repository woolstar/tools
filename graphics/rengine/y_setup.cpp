#include "ye.h"

// #include	<aart_post.h>
#include	"efilter.h"
#include	"y_camera.h"

	// rendering data
extern "C"
{
	int	val_rdepth, val_gdepth,
//		ct_acts, ct_ents,
		ct_ball, ct_deform;
	long	mem_phase1, mem_phase2, mem_phase3;

	color	c_ambient ;
} ;

	// image data
extern "C"
{
	long	*imbuffer= NULL,
			cfreswidth;
	int		pa_width, pa_height, pa_slot, pa_flip;
} ;

static int	imbuffersize= 0 ;

	// idle call back
extern "C"
{
	int	(*rbuild_idle)()= NULL;
} ;

	// fog
extern "C"
{
	int	ct_fog, ct_env0, ct_env1;

	vector	fogmath ;
	color	fogcol ;
} ;

	// global components
extern "C"
{
	er_camera	magic_cam;
	er_smap		*magic_im= NULL;

#if	SPHERE_SUPPORT
	er_smot		*magic_spot;
	er_sform	*magic_deform;
	er_smear	*magic_spheres;
#endif
} ;


extern "C"
{
		// dx_mempage
	extern int	w_tilerimage(long x, long y, long slot, int filter, int adapt) ;
		// dx_post
	extern int	aa_samp, aa_width, aa_stretch, aa_window ;
	extern void	t_setup(unsigned int adapt, unsigned int afilter, unsigned int filterwindow) ;
		// cp_map
	extern int	mapdirection ;
} ;


	////
	// yzor

	// dosetup
	//	install setup parameters
bool	yzor::dosetup()
{
	aart::setup * setupref ;
	const aart_post * postref ;
	int fid ;

	setupref= & (mscene-> setup_data) ;
	postref= aart_post::getpost() ;

		// ambient light
	c_ambient= mnode-> lightref-> ambient ;

		// post data
	val_rdepth= postref-> rdepth ;
	val_gdepth= postref-> tdepth ;

#if ENVIR_SUPPORT
		// legacy
	ct_fog= 0;
	fogmath.z= 1e304;
#endif

	if (postref-> adapt)
	{
			// image filter
		switch (postref-> filter)
		{
			case aart_post::eGauss: fid= FILTER_GAUSS;  break; 
			case aart_post::eSync: fid= FILTER_SINC;  break;
			default:
				fid= FILTER_BOX ;
				break; 
		}

		t_setup(postref-> adapt, fid, postref-> filterwindow) ;
	}
		else t_setup(0, 0, 0) ;

	pa_width= setupref-> x ;
	pa_height= setupref-> y ;

	if (postref-> textureflip)
		mapdirection= 1 ;

#if 0
	long lv, l ;

		// figure out sections for rendering
	lv= pa_width * pa_height ;
		if (aa_samp) lv *= (aa_samp * aa_samp) ;
	for(sl= 0, l= lv; (l > 6291456); ) { sl ++, l= lv / sl; }
#endif

	pa_slot= pa_height ;	// XX
	pa_flip= 0 ;

		// tiler setup
	w_tilerimage(pa_width, pa_height, pa_slot, aa_width, aa_samp);

		// image setup
	if (pa_width > imbuffersize)
	{
		if (imbuffersize) { free(imbuffer);  imbuffer= NULL, imbuffersize= 0; }
		imbuffer= (long *) GM(pa_width, long) ;
		if (NULL == imbuffer)
			return false ;

		imbuffersize= pa_width ;
	}

	cfreswidth= pa_width ;
	imheader.width= pa_width ;
	imheader.height= pa_height ;

	doeffect(postref) ;
	return true ;
}

bool	yzor::setupmotion(void)
{
	aart::camera * tmpcam ;
	aart_node::cameraset * cptr ;
	tbagstepper<aart::camera *> * step ;
	int i, j ;
	FILE * ftmp ;
	ymotioncam	* ycam ;

	cptr= mnode-> cameraref ;
	if (! cptr) cptr= mbase-> cameraref ;

	step= cptr-> getstep() ;
	while (step->next(tmpcam))
	{
		switch (tmpcam-> mimport)
		{
			case aart::camera::eMotion:
				i= atoi(tmpcam-> importsrc) ;
				if (i) new ymotioncam(tmpcam, i) ;
				break ;
			case aart::camera::eFile:
				ftmp= fopen(tmpcam-> importsrc, "r") ;
				if (ftmp)
				{
					char	tmpstr[512] ;
					float	f[8] ;
					i= 0 ;
					while (fgets(tmpstr, sizeof(tmpstr), ftmp)) { i ++ ; }
					if (i)
					{
						ycam= new ymotioncam(tmpcam, 0) ;
						ycam->setRange(i) ;
						rewind(ftmp) ;
						i= 0 ;
						while (fgets(tmpstr, sizeof(tmpstr), ftmp))
						{
							j= sscanf(tmpstr, "%f %f %f %f %f %f %f %f",
								f +0, f +1, f +2, f +3, f +4, f +5, f +6, f +7) ;
							if (j) { ycam->setBlock(i, f) ;  i ++ ; }
						}
					}
					fclose(ftmp) ;
				}
				break ;
		}
	}
	delete step ;
	
	return true ;
}

