#include "ye.h"
#include	"ymath.h"

ymath::ymath()
{
	init() ;
}

	/// interface
void	yartmath::load(aart::geometry * ageom)
{
	init() ;

	scale(ageom-> scale) ;
	offset(ageom-> position) ;
}

	/// apply

void	ymath::apply(vector * adest, unsigned long acount) const
{
	int	istep ;
	vector	vwork ;

	for (istep= acount; (istep --); adest ++)
	{
		MpP_cpy(vwork, adest) ;
		adest-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2] + ma[0][3];
		adest-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2] + ma[1][3];
		adest-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2] + ma[2][3];
	}
}

void	ymath::apply(vector * adest, const vector * asrc, unsigned long acount) const
{
	int	istep ;
	vector	vwork ;

	for (istep= acount; (istep --); asrc ++, adest ++)
	{
		MpP_cpy(vwork, asrc) ;
		adest-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2] + ma[0][3];
		adest-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2] + ma[1][3];
		adest-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2] + ma[2][3];
	}
}

void	ymath::apply(svector * adest, unsigned long acount) const
{
	int	istep ;
	svector	vwork ;

	for (istep= acount; (istep --); adest ++)
	{
		MpP_cpy(vwork, adest) ;
		adest-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2] + ma[0][3];
		adest-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2] + ma[1][3];
		adest-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2] + ma[2][3];
	}
}

void	ymath::apply(svector * adest, const svector * asrc, unsigned long acount) const
{
	int	istep ;
	svector	vwork ;

	for (istep= acount; (istep --); asrc ++, adest ++)
	{
		MpP_cpy(vwork, asrc) ;
		adest-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2] + ma[0][3];
		adest-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2] + ma[1][3];
		adest-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2] + ma[2][3];
	}
}

void	ymath::applyn(svector * adest, unsigned long acount) const
{
	int	istep ;
	svector	vwork ;

	for (istep= acount; (istep --); adest ++)
	{
		MpP_cpy(vwork, adest) ;
		adest-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2];
		adest-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2];
		adest-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2];
	}
}

	/// internal

#define	MM_MINOR(ax,ay,bx,by)	(ma[ax][ay] * ma[bx][by] - ma[bx][ay] * ma[ax][by])

bool	ymath::invert()
{
	unsigned int	i, j ;
	double	su ;

	MM_init(m);

	m[0][0]= MM_MINOR(1,1,2,2),
	m[0][1]= -MM_MINOR(0,1,2,2),
	m[0][2]= MM_MINOR(0,1,1,2),
	m[1][0]= -MM_MINOR(1,0,2,2),
	m[1][1]= MM_MINOR(0,0,2,2),
	m[1][2]= -MM_MINOR(0,0,1,2),
	m[2][0]= MM_MINOR(1,0,2,1),
	m[2][1]= -MM_MINOR(0,0,2,1),
	m[2][2]= MM_MINOR(0,0,1,1);

	for (i= 3, su= 0.0; i--; ) su+= ma[i][0] * m[0][i];
	if (su == 0.0) { return false; }
	su= 1.0 / su;

	for (i= 3; i--; ) for (j= 3; j--; ) m[i][j] *= su;

		/** better fixed offset calc hack (r3) */
	m[0][3]= -(m[0][0] * ma[0][3] + m[0][1] * ma[1][3] + m[0][2] * ma[2][3]),
	m[1][3]= -(m[1][0] * ma[0][3] + m[1][1] * ma[1][3] + m[1][2] * ma[2][3]),
	m[2][3]= -(m[2][0] * ma[0][3] + m[2][1] * ma[1][3] + m[2][2] * ma[2][3]);

	MM_cpy(ma, m) ;

	return true ;
}

	// init
	//	set matrix to identiry
void	ymath::init()
{
	unsigned short	i, j ;
	MM_init(ma) ;
	MM_ident(ma) ;
}

	// mult
	//	combine ma & m
void	ymath::mult(void)
{
	unsigned int	i, j, k ;
	double su, mt[4][4] ;

   for (i= 4; i--; ) for (j= 4; j--; )
      { for (k= 4, su= 0.0; k--; ) { su+= m[k][j] * ma[i][k]; }  mt[i][j]= su; }
   MM_cpy(ma, mt) ;
}

void	ymath::rotx(double angle)
{
	unsigned short i, j;
	double c, s ;

	MM_init(m)  MM_ident(m) ;
	c= cos(angle), s= sin(angle) ;
	m[1][1]= c, m[2][2]= c;
	m[2][1]= s, m[1][2]= -s;
	mult() ;
}

void	ymath::roty(double angle)
{
	unsigned short i, j;
	double c, s ;

	MM_init(m)  MM_ident(m) ;
	c= cos(angle), s= sin(angle) ;
	m[0][0]= c, m[2][2]= c;
	m[2][0]= -s, m[0][2]= s;
	mult() ;
}

void	ymath::rotz(double angle)
{
	unsigned short i, j;
	double c, s ;

	MM_init(m)  MM_ident(m) ;
	c= cos(angle), s= sin(angle) ;
	m[0][0]= c, m[1][1]= c;
	m[1][0]= s, m[0][1]= -s;
	mult() ;
}

void	ymath::rotq(quaternion &q)
{
	unsigned short	i, j ;
	vector	vtmp ;

	MM_init(m)

	QM_x(vtmp, q);  m[0][0]= vtmp.x, m[0][1]= vtmp.y, m[0][2]= vtmp.z ;
	QM_y(vtmp, q);  m[1][0]= vtmp.x, m[1][1]= vtmp.y, m[1][2]= vtmp.z ;
	QM_z(vtmp, q);  m[2][0]= vtmp.x, m[2][1]= vtmp.y, m[2][2]= vtmp.z ;
	m[3][3]= 1.0;

	mult() ;
}

void	ymath::scale(vector & ascale)
{
	unsigned short	i, j ;

	MM_init(m) ;
	m[0][0]= ascale.x, m[1][1]= ascale.y, m[2][2]= ascale.z, m[3][3]= 1.0 ;
	mult() ;
}

void	ymath::offset(vector & aposition)
{
	unsigned short i, j;

	MM_init(m)  MM_ident(m) ;
	m[0][3]= aposition.x, m[1][3]= aposition.y, m[2][3]= aposition.z;
	mult() ;
}
