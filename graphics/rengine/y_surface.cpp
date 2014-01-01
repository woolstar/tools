#include "ye.h"
#include	"y_surf.h"

#define	ZM(aa, bb)	aa * sizeof(bb)

bool	yzor::setupsurf(tr_object * atarget) const
{
	int	istep, icount,
		surfcount, mapcount, projcount ;
	ev_surfbsi * layerbase, * layerfill ;
	er_surface * surfbase, * surfstep ;
	ev_mapdat
		*edbase,
		*edstep;
	ev_mapping 
		*embase,
		*emuse;
	tro_surface * tsstep ;
	bool	snotify= errnotify ;

	aart::surface * surfptr ;

	icount= atarget-> ta.surface_c ;
	layerbase= GM(icount, ev_surfbsi) ;
	if (NULL == layerbase)
		return false ;

		// init basis
	memset(layerbase, 0, icount * sizeof(ev_surfbsi)) ;
	for (istep= icount, layerfill= layerbase; (istep --); layerfill ++)
	{
		layerfill-> global_v= atarget-> ta.vlist -1 ;
		layerfill-> global_n= atarget-> ta.nlist -1 ;
	}

		// determine memory usage
	surfcount= 0, mapcount= 0, projcount= 0;
	for (istep= icount, tsstep= atarget-> ta.slist; (istep --); surfcount ++, tsstep ++)
	{
		surfptr= mnode-> surfaceref-> retrieve(tsstep-> name) ;
		if (surfptr && surfptr-> hastexture())
		{
				// extra surface for mixing
			surfcount ++ ;
				// n* project & n maps
			projcount += surfptr-> texturecount() ,
			mapcount += surfptr-> texturecount() ;
		}
	}

	surfbase= GM(surfcount, er_surface) ;
	if (NULL == surfbase)
	{
		free(layerbase) ;
		return false ;
	}

	if (projcount)
	{
		edbase= GM(projcount, ev_mapdat) ;
		embase= GM(mapcount, ev_mapping) ;

		if ((NULL == edbase) || (NULL == embase))
		{
			free(layerbase) ;
			free(surfbase) ;
			return false ;
		}
	}
		else edbase= NULL, embase= NULL ;

	memset(surfbase, 0, ZM(icount, er_surface)) ;
	for (istep= icount, surfstep= surfbase, edstep= edbase, emuse= embase, layerfill= layerbase, tsstep= atarget-> ta.slist; (istep --); tsstep ++, surfstep ++, layerfill ++)
	{
		layerfill-> surfp= surfstep ;
		layerfill-> surfmix= NULL ;

		surfptr= mnode-> surfaceref-> retrieve(tsstep-> name) ;
		if (surfptr)
		{
			applysurf(surfstep, surfptr, layerfill) ;
			if (surfptr-> hastexture())
			{
				applytexturing(layerfill, surfptr-> getstep(), edstep, emuse) ;
				surfstep ++ ;
			}
		}
		else
		{
			if (strcmp(tsstep-> name, "none") && strcmp(tsstep-> name, "desk") && snotify)
			{
				char msg[256] ;

				aart::geometry * tmpelem= (aart::geometry *) atarget-> matchref ;
				sprintf(msg, "unable to locate surface %s for actor %s", tsstep-> name, tmpelem-> getname()) ;
				generallog::message(msg, GL::eWarning) ;
				((yzor *) this)-> errnotify= false ;
			}

			applybase(surfstep, tsstep-> base) ;
		}
	}

	memset(& (atarget->ta.sdata), 0, sizeof(atarget-> ta.sdata)) ;
	atarget-> ta.sdata.ersurf= surfbase ;
	atarget-> ta.sdata.erbasis= layerbase ;
	atarget-> ta.sdata.ermap= embase,
	atarget-> ta.sdata.ermapref= edbase ;

	return true ;
}
