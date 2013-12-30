
#ifndef	TF_SCENE_STRUCTURES
#	define	TF_SCENE_STRUCTURES

#include <bdef.h>

	/****************/

enum elements 
{	

	_E_CAM= 1,

	_E_LIGHT,
	_E_SPOT,
	_E_BEAM,

	_E_OBJECT
};

enum filters 
{	_F_BOX=1,
	_F_TRI,
	_F_GAUSS,
	_F_LANCZOS
};

enum backgroundtype
{
	TSB_COLOR = 1,
	TSB_FILE
};

enum importtype
{
	CAM_CAM = 1,
	CAM_WVM,
	CAM_INC
} ;

thing
{
	int	type, aux ;
	char srcfile[128];

	long n, width;
	float * data ;

} tsrc_import ;

thing
{
	char name[64];

	unsigned long attr;
	unsigned int
		x, y,

		aspect_width, aspect_height,
		wxa, wxb, wya, wyb,
		motion;

	float
		gaspect,
		aspect;

	tsrc_import * import;
	void *parent;
} tscene_camera;

thing
{
	char name[64];
} tscene_character;

thing
{
	int
		bufferx, buffery,
		softrad;
	float
		bright, atten,
		safe, soft,
		buffertol;
	svector fall;
} te_light;

thing
{
	char	srcnameform[128], srcroot[128];
	int ioffset, icode;
} te_object;

thing
{
	ul	cmdtype, cmdatr, cmdport;
	char	cmdstring[128],
			cmdparams[512];
} te_system;

thing TSelement
{
	char name[64];

	unsigned long type, attr;
	unsigned int
		motion;
	union
	{
		te_light light;
		te_object object;
	} dat;
	te_system *cmd;
	struct TSelement *parent;
} tscene_element;

/*** surfaces */
/**
	colors: ambient, lumen, diffuse, specular, reflective, transparent
	params:
		spec - specular power,
		dspec - secondary spec (strata)
		dma, dmb - spec mix rates (strata), or liquid highlight in/out [wet_in,wet_out]
		dmr - liquid mix rate [wet_rate]
		indx - index of refraction
		alpha - alpha channel value (normally 1)
	*/

thing
{
	long
		lapply,
		lset;
	char name[StrNAME];
} tsc_texture;

thing
{
	unsigned short
		mapcount, projcode;
	long spanx, spany;
	svector scale, offset;
	float postx, posty, postsx, postsy;
	tsc_texture *imagelist;
} tsc_map;

thing
{
	long attr, amix;
	unsigned short
		mapcount, layer;
	char sname[32];
	color
		base,
		ca, cl, cd, cs, cr, ct;
	float
		ka, kl, kd, ks, kr, kt,
		spec, dspec, dma, dmb, dmr,
		indx, alpha, bend;
	tsc_map *maplist;
	char *uext;
} tscene_surface;

thing
{
	long attr;
	float ka, kb;
	tscene_surface
		*ptra, *ptrb,
		lfinal;
} tscene_blend;

	/****************/

#define	TSC_WINDOW		0x10
#define TSC_ASPECT		0x20
#define	TSC_WIDEVIEW	0x40
#define TSC_IMPORT		0x80

thing
{
	int count;
	tscene_camera *list;
} ts_cameras;

#define	TSS_GAMMA		0x10

thing
{
	char imageform[128];
	int
		iformat,
		ioffset,
		icode;
} tss_image;

thing TSSexec
{
	int type;
	char cmdword[256], cmdform[256];
	struct TSSexec *next;
} tss_exec;

thing TSSbackground
{
	int attr;
	color cbase;
	char imagename[256];
} tss_background;

thing
{
	unsigned long	attr;
	unsigned int
		adapt,
		rdepth, gdepth,
		filtertype, filterwidth;
	lcolor ambient;
	float gamma;
	tss_image image;
	tss_background bg;
	tscene_camera *curcam;
	tss_exec *cmdlist;
	char path[128];
} ts_setup;

#define	TSL_SHADOW	0x100
#define TSL_BUFFER	0x200
#define TSL_SOFT	0x400
#define TSL_FALLOFF	0x1000
#define	TSL_SAFE	0x2000

#define	TSLS_CONE	0x10
#define	TSLS_POWER	0x20

#define	TSO_CMD		0x100
#define	TSO_INVIS	0x200

thing
{
	int count;
	tscene_character *list;
} ts_character;

thing
{
	int count;
	tscene_element *list;
} ts_element;

thing
{
	int count;
	tscene_surface *list;
} ts_material;

thing
{
	int count;
	tscene_element **list;
} ts_contain;

thing
{
	int count;
	long totalspan;
	unsigned short *depth;
	float **data;
} ts_track;

thing
{
	char 
		*a1, *a2, *a3, *a4,
		*b1, *b2, *b3, *b4,
		*c1, *c2, *c3, *c4,
		*d1, *d2, *d3, *d4,
		*e1, *e2, *e3, *e4;
} ts_mem;

thing
{
	ts_cameras cam;
	ts_setup setup;
	ts_character chrs;
	ts_element elem;
	ts_material mat;
	ts_contain set;
	ts_track tracks;
	ts_mem memory;
} tscene;

#endif
