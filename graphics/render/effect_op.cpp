#include "trun.h"
#include "rqdata.h"
#include	"effect_op.h"

effect_op::effect_op()
{
	geffect.append(this) ;
}

void	effect_op::process()
{
	effect_op	* effptr ;
	tbagstepper<effect_op *>	step(& geffect) ;

	while (step.next(effptr)) { effptr->op(); }
}

void	effect_op::size()
{
	effect_op	* effptr ;
	tbagstepper<effect_op *>	step(& geffect) ;

	while (step.next(effptr)) { effptr->setsize(workset.width, workset.height); }
}

generalbag	effect_op::geffect ;