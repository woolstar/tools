#ifndef	TF_RENDER_STRUCTURES
#define	TF_RENDER_STRUCTURES	1

#include "tscenelink.h"

	 /********/

/*** camera */
/*
	attribs ( use scaling, use viewplane stretch )
	cfov - degrees
	cratio - pixel aspect ratio
	cplatex, cplatey - scaling
		-
	starting position, view vector, up vector, side vector
		[for ray tracing:]
	basev = cos(fov) * cview, scaled for raytr
	panx = cside * sin(fov) / image_width, pany ...
	platex = cside * cplatex / image_width, platey ...
**/

typedef struct
{
	unsigned long attrib;
    int
        view_ixl, view_ixr,
        view_iyt, view_iyb,
        paint_lx, paint_hx,
        paint_ly, paint_hy,
        shadrate, zzzz;
	long
		page_width,
		page_size;
	double
		cfov, cratio,
        scale_x, scale_y, scale_z,
        offset_x, offset_y, offset_z,
		cplatex, cplatey;
	vector cpos, basev;
	svector
		cview, ctop, cside,	/** cw, cz, cy, cx **/
		panx, pany, platex, platey;
	unsigned char * pixbuf;
	unsigned long * depthbuf, * backbuf;

} tr_camera;

	 /********/

/*** collectors */

typedef struct
{
	void *trtxrbase;
	void *trmapbase;
	void *ersurf;
 	void *erbasis;
	void *ermap;
	void *ermapref;
} smem;

typedef struct { long bits;  double m, b; } tro_side;
typedef struct { char name[64];  scolor base; } tro_surface ;

typedef struct
{
	unsigned short
		bits, ray_count;
	tro_side *bound_sidea;
	unsigned short *indic_sides;
	tro_surface *shading;
	svector face_normal;
	double plate_d;
} tro_face;

typedef struct
{
	unsigned short
		mark, surface_c;
	unsigned long
		vector_c, normal_c,
		poly_c;
	vector *vlist;
	svector *nlist;
	tro_surface *slist;
	smem sdata;
	unsigned short
		*ilist;
	tro_face *flist;
} tro_polys;

typedef struct
{
	long ia, attr;
	vector ibound, jbound;
	tro_polys ta;
	// void * matchref;
	tlist_element * iamatch ;
} tr_object;

typedef struct
{
	int count;
	vector ibound, jbound;
	tr_object **list;
} tr_set;

#endif
