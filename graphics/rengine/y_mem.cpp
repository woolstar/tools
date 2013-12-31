#include	"ye.h"

extern "C"
{
		// dx mempage
	extern void	w_tilepadrel() ;

		// tu grid
	extern void	tgrid_destroy(void) ;

		// y geom
	extern long		cobjects;
	extern tr_object	* tolist ;

	extern tro_side	*rbase_sides ;
} ;

void	yzor::release()
{
	int	istep ;
	tr_object	* tstep;

		// delete ray data
	GFU(rbase_sides) ;
	tgrid_destroy();
	w_tilepadrel() ;

	if (cobjects)
	{
		for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
		{
				// delete surface
			if (tstep-> ta.surface_c)
			{
				GFU(tstep-> ta.sdata.ermap);
				GFU(tstep-> ta.sdata.ermapref);
				GF(tstep-> ta.sdata.ersurf);
				GF(tstep-> ta.sdata.erbasis);

				GF(tstep-> ta.slist) ;
			}
				// delete geom
			GFU(tstep-> ta.vlist) ;
			GFU(tstep-> ta.ilist) ;
			GFU(tstep-> ta.flist) ;
		}

		GF(tolist) ;

			// delete objects

		cobjects= 0 ;
	}
}