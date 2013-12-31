#ifndef	YMATH_DEF
#	define	YMATH_DEF	1

#	include <aart.h>

class	ymath
{
	public:
		ymath() ;

		void	apply(vector *, unsigned long) const ;
		void	apply(vector *, const vector *, unsigned long) const ;
		void	apply(svector *, unsigned long) const ;
		void	apply(svector *, const svector *, unsigned long) const ;

		void	applyn(svector *, unsigned long) const ;

		bool	invert(void) ;

	protected:
		void	init(void) ;
		void	mult(void) ;

		void	rotx(double) ;
		void	roty(double) ;
		void	rotz(double) ;
		void	rotq(quaternion &) ;

		void	scale(vector &) ;
		void	offset(vector &) ;

	private:
		double	m[4][4], ma[4][4] ;
} ;

class	yartmath : public ymath
{
	public:
		void	load(aart::geometry *) ;
} ;


#endif	// YMATH_DEF