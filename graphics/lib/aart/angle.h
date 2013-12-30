
#ifndef	QSTRUCT
#	define	QSTRUCT
typedef struct { double s, i, j, k; } quaternion ;

#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern void	q2exyz(quaternion *, vector *) ;
extern void	q2ezyx(quaternion *, vector *) ;
extern void	q2exzy(quaternion *, vector *) ;
extern void	q2eyzx(quaternion *, vector *) ;
extern void	q2eyxz(quaternion *, vector *) ;
extern void	q2ezxy(quaternion *, vector *) ;

#ifdef	__cplusplus
} ;
#endif

	/** matrix creation */
#define	QM_x(mx, q)	mx.x= 1.0 -2*(q.j*q.j + q.k*q.k), \
					mx.y= 2.0*(q.i*q.j - q.s*q.k), \
					mx.z= 2.0*(q.i*q.k + q.s*q.j);
#define QM_y(my, q)	my.x= 2.0*(q.i*q.j + q.s*q.k), \
					my.y= 1.0 -2*(q.i*q.i + q.k*q.k), \
					my.z= 2.0*(q.j*q.k - q.s*q.i);
#define	QM_z(mz, q)	mz.x= 2.0*(q.i*q.k - q.s*q.j), \
					mz.y= 2.0*(q.j*q.k + q.s*q.i), \
					mz.z= 1.0 -2*(q.i*q.i + q.j*q.j);

	/** euler matrix creation */
#define	XYZM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= -c.y*s.z, mx.z= s.y
#define XYZM_y(my, c, s)	my.x= c.x*s.z + s.x*s.y*c.z, my.y= c.x*c.z - s.x*s.y*s.z, my.z= -s.x*c.y
#define XYZM_z(mz, c, s)	mz.x= s.x*s.z - c.x*s.y*c.z, mz.y= s.x*c.z + c.x*s.y*s.z, mz.z= c.x*c.y

#define	ZYXM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= s.x*s.y*c.z - c.x*s.z, mx.z= c.x*s.y*c.z + s.x*s.z
#define	ZYXM_y(my, c, s)	my.x= c.y*s.z, my.y= s.x*s.y*s.z + c.x*c.z, my.z= c.x*s.y*s.z - s.x*c.z
#define	ZYXM_z(mz, c, s)	mz.x= -s.y, mz.y= s.x*c.y, mz.z= c.x*c.y

#define	ZXYM_x(mx, c, s)	mx.x= c.y*c.z - s.x*s.y*s.z, mx.y= - c.x*s.z, mx.z= s.x*c.y*s.z + s.y*c.z
#define	ZXYM_y(my, c, s)	my.x= s.x*s.y*c.z + c.y*s.z, my.y= c.x*c.z, my.z= s.y*s.z - s.x*c.y*c.z
#define	ZXYM_z(mz, c, s)	mz.x= - c.x*s.y, mz.y= s.x, mz.z= c.x*c.y

#define	YXZM_x(mx, c, s)	mx.x= s.x*s.y*s.z + c.y * c.z, mx.y= s.x*s.y*c.z - c.y*s.z, mx.z= c.x*s.y
#define	YXZM_y(my, c, s)	my.x= c.x*s.z, my.y= c.x*c.z, my.z= - s.x
#define	YXZM_z(mz, c, s)	mz.x= s.x*c.y*s.z - s.y*c.z, mz.y= s.x*c.y*c.z + s.y*s.z, mz.z= c.x*c.y

#define	XZYM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= -s.z, mx.z= s.y*c.z
#define	XZYM_y(my, c, s)	my.x= s.x*s.y + c.x*c.y*s.z, my.y= c.x*c.z, my.z= c.x*s.y*s.z - s.x*c.y
#define	XZYM_z(mz, c, s)	mz.x= s.x*c.y*s.z - c.x*s.y, mz.y= s.x*c.z, mz.z= s.x*s.y*s.z + c.x*c.y

#define	YZXM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= s.x*s.y - c.x*c.y*s.z, mx.z= s.x*c.y*s.z + c.x*s.y
#define	YZXM_y(my, c, s)	my.x= s.z, my.y= c.x*c.z, my.z= - s.x*c.z
#define	YZXM_z(mz, c, s)	mz.x= - s.y*c.z, mz.y= c.x*s.y*s.z + s.x*c.y, mz.z= c.x*c.y - s.x*s.y*s.z


	/** euler q creation */
#define	XYZ2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z - se.x * se.y * se.z, \
						q.i= ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z - se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z + se.x * se.y * ce.z

#define	ZYX2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z + se.x * se.y * se.z, \
						q.i= - ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z + se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z - se.x * se.y * ce.z

#define	ZXY2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z - se.x * se.y * se.z, \
						q.i= - ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z + se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z + se.x * se.y * ce.z

#define	YXZ2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z + se.x * se.y * se.z, \
						q.i= ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z - se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z - se.x * se.y * ce.z

#define	YZX2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z - se.x * se.y * se.z, \
						q.i= ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z + se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z - se.x * se.y * ce.z

#define	XZY2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z + se.x * se.y * se.z, \
						q.i= - ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= ce.x * se.y * ce.z - se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z + se.x * se.y * ce.z

