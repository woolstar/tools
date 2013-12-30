#include "aar.h"

#include	"aart_core.h"

aart::setup::setup()
{
	x= 320, y= 240 ;
	aspect_x= 0, aspect_y= 0 ;

	screen_aspect= (float) (4.0 / 3.), pixel_aspect= 1.0 ;

	background= eNone ;
	C_zero(bc) ;
	* bimage= '\0' ;
}

bool	aart::setup::parse(const char * astr, FILE *)
{
	unsigned long	lcode= atocode(astr) ;
	int ia, ib ;

	switch (lcode)
	{
		case 'SIZE':
			if (! aart_element::parsepoint(ia, ib, astr) || (ia < 1) || (ib < 1))
				return false ;
			x= ia, y= ib ;
			break ;
		case 'ASPE':
			if (! aart_element::parsepoint(ia, ib, astr) || (ia < 1) || (ib < 1))
				return false ;
			aspect_x= ia, aspect_y= ib ;
			break ;

		case 'BACK':
			generalstring::skip_word(astr) ;
			lcode= atocode(astr) ;
			if ('COLO' == lcode)
			{
				if (! aart_element::parsecolor(bc, astr))
					return false ;
				background= eColor ;
			}
			else
			if ('IMAG' == lcode)
			{
				generalstring::skip_word(astr) ;
				if (* astr)
				{
					generalstring::paramK(bimage, astr) ;
					background= eImage ;
				}
					else return false ;
			}
			else
				return false ;

			break ;

		default:
			ignore(astr) ;
	}

	return true ;
}

bool	aart::setup::print(aart_output * adest)
{
	adest-> print("{setup\n") ;

	adest-> printf("size %d %d\n", x, y) ;
	if (aspect_x && aspect_y)
		adest-> printf("aspect %d %d\n", aspect_x, aspect_y) ;

	switch (background)
	{
		case eColor:
			adest-> printf("background color %g %g %g\n", C_expandl(bc)) ;
			break ;
		case eImage:
			adest-> printf("background image %s\n", bimage) ;
			break ;
	}

	if (other.getcount())
		adest-> print("# ---\n") ;
	return aart_component::print(adest) ;
}

	// leavescope
	//	called at completion of parsing
	//	time to work out attribute linking
void	aart::setup::leavescope()
{
	screen_aspect= (float) (1.0 * x / y) ;
	if (x && y && aspect_x && aspect_y)
		pixel_aspect= (float) (1.0 * (aspect_x * y) / (aspect_y * x)) ;
}
