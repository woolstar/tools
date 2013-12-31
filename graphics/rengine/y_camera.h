#ifndef	YCAMERA_DEF
#	define	YCAMERA_DEF	1

#include <aart_link.h>

class	ymotioncam : public aart_link
{
	public:
		ymotioncam(aart_element *, unsigned int = 0) ;
		~ ymotioncam() ;

		void	setRange(unsigned int) ;
		void	setBlock(unsigned int, float *) ;

		void	getFOV(unsigned int, float &) ;
		void	getMotion(unsigned int, vector &, quaternion &) ;

		unsigned int	index ;
		static unsigned int		depth ;

		static bool	hasmotion(void) { return ! list.isempty() ; }
		static tbagstepper<ymotioncam *> *	getstep(void) { return new tbagstepper<ymotioncam *>(& list) ; }

	protected:
		unsigned int	max, range ;

		static generalbag list ;
} ;

#endif	// YCAMERA_DEF