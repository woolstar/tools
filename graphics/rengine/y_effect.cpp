#include "ye.h"

#include	<effect_op.h>
#include	<effect_toon.h>

bool	yzor::doeffect(const aart_post * apost)
{
	aart_post::effect * tmpeffect ;
	aart::setup * setupref ;

	tbagstepper<aart_post::effect *> * step ;

	step= apost-> getstep() ;

	while (step-> next(tmpeffect))
	{
		const char * atype= tmpeffect-> gettype() ;

		switch (* atype)
		{
#if 0
			case 't':
				if (! strcmp(atype, "toon"))
				{
					aart_post::toon * arttoon= (aart_post::toon *) tmpeffect ;
					toon_effect * posttoon ;
					int	istep, ibit ;
					color * cstep ;

					setupref= & (mscene-> setup_data) ;
					posttoon= new toon_effect(setupref-> x, setupref-> y, setupref-> screen_aspect) ;

					posttoon-> set("Ch", arttoon-> cline_h) ;
					posttoon-> set("Ct", arttoon-> cline_t) ;
					posttoon-> set("Cw", arttoon-> cline_w) ;
					posttoon-> set("Th", arttoon-> tline_h) ;
					posttoon-> set("Tt", arttoon-> tline_t) ;
					posttoon-> set("Tw", arttoon-> tline_w) ;

					for (istep= 16, cstep= arttoon-> line, ibit= 1; (istep --); cstep ++, ibit *= 2)
					{
						if (ibit & arttoon-> luse)
						{
							char tmpparam[64] ;
							sprintf(tmpparam, "[%d] %g %g %g", 16 - istep, C_expandl((* cstep))) ;
							posttoon-> set("Line", tmpparam) ;
						}
					}
				}
#endif
		}
	}

	delete step ;
	return true ;
}
