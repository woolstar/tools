#include "rrun.h"

#include	"effect_op.h"

extern "C"
{
	void teffect_apply(void) ;
} ;

void	teffect_apply(void)
{
	effect_op::process() ;
}