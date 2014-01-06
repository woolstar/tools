#include "ye.h"

	// legacy interfaces
	//	direct render setup functions
extern "C"
{
		// cp map
	extern void	cpmap_start(void) ;
	extern int	cpmap_load(void);
	extern void	cpmap_release(void) ;
	extern void	cpmap_destroy(void) ;

		// dx effect
	extern void teffect_apply(void) ;

		// dx mempage
	extern void	w_tilerclear(void);
	extern void	w_tilerrel(void);

		// dx tile
	extern void	dx_tileinit(void);

		// r data
	extern void rstart_count(void) ;

		// rv light
	extern void	rvm_destroy(void) ;

		// r rand
	extern void	rran_initgen() ;

		//	y geom
	extern bool	tgeom_range(void) ;
	extern int	tgeom_tile(void) ;

		// y setup
	extern long	*imbuffer ;

		// l thread
	extern int	p_active ;

} ;

	extern void	cto_setup(int, const char *) ;


yzor::yzor(aart * ascene)
{
	t= 0 ;
	mscene= ascene ;
	mcamera= NULL ;

	mbase= mscene-> getbase() ;
	mnode= mscene-> getcur() ;

	dx_tileinit() ;
	cpmap_start() ;

	rran_initgen();
	errnotify= true ;
}

yzor:: ~ yzor()
{
	release() ;
	cpmap_destroy() ;
//	GFU(imbuffer) ;

	delete	mscene ;
}

bool	yzor::init()
{
	return dosetup() && setupmotion() ;
}

void	yzor::prep(unsigned int aframe)
{
	const char * tmppath ;

	settime(aframe) ;

	tmppath= NULL ;
	if (mnode && mnode->geometryref && mnode->geometryref->paths.getcount())
		tmppath= mnode->geometryref->paths[0] ;
	else
	if (mbase && mbase-> geometryref && mbase->geometryref->paths.getcount())
		tmppath= mbase->geometryref->paths[0] ;
	cto_setup(aframe, tmppath) ;

	loadgeom() ;
	dogeom() ;
}

bool	yzor::render(unsigned int aframe)
{
	p_active= 1;
	prep(aframe);

	docamera() ;
	dolight() ;

	cpmap_load() ;
	w_tilerclear() ;
	setview() ;
	rstart_count();
	if (tgeom_range())
	{
		tgeom_tile();
		teffect_apply() ;

		save() ;
	}
	else
		generallog::message("nothing visible to render", GL::eWarning) ;

//	rbuild_release();
	cpmap_release();
	w_tilerrel();

		// delete lights
	rvm_destroy() ;
	release() ;

#if 0
	tgeom_dispose();
	cto_dispose();

#endif

	p_active= 0;

	return true ;
}