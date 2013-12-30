
#include <general.h>
#include <math.h>

typedef struct { double x, y, z; } vector ;

#include "angle.h"

	/** matrix creation */

#define	QM_ii(q)	(1.0 -2*(q.j*q.j + q.k*q.k))
#define	QM_ji(q)	2.0*(q.i*q.j - q.s*q.k)
#define	QM_ki(q)	2.0*(q.i*q.k + q.s*q.j)
#define	QM_ij(q)	2.0*(q.i*q.j + q.s*q.k)
#define	QM_jj(q)	(1.0 -2*(q.i*q.i + q.k*q.k))
#define	QM_kj(q)	2.0*(q.j*q.k - q.s*q.i)
#define	QM_ik(q)	2.0*(q.i*q.k - q.s*q.j)
#define	QM_jk(q)	2.0*(q.j*q.k + q.s*q.i)
#define	QM_kk(q)	(1.0 -2*(q.i*q.i + q.j*q.j))


void	q2exyz(quaternion * qsrc, vector * aresult)
{
	double	sy, cy, smx, cmx, smz, cmz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sy= QM_ki(qt) ;
	if ((sy > 0.99999) || (sy < -0.99999))
	{
		cy= 0 ;

			/** gimbal locked, assign all rotation to X */
		smx= - QM_jk(qt), cmx= QM_jj(qt) ;
		smz= 0, cmz= 1 ;
	}
	else
	{
		cy= sqrt(1.0 - sy * sy) ;

			/** extract Z & X */
		cmz= QM_ii(qt), smz= - QM_ji(qt) ;
		cmx= QM_kk(qt), smx= - QM_kj(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(smx, cmx) ;
	aresult-> y= k* atan2(sy, cy) ;
	aresult-> z= k* atan2(smz, cmz) ;
}

void	q2ezyx(quaternion * qsrc, vector * aresult)
{
	double	sy, cy, smx, cmx, smz, cmz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sy= - QM_ik(qt) ;
	if ((sy > 0.99999) || (sy < -0.99999))
	{
		cy= 0 ;

			/** gimbal locked, assign all rotation to X */
		smx= - QM_kj(qt), cmx= QM_jj(qt) ;
		smz= 0, cmz= 1 ;
	}
	else
	{
		cy= sqrt(1.0 - sy * sy) ;

			/** extract Z & X */
		cmz= QM_ik(qt), smz= - QM_jk(qt);
		cmx= QM_kk(qt), smx= QM_jk(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(smx, cmx) ;
	aresult-> y= k* atan2(sy, cy) ;
	aresult-> z= k* atan2(smz, cmz) ;
}

void	q2ezxy(quaternion * qsrc, vector * aresult)
{
	double	sx, cx, smy, cmy, smz, cmz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sx= QM_jk(qt) ;
	if ((sx > 0.99999) || (sx < -0.99999))
	{
		cx= 0 ;

			/** gimbal locked, assign all rotation to Y */
		smy= - QM_ki(qt), cmy= QM_ii(qt) ;
		smz= 0, cmz= 1;
	}
	else
	{
		cx= sqrt(1.0 - sx * sx) ;

			/** extract Y & Z rotation */
		cmy= QM_kk(qt), smy= - QM_ik(qt) ;
		cmz= QM_jj(qt), smz= - QM_ji(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(sx, cx) ;
	aresult-> y= k* atan2(smy, cmy) ;
	aresult-> z= k* atan2(smz, cmz) ;
}

void	q2eyxz(quaternion * qsrc, vector * aresult)
{
	double	sx, cx, smy, cmy, smz, cmz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sx= - QM_kj(qt) ;
	if ((sx > 0.99999) || (sx < -0.99999))
	{
		cx= 0 ;

			/** gimbal locked, assign all rotation to Y */
		smy= - QM_ik(qt), cmy= QM_ii(qt) ;
		smz= 0, cmz= 1;
	}
	else
	{
		cx= sqrt(1.0 - sx * sx) ;

			/** extract Y & Z rotation */
		cmy= QM_kk(qt), smy= QM_ki(qt) ;
		cmz= QM_jj(qt), smz= QM_ij(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(sx, cx) ;
	aresult-> y= k* atan2(smy, cmy) ;
	aresult-> z= k* atan2(smz, cmz) ;
}

void	q2exzy(quaternion * qsrc, vector * aresult)
{
	double	smx, cmx, smy, cmy, sz, cz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sz= - QM_ji(qt) ;
	if ((sz > 0.99999) || (sz < -0.99999))
	{
		cz= 0 ;

			/** gimbal locked, assign all rotation to Y */
		smy= - QM_ik(qt), cmy= QM_kk(qt) ;
		smx= 0, cmx= 1;
	}
	else
	{
		cz= sqrt(1.0 - sz * sz) ;

			/** extract Y & X rotation */
		cmy= QM_ii(qt), smy= QM_ki(qt) ;
		cmx= QM_jj(qt), smx= QM_jk(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(smx, cmx) ;
	aresult-> y= k* atan2(smy, cmy) ;
	aresult-> z= k* atan2(sz, cz) ;
}

void	q2eyzx(quaternion * qsrc, vector * aresult)
{
	double	smx, cmx, smy, cmy, sz, cz, k ;
	quaternion qt ;

	if ((NULL == qsrc) || (NULL == aresult))
		return ;
	qt= * qsrc ;

	sz= QM_ij(qt) ;
	if ((sz > 0.99999) || (sz < -0.99999))
	{
		cz= 0 ;

			/** gimbal locked, assign all rotation to Y */
		smy= QM_ki(qt), cmy= QM_kk(qt) ;
		smx= 0, cmx= 1;
	}
	else
	{
		cz= sqrt(1.0 - sz * sz) ;

			/** extract Y & X rotation */
		cmy= QM_ii(qt), smy= - QM_ik(qt) ;
		cmx= QM_jj(qt), smx= - QM_kj(qt) ;
	}

	k= 180.0 / M_PI ;
	aresult-> x= k* atan2(smx, cmx) ;
	aresult-> y= k* atan2(smy, cmy) ;
	aresult-> z= k* atan2(sz, cz) ;
}

