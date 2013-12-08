	/** historic surface types */
		/* replaced by ev_ series */

#define ERT_PROJECT		0x0200
#define ERT_PSQCYLN		0x0004
#define ERT_PALIGN		0x0006
#define ERT_PPOINT		0x0008

#define TRS_INVIS		0x100
#define TRS_DUALSPEC	0x200
#define TRS_ALPHA		0x400
#define TRS_ALPHVAP		0x800
#define TRS_CAMBIENT	0x1000
#define TRS_SOLID		0x2000
#define TRS_ALPHALITE	0x4000
#define TRS_BUMPY	0x8000

#define ERSA_MAP	0x1000
#define ERSA_MAPS	0x2000
#define ERSA_BLEND	0x4000
#define ERSA_SOLID	0x0100
#define ERSA_SOLIDS	0x0200
#define ERSA_BLASIS	0x8000
#define ERSA_TWOBLEND	(ERSA_BLEND | ERSA_MAPS)


typedef struct
{
   unsigned short mapcode, mapodd;
   double imadjx, imadjy;
   er_smap
      *tm_image;
} er_mapping;

typedef struct
{
   int ctturb;
   double iscale;
   colU
      lmain, hmain,
      lveid, hveid,
      lvein, mvein, hvein;
   } er_solid_marblesv;

typedef struct
{
   double
      iscale, ishades,
      vshades, vexpo;
   colU
      cgrain;
   } er_solid_stonesv;

typedef struct
{
   int ctturb;
   double
      iscale,
      a3grain,
      a2swirl, a3swfreq;
   colU
      cdark;
   } er_solid_woodsv;

typedef struct
{
   unsigned short
      mapct, mapfin,
      xsolid, xmajor, xfin,
      pjcode, prepx, prepy;
   er_mapping *maplist;
   unsigned char *solidblock;
   double
      cx_a, cx_l, cx_d, cx_s,
      cx_r, cx_t, cx_i;
   colU basemix, ambmix;

   double
      pscalex,
      pscaley;
} er_mapdat;

typedef struct
{
   unsigned short
      mapct, mapcomb;
   er_surface
      *surfp, *surfq,
      *surfmix;
   er_mapdat
      *mapref,
      *mapblen;
   double
      blend_ra,
      blend_rb;
   vekU
      matx, maty, matz, matw,
      imatx, imaty, imatz,
      *global_v, *global_n, *motion_t;
} er_surfbsi ;

typedef struct
{
   vekU
      matx, maty, matz;
   double
      scalx, scaly,
      zerox, zeroy;
} er_platemap;
