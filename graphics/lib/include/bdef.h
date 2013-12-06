   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_DEF
#	define	TFBASIC_DEF	1

typedef unsigned char	byte;
typedef unsigned char	uc;
typedef unsigned char	uchar;
typedef unsigned short	shurt;
typedef unsigned short	us;
typedef unsigned long	lung;
typedef unsigned long	ul;

#define td      typedef
#define thing   typedef struct
#define elst    else if
#define	RE	return 1;

#define SLITTLE	16
#define	StrWORD	32
#define	StrNAME	64
#define	SLINE	128
#define	SPAGE	1024
#define SBLOCK	8192

td	float	realS;
td	double	realU;

#ifdef MAC
#   define	EMEM(a1, a2)	(a2 *) malloc((unsigned long) (a1) * sizeof(a2))
#   define	EMMEM(a1, a2)	malloc((unsigned long) (a1) * sizeof(a2))
#else
#	define	EME(aa)	(aa *) malloc(sizeof(aa))

#   define	EMEM(a1, a2)	(a2 *) malloc((unsigned) (a1) * sizeof(a2))

#   define	EMMEM(a1, a2)	malloc((unsigned) (a1) * sizeof(a2))
#endif

#define EUMEM(a1, a2)	(a1) ? EMEM(a1, a2) : NULL
#define EUMMEM(a1, a2)	(a1) ? EMMEM(a1, a2) : NULL
#define	EFREE(X)	free((char *) X)
#define	EUFREE(X)	if (X != NULL) free((char *) X)

#define	HOLE_LIMIT	32

#endif

