   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_VEK
#	define	TFBASIC_VEK	1

typedef struct	{ float x, y, z; } svector;
typedef struct	{ double x, y, z; } vector;
typedef struct	{ double x, y; } flatU;

#define	vekS	svector
#define	vekU	vector

#define M_cpy(A,B) 	A.x= B.x, A.y= B.y, A.z= B.z;
#define M_cip(A,B) 	A r= B r, A g= B g, A b= B b;
#define MPP_cpy(A,B) 	A->x= B->x, A->y= B->y, A->z= B->z;
#define MPp_cpy(A,B) 	A->x= B.x, A->y= B.y, A->z= B.z;
#define MpP_cpy(A,B) 	A.x= B->x, A.y= B->y, A.z= B->z;
#define MPc_cpy(A,B) 	A->x= B.r, A->y= B.g, A->z= B.b;
#define Mpc_cpy(A,B) 	A.x= B.r, A.y= B.g, A.z= B.g;
#define MCc_cpy(A,B) 	A->r= B.r, A->g= B.g, A->b= B.b;

#define	M_zero(A)	A.x= 0.0, A.y= 0.0, A.z= 0.0;
#define M_plus(A,B)	A.x+= B.x,  A.y+= B.y,  A.z+= B.z;
#define M_minus(A,B)	A.x-= B.x,  A.y-= B.y,  A.z-= B.z;
#define M_times(A,B)	A.x*= B.x,  A.y*= B.y,  A.z*= B.z;
#define M_add(N, A,B)	N.x= A.x +B.x, N.y= A.y +B.y, N.z= A.z +B.z;
#define M_sub(N, A,B)	N.x= A.x -B.x, N.y= A.y -B.y, N.z= A.z -B.z;
#define M_scale(A, S)	A.x*= S,  A.y*= S,  A.z*= S;
#define M_neg(A)		A.x= -A.x, A.y= -A.y, A.z= -A.z;
#define M_cross(N, A,B) N.x= (A.y *B.z) -(A.z *B.y), \
			N.y= (A.z *B.x) -(A.x *B.z), \
			N.z= (A.x *B.y) -(A.y *B.x);
#define M_ptcross(N, A,B,C) N.x= ((A.y-B.y) *(C.z-B.z)) -((A.z-B.z) *(C.y-B.y)), \
			N.y= ((A.z-B.z) *(C.x-B.x)) -((A.x-B.x) *(C.z-B.z)), \
			N.z= ((A.x-B.x) *(C.y-B.y)) -((A.y-B.y) *(C.x-B.x));

#define	FPp_add(N, A,B)	N.x= A->x +B.x, N.y= A->y +B.y;

#define M_expand(A)	A.x, A.y, A.z
#define M_load(A, S)	A.x= A.y= A.z= S;
#define M_dot(A,B)	((A.x *B.x) + (A.y *B.y) +(A.z *B.z))
#define M_selfdot(A)	((A.x *A.x) + (A.y *A.y) +(A.z *A.z))
#define M_len(A,B)	sqr(A.x-B.x) +sqr(A.y-B.y) +sqr(A.z-B.z)
#define M_selfnorm(A)	dtmp= sqrt(M_selfdot(A)); if (dtmp) {dtmp= 1.0/dtmp; M_scale(A, dtmp)}
#define M_average(N,A,B)	N.x= 0.5* (A.x +B.x), N.y= 0.5* (A.y +B.y), N.z= 0.5* (A.z +B.z);
#define MM_init(A)	for (i=4; i--; ) for (j=4; j--; ) A[i][j]= 0.0;
#define MM_ident(A)	for (i=4; i--; ) A[i][i]= 1.0;
#define MM_cpy(A,B)	for (i=4; i--; ) for (j=4; j--; ) A[i][j]= B[i][j];

#endif

