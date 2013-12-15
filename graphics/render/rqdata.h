#ifndef	RENDER_QDATA
#	define	RENDER_QDATA	1

	/** image section */

typedef struct
{
	unsigned long
		width, page, height ;
	unsigned char
		* image ;
	unsigned long
		* depth, * ref ;
	svector
		* normal ;
} q_image ;

#if 0
	svector		*workspace;
	dx_point	*workpoint;
	dx_edge		*workedge, *workefar;

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern	q_image	workset ;

#ifdef __cplusplus
}
#endif

#endif