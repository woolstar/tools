#include "aar.h"

#include	"aart_core.h"
#include	"angle.h"

	// construct
	//	set camera defaults
aart::camera::camera(const char * astr, aart_component * aparent) : aart_element(astr, aparent)
{
	M_zero(position) ;
	Q_zero(direction) ;

	fov= 30.0 ;
	focus= 100.0 ;
	mimport= eNone ;
}

static void	resolvq(quaternion & adir, const vector * apos, const vector * atarget)
{
	double dtmp, da, db ;
	vector vdelt, vc, vs ;

	M_sub(vdelt, (* apos), (* atarget)) ;
	M_selfnorm(vdelt) ;
	if (dtmp < 1e-5)
		return ;

	da= 0.5 * atan2(vdelt.x, vdelt.z) ;
	db= - 0.5 * asin(vdelt.y) ;

	vc.x= cos(db), vs.x= sin(db) ;
	vc.y= cos(da), vs.y= sin(da) ;
	vc.z= 1.0, vs.z= 0.0 ;

	ZYX2_Q(adir, vc, vs) ;
}

	// parse
bool	aart::camera::parse(const char * astr, FILE *)
{
	unsigned long	lcode= atocode(astr) ;
	int itmp ;
	float ftmp ;
	vector vtmp ;

	switch (lcode)
	{
		case 'POSI':
			if (! parsevec(position, astr))
				return false ;
			break ;
		case 'DIRE':
		case 'QUAT':
			if (! parseq(direction, astr))
				return false ;
			break ;
		case 'TARG':
			if (! parsevec(vtmp, astr))
				return false ;

			resolvq(direction, & position, & vtmp) ;
			break ;

		case 'VFOV':
		case 'FOV':
		case 'VERT':
			if (! parseval(ftmp, astr) || (ftmp < 0.0) || (ftmp > 180.0))
				return false ;
			fov= ftmp ;
			break ;
		case 'FOCU':
			if (! parseval(ftmp, astr) || (ftmp < 1e-3)) return false ;
			focus= ftmp ;
			break ;

		case 'IMPO':	// import
			generalstring::skip(astr) ;
			switch (* astr)
			{
				case 'm':	// motiontrack
					generalstring::skip(astr) ;
					itmp= atoi(astr) ;
					if (astr)
					{
						mimport= eMotion ;
						generalstring::captureKnown(importsrc, astr) ;
					}
					break ;
				case 'f':	// file
					generalstring::skip(astr) ;
					if (* astr)
					{
						mimport= eFile ;
						generalstring::paramKnown(importsrc, astr) ;
					}
					break ;
			}
			break; 

		default:
			if (strncmp(astr, "# ---", 5))
				ignore(astr) ;
	}

	return true ;
}

bool	aart::camera::print(aart_output * adest)
{
	adest-> printf("{camera'%s\n", mname) ;

	adest-> printf("position %g %g %g\n", M_expand(position)) ;
	adest-> printf("quaternion %g %g %g %g  # direction\n", Q_expand(direction)) ;
	adest-> printf("verticalfov %g\n", fov) ;
	adest-> printf("focus %g\n", focus) ;

	switch (mimport)
	{
		case eMotion:
			adest-> printf("import motiontrack %s\n", importsrc) ;
			break ;
		case eFile:
			adest-> printf("import file%s\n", importsrc) ;
			break; 
	}


	if (other.getcount())
		adest-> print("# ---\n") ;
	return aart_component::print(adest) ;
}
