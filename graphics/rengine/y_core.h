#ifndef YCORE_DEF
#	define	YCORE_DEF	1

#include	<aart.h>
#include	<aart_post.h>
#include	<bpic.h>
#include	<tfren.h>

class	yzor
{
	public:
		yzor(aart *) ;
		~ yzor() ;

		void	settime(unsigned int atime) { t= atime; }

	public:
		bool	init(void) ;

		bool	dosetup(void) ;
		bool	doeffect(const aart_post *) ;
		bool	setupmotion(void) ;

		bool	docamera(void) ;
		void	setview(void) ;
		bool	docamera(aart::camera *) ;
		bool	dolight(void) ;
		bool	dogeom(void) ;

		void	prep(unsigned int) ;
		bool	render(unsigned int) ;

		bool	save() ;
		void	release() ;

		aart_node	* getnode(void) { return mnode; }

	protected:
		unsigned int	t ;

		aart	* mscene ;
		aart_node	* mnode, * mbase ;

		aart::camera	* mcamera ;

			// image
		ehdr_image	imheader ;

		bool	errnotify ;

	protected:
		bool	loadgeom(void) const ;
		bool	gendata(tr_object * adest, const aart::geometry *) const ;
		bool	loaddata(tr_object * adest, const char *) const ;

		bool	setupsurf(tr_object *) const ;
} ;

#endif	// YCORE_DEF