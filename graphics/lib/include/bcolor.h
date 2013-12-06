   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_COLOR
#	define	TFBASIC_COLOR	1

#ifdef NAMED_COLOR
typedef struct	{ float r, g, b; } TFcolor;
typedef struct	{ double r, g, b; } TFlcolor;

#else
typedef struct	{ float r, g, b; } color;
typedef struct	{ float r, g, b; } scolor;
typedef struct	{ double r, g, b; } lcolor;

#define	colS	color
#define	colU	lcolor
#endif

#define C_cpy(A,B) 	A.r= B.r, A.g= B.g, A.b= B.b;
#define C_zero(A)	A.r= 0.0, A.g= 0.0, A.b= 0.0;
#define C_set(A)	A.r= 1.0, A.g= 1.0, A.b= 1.0;
#define C_add(A,B)	A.r+= B.r, A.g+= B.g, A.b+= B.b;
#define C_mult(A,B)	A.r*= B.r, A.g*= B.g, A.b*= B.b;
#define C_blend(A,B,C) A.r= B.r * C.r, A.g= B.g * C.g, A.b= B.b * C.b;
#define	C_expandl(A)	A.r, A.g, A.b
#define C_plus(A,B,C)	A.r= B.r + C.r, A.g= B.g + C.g, A.b= B.b + C.b;
#define	Ck_	1.52590219e-5 
#define	CA_cpy(A, B)	A.r= Ck_ * B.red, A.g= Ck_ * B.green, A.b= Ck_ * B.blue;
#define	C_nonzero(A)	(A.r > 0.0) || (A.g > 0.0) || (A.b > 0.0)
#define	C_bright(a)		(0.3 * a.r + 0.59 * a.g + 0.11 * a.b)

#endif

