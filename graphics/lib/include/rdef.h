#ifndef	RENDER_DEF
#	define	RENDER_DEF	1

   /********/

typedef struct
{
   int
      warp_plate;
   double
      centr_ix, centr_iy;
   vector
      home_point,
      current_view,
      base_view,
      pan_ix, pan_iy,
      plate_ix, plate_iy;
   } er_camera;

   /********/

#define	ER_AMBIENT	0x0001
#define	ER_LUMEN	0x0002
#define	ER_DIFFUSE	0x0004
#define	ER_SPECULAR	0x0008
#define	ER_REFLECT	0x0010
#define	ER_GLASS	0x0020
#define	ER_TRANSMT	0x0040
#define	ER_VAPOR	0x0080

#define	TRS_HAMBIENT	0x01
#define	TRS_HLUMEN		0x02
#define TRS_HDIFF		0x04
#define TRS_HSPEC		0x08
#define TRS_HREFLECT	0x10
#define TRS_HGLASS		0x20
#define TRS_HWET		0x80

#define TRS_TAMBIENT	0x0001
#define TRS_TLUM		0x0002
#define TRS_TDIFF		0x0004
#define TRS_TSPEC		0x0008
#define TRS_TREFLECT	0x0010
#define TRS_TGLASS		0x0020
#define TRS_TTRANS		0x0040
#define TRS_TCOLOR		0x0080
#define TRS_TALT		0x0800
#define TRS_TBUMP		0x0400
#define TRS_TGLOS		0x0200
#define TRS_TINDX		0x0100
#define	TRS_TALPHA		0x1000
#define	TRS_XALPHA		0x2000
#define TRS_XGLASS		0x4000
#define	TRS_XSTENCIL	0x8000

#define	TRS_TMIX		0x10000
#define TRS_TCOMP		0x20000
#define TRS_TENVIR		0x40000

#define ER_MDS		0x010c
#define ER_TDS		0x00b0
#define ER_LIQUID	0x0100
#define	ER_POSTER	0x0200
// #define ER_ALVAP	0x0200
#define	ER_ALPHA	0x0400
// #define ER_DUALSPEC	0x0800

#define	ER_ENVIR	0x1000

#define	ERS_INVIS	0x010000
#define ERS_ALITE	0x020000

#define ERT_PPLANAR		0x0001
#define ERT_PCUBIC		0x0002
#define ERT_PCYLN		0x0003
#define ERT_PSPHER		0x0005
#define ERT_PENV		0x0007

#define ERT_PTILE		0x0010
#define ERT_PTILEMIR	0x0020

#define	EMP_COLOR	0x1
#define	EMP_V		0x10
#define EMP_A		0x20
#define	EMP_BMP		0x100
#define EMP_AB		0x200

typedef struct
{
   vekU
      mmx, mmy, mmz,
      mmw;
   } er_matrix;

typedef struct smap
{
   char imname[128];
   int
      bits, marked,
      wide, tall, wideplus, tallplus;
   double imscalex,imscaley;
   unsigned char *vdat;
   struct smap *next;
   } er_smap;

typedef struct
{
	long attrib;
	colU
		col_a, col_l, col_d, col_s,
		col_r, col_t, col_i;
	double
		spec, xval,
		indx, sqindx, rindx, rqindx,
		bend, alpha,
		in_t1, in_t2, ut, uit,
		v1, v2, v3, v4;
} er_surface;

typedef struct
{
	short	bits, marked,
		imw, imh, imwp, imhp;
	double	iscax, iscay;
	unsigned char
		*array_base,
		*arrayv, *arrayam,
		*arrayc, *arraybmp;
} ev_smap;

typedef struct
{
	long mapt, mapcode, mapstencil;
	double imadjx, imadjy;
	ev_smap *pixmapref;
} ev_mapping;

typedef struct
{
	unsigned short
		mapct, mapfin, 
		xsolid, xmajor, xfin,
		pjcode, prepx, prepy;
	ev_mapping *maplist;
	unsigned char *solidblock;
	double
		cx_a, cx_l, cx_d, cx_s,
		cx_r, cx_t, cx_i;
	colU basemix, ambmix,
		cma, cml, cmd, cms, cmr, cmt, cmi;

	double
		pscalex, pscaley,
		plat, plng;
	vekU
		matx, maty, matz, matw,
		imatx, imaty, imatz;
} ev_mapdat;

typedef struct
{
	unsigned short
		mapct, mapcomb;
	er_surface
		*surfp,
		*surfmix;
	ev_mapdat *maplist;
	vector
		*global_v,
		*motion_t;
	svector
		*global_n;
	void	(* shader)() ;
	void	(* lighting)(vector *, void *) ;
} ev_surfbsi;

typedef struct
{
	int cts, ctm, cto;
	er_surface *newsurf;
	ev_mapping *newmap;
	ev_mapdat *newref;
	unsigned char *newblock;
} er_creatsrec;


#define	RSIDE_X		0
#define	RSIDE_Y		0x1000
#define	RSIDE_BELOW	0
#define	RSIDE_ABOVE	0x2000
#define	RSIDE_INSIDE	0x4000
#define	RSIDE_END	0x8000

#define RACE_NORMAL	0x4000
#define RACE_TEXTURE	0x8000
#define RACE_MASK	0xc000
#define RACE_LENS	0x1000
#define RACE_INVIS	0x2000

#define	RACE_PLANEX	0x400
#define	RACE_PLANEY	0x800
#define	RACE_PLANEZ	0xc00
#define	RACE_PLANEM	0xc00  /** mask */

typedef struct { long bits;  double m, b; } er_side;

typedef struct
{
   unsigned short
      bits, ray_count;
   er_side *bound_sidea;
   unsigned short *indic_sides;
   ev_surfbsi *shading;
   svector face_normal;
   double plate_d;
   } er_face;

typedef struct
{
   unsigned short
      bits, ray_count;
   er_side *bound_sidea;
   unsigned short *indic_sides;
   ev_surfbsi *shading;
   vekU face_normal;
   double
      plate_d,
      ubx, uby, ubz;
   } er_uface;

   /***
      statistics on component sizes,
	 vert, norms, borders, faces, ufaces, surfaces
      link to scene element
      object bounds
      transformed vert & norm, orig vert & motion transform
      original object binary face+side+surface list
      simple faces
      bump faces
      */

#define	RFACE_BUMP	0x100

typedef struct
{
   long
      bits,
      stat_v, stat_f;
   vector
      lbound, hbound,
      *global_v, *global_n,
      *local_mv, *motion_t;
   union { er_face *a;  er_uface *u; } use_face;
   er_surface *use_surf;
   ev_surfbsi *use_basis;
   unsigned char *use_block;

   void *element_link;

   unsigned short 
      *mem_insides,
      *mem_xtrasi;
   er_side *mem_side;
   } ev_ffile;

typedef struct
{
   int attrib;
   double radius2;
   vector centr;
   ev_surfbsi *shading;
   } er_smot;

typedef struct
{
   vekU
      matx, maty, matz,
      imatx, imaty, imatz,
      nmatx, nmaty, nmatz,
      matw;
   } er_sform;

typedef struct
{
   int
      ct_ball, ct_deform;
   er_surface *bsurf;
   ev_surfbsi *bshad;
   unsigned char *bsolid;
   } er_smear;

   /********/

typedef struct
{
   unsigned short
      count_tiny, count_large;
   er_face **things;
   } er_cell;

typedef struct
{
   long
      cn, sx, sy, sz,
      cell_subz,
      cell_ct;
   vekU
      lb, hb,
      vcbase,
      vcsize, 
      vcscale;
   ev_ffile **contain;
   er_cell *cells, *cellsmax;
   } er_grid;

   /********/

#define	ERL_POINT	0x1000
#define ERL_SPOT	0x2000
#define ERL_BEAM	0x4000
#define ERL_QUAD	0x5000

#define ERLA_SHAD		0x0800
#define ERLA_SOFT		0x0400
#define	ERLA_PROJ		0x0008

#define ERLA_CONE		0x0200
#define ERLA_POWER	0x0080
#define ERLA_NEAR	0x0200
#define ERLA_FALL		0x0100

#define	ERLA_SHADSOLID	0x20
#define	ERLA_SHADSAFE	0x40
#define ERLA_BUFFER		0x10

typedef struct
{
   unsigned short
      tipe, bits,
      samples, xx;
   double
      cone_edge, cone_bound, cone_scale,
      zone_d, zone_weight;
   lcolor
      color;
   svector
      pt, direc,
      falloff;

   er_face *cache_1;
   er_cell *cache_2;
   } er_light;

#ifdef	TF_SCENE_STRUCTURES

typedef struct
{
   int b_loaded, b_size;
   void *ref;
   tscene_surface *contents;
   tsc_map *maps;
   void *solids;
   unsigned char *sodblock;
   } er_surfdict;

#endif

typedef struct
{
	unsigned short
		attrib,
		prepx, prepy, plast;
	unsigned long safepad;
	svector
		vcore, vbase,
		vplanei, vplanej,
		wscale, woffset;
	unsigned long
		* buffer;
} ev_lightshadowbuffer;

typedef struct
{
	unsigned short
		mapct, mapfin,
		pjcode, prepx, prepy, pzzz;
	ev_mapping *maplist;
	double
		pscalex, pscaley,
		plat, plng;
	vekU
		matx, maty, matz, matw,
		imatx, imaty, imatz;
} ev_lightmaptable;

typedef struct
{
	unsigned short
		tipe, bits,
		samples, zzz;
	long srcid;
	double
		cone_edge, cone_bound, cone_scale,
		zone_d, zone_weight, safedist;
	lcolor color;
	svector
		pt, direc, lunge, lup,
		falloff;

	ev_lightshadowbuffer *lshadmap;
	ev_lightmaptable *lprojmap;	
	er_face *cache_1;
	er_cell *cache_2;
	} ev_light;

   /********/

#define	RAY_PARAX	0x0001
#define	RAY_PARAY	0x0002
#define	RAY_PARAZ	0x0004
#define	RAY_NEGX	0x0010
#define	RAY_NEGY	0x0020
#define	RAY_NEGZ	0x0040

typedef struct
{
   unsigned short
      kinmd, tributs;
   ev_surfbsi *surfat;
   er_face *facptr;
   er_smot *spherptr;
   er_sform *warptr;
   double
      dist, dost;
   } ray_opaque;

typedef struct
{
   er_face *target;
   er_smot *sarget;
   unsigned long
      hashing_code,
      hashing_shadow;
   short
      bits, ray_count, 
      smi_u, smi_v;
   vector
      space_p, 
      space_d,
      space_dn,
      hit_p, hit_n;
   double
      dist, paint_u, paint_v,
      result_a, result_b, result_c, result_d;
   } ray_element;

typedef struct { double tnear;  er_cell *p; } cell_active;

   /********/

#endif