   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_Q
#	define	TFBASIC_Q	1


#ifndef	QSTRUCT
#	define	QSTRUCT
typedef struct	{ float s, i, j, k; } squaternion ;
typedef struct	{ double s, i, j, k; } quaternion;
#endif



#define	Quse	double dq1, dq2;

#define Qu_padd(Q,E,F)	Q.i+=E.i+F.i, Q.j+=E.j+F.j, Q.k+=E.k+F.k
#define	Qu_dot(G,H)	(G.i*H.i + G.j*H.j + G.k*H.k)
#define Qu_cross(N,A,B)	N.i= (A.j *B.k) -(A.k *B.j), \
			N.j= (A.k *B.i) -(A.i *B.k), \
			N.k= (A.i *B.j) -(A.j *B.i)

#define	Q_zero(QQQ)	QQQ.s= 1.0, QQQ.i= QQQ.j= QQQ.k= 0.0;
#define Q_neg(A)	A.s=-A.s, A.i=-A.i, A.j=-A.j, A.k=-A.k;
#define Q_minus(A,B)	A.s= -B.s, A.i=B.i, A.j=B.j, A.k=B.k;
#define	Q_add(N, A,B)	N.s= A.s*B.s - Qu_dot(A,B), \
			N.i= (A.j *B.k) -(A.k *B.j) +(A.s *B.i) +(B.s *A.i), \
			N.j= (A.k *B.i) -(A.i *B.k) +(A.s *B.j) +(B.s *A.j), \
			N.k= (A.i *B.j) -(A.j *B.i) +(A.s *B.k) +(B.s *A.k);
#define	Q_sub(N, A,B)	N.s= -A.s*B.s - Qu_dot(A,B), \
			N.i= (A.j *B.k) -(A.k *B.j) +(A.s *B.i) -(B.s *A.i), \
			N.j= (A.k *B.i) -(A.i *B.k) +(A.s *B.j) -(B.s *A.j), \
			N.k= (A.i *B.j) -(A.j *B.i) +(A.s *B.k) -(B.s *A.k);

#define	Q_expand(QQ)	QQ.s, QQ.i, QQ.j, QQ.k

#define Q_selfdot(A)	A.s*A.s+Qu_dot(A,A)
#define	Q_norm(A)	dtmp= Q_selfdot(A); if(dtmp<1e-10) Q_zero(A) else \
			{dq1= 1.0/sqrt(dtmp); A.s*=dq1,A.i*=dq1,A.j*=dq1,A.k*=dq1; }

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

#define	QtM_x(mx, q)	mx.x= 1.0 -2*(q.j*q.j + q.k*q.k), \
						mx.y= 2.0*(q.i*q.j + q.s*q.k), \
						mx.z= 2.0*(q.i*q.k - q.s*q.j);
#define QtM_y(my, q)	my.x= 2.0*(q.i*q.j - q.s*q.k), \
						my.y= 1.0 -2*(q.i*q.i + q.k*q.k), \
						my.z= 2.0*(q.j*q.k + q.s*q.i);
#define QtM_z(mz, q)	mz.x= 2.0*(q.i*q.k + q.s*q.j), \
						mz.y= 2.0*(q.j*q.k - q.s*q.i), \
						mz.z= 1.0 -2*(q.i*q.i + q.j*q.j);
#define	Qaxis_x	QtM_x
#define	Qaxis_y	QtM_y
#define	Qaxis_z	QtM_z

#if 0
	/** euler matrix creation */
#define	XYZM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= -c.y*s.z, mx.z= s.y;
#define XYZM_y(my, c, s)	my.x= c.x*s.z + s.x*s.y*c.z, my.y= c.x*c.z - s.x*s.y*s.z, my.z= -s.x*c.y;
#define XYZM_z(mz, c, s)	mz.x= s.x*s.z - c.x*s.y*c.z, mz.y= s.x*c.z + c.x*s.y*s.z, mz.z= c.x*c.y;

#define ZXYM_x(mx, c, s)	mx.x= c.y*c.z - s.x*s.y*s.z, mx.y= - c.x*s.z, mx.z= s.x*c.y*s.z + s.y*c.z
#define ZXYM_y(my, c, s)	my.x= s.x*s.y*c.z + c.y*s.z, my.y= c.x*c.z, my.z= s.y*s.z -s.x*c.y*c.z
#define ZXYM_z(mz, c, s)	mz.x= - c.x*s.y, mz.y= s.x, mz.z= c.x*c.y

#define	ZYXM_x(mx, c, s)	mx.x= c.y*c.z, mx.y= s.x*s.y*c.z - c.x*s.z, mx.z= c.x*s.y*c.z + s.x*s.z;
#define	ZYXM_y(my, c, s)	my.x= c.y*s.z, my.y= s.x*s.y*s.z + c.x*c.z, my.z= c.x*s.y*s.z - s.x*c.z;
#define	ZYXM_z(mz, c, s)	mz.x= -s.y, mz.y= s.x*c.y, mz.z= c.x*c.y;

	/** euler q creation */
#define	XYZ2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z - se.x * se.y * se.z, \
						q.i= ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= - se.x * ce.y * se.z + ce.x * se.y * ce.z, \
						q.k= se.x * se.y * ce.z + ce.x * ce.y * se.z

#define ZXY2_Q(q, ce, se) q.s= ce.x * ce.y * ce.z + se.x * se.y * se.z, \
						q.i= ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= - ce.x * se.y * ce.z + se.x * ce.y * se.z, \
						q.k= ce.x * ce.y * se.z - se.x * se.y * ce.z

#define	ZYX2_Q(q, ce, se)	q.s= ce.x * ce.y * ce.z + se.x * se.y * se.z, \
						q.i= - ce.x * se.y * se.z + se.x * ce.y * ce.z, \
						q.j= se.x * ce.y * se.z + ce.x * se.y * ce.z, \
						q.k= - se.x * se.y * ce.z + ce.x * ce.y * se.z

#endif

#endif
