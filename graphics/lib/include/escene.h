   /****
      Copyright 1994 Video Bits
      All Rights Reserved
      */

#define	E_Epsi	-1e10

#define	EFIL_ID3OB	0x44e0
#define EFIL_IDSCT	0x44e1
#define EFIL_RGB	0x03
#define EFIL_VID	0x44ec
#define EFIL_VAD	0x44ef

#define	VISW_IGENERAL	0
#define	VISW_IGROUP	0
#define	VISW_ITIPE	0
#define	VISW_PCLI	1
#define	VISW_PSCO	2
#define VISW_PSCT	3
#define VISW_PACF	4
#define VISW_PAOB	6
#define VISW_PSRF	7
#define VISW_P3OB	8
#define VISW_PSUP3D	9
#define VISW_PTETU	12
#define	VISW_PIMA	13
#define	VISW_PINT	50
#define VISW_PFLOAT	51
#define VISW_PSTRING	52

#define	ESP_QUIET	0
#define ESP_BABBLE	1
#define ESP_NOISY	2

   /***** ray 30 compatibility */

#define	FACE_MN		0x1000
#define FACE_MT		0x2000
#define	FACE_M		0x3000

#define	OLDES_LUMEN	0x01
#define OLDES_DIFF	0x02
#define OLDES_SPEC	0x04

   /***** object elements */

#define FLAT_SPACE	4096
#define FRACT_INIT	8192
#define FRACT_STACK	16384+8192
#define FRACT_FINAL	8192+4096

#define	HOLE_LIMIT	32

#define	MAXOBH	8

#define	ELOB_ID		0x7841
#define	ELOB_FEADER	0x1001
#define	ELOB_BEADER	0x1002
#define	ELOB_SURF	0x4001
#define ELOB_BLEND	0x4401
#define	ELOB_BASIS	0x5001
#define	ELOB_VERT	0x8001
#define	ELOB_STRATA	0x4011
#define	ELOB_FACE	0xa001
#define	ELOB_SPHERE	0xb001
#define	ELOB_END	0xffff

#define	EPB_FACE	0x4000
#define EPB_FACTN	0x1000
#define EPB_FACTT	0x2000
#define EPB_FACTM	0x3000
#define EPB_ATTRIB	0x8000
#define EPB_SURF	0x8400
#define EPB_BASIS	0x8800
#define EPB_EEEK	0x8c00

#define EPBA_UNFRACT	0x0100

#define	ES_VERSION	0x04010000

#define	ES_HAMBIENT	0x01
#define	ES_HLUMEN	0x02
#define ES_HDIFF	0x04
#define ES_HSPEC	0x08
#define ES_HREFLECT	0x10
#define ES_HGLASS	0x20
#define ES_HLENS	0x40
#define ES_HWET		0x80

#define ES_INVIS	0x100
#define ES_DUALSPEC	0x200
#define ES_ALPHA	0x400
#define ES_ALPHVAP	0x800
#define ES_CAMBIENT	0x1000
#define ES_SOLID	0x2000
#define ES_ALPHALITE 0x4000

#define ES_TAMBIENT	0x0001
#define ES_TLUM		0x0002
#define ES_TDIFF	0x0004
#define ES_TSPEC	0x0008
#define ES_TREFLECT	0x0010
#define ES_TGLASS	0x0020
#define ES_TTRANS	0x0040
#define ES_TCOLOR	0x0080
#define ES_TALT		0x0800
#define ES_TBUMP	0x0400
#define ES_TGLOS	0x0200
#define ES_TINDX	0x0100
#define	ES_TALPHA	0x1000
#define	ES_XALPHA	0x2000
#define ES_XGLASS	0x4000
#define	ES_XSTENCIL	0x8000

#define	ES_TMIX		0x1000
#define ES_TCOMP	0x2000
#define ES_TENVIR	0x4000

#define ES_PROJECT	0x0200
#define ES_PPLANAR	0x0001
#define ES_PCUBIC	0x0002
#define ES_PCYLN	0x0003
#define ES_PSQCYLN	0x0004
#define ES_PSPHER	0x0005
#define ES_PALIGN	0x0006
#define ES_PENV		0x0007
#define ES_PPOINT	0x0008
#define ES_PTILE	0x0010
#define ES_PTILEMIR	0x0020

#define ES_VCOLOR	1
#define ES_VHORIZON	2
#define ES_VTEXTURE	3
#define ES_VPICTURE	4

#define	EWP_REVERSE	0x10

thing
{
   int tm_code;
   char tm_name[StrNAME];
} ep_mapping;

thing
{
   realU
      vscale, vturb;
   colU
      amain, bmain,
      aveib, bveib,
      avein, bvein, cvein;
   } eps_solid_marblesv;

thing
{
   realU
      vsize, vscale,
      vshades, vexpo;
   colU
      cgrain;
   } eps_solid_stonesv;

thing
{
   realU
      vgrain, vturb,
      vscale, vswirl, vswfreq;
   colU
      cdark;
   } eps_solid_woodsv;

thing
{
   int pos_code, pos_resource;
   byte *resblock;
   } ep_solidtx;

/*
   this next piece needs real explanation:
   /// surfaces
      surface name, alternate surface name,

      attributes, linked color codes (ie cr== color * xr...),
      texture map properties count,
      generic projection code, generic pjt repeat count xy,
      texture map list

	++++ channel assignment [0..1000]
	++++ bump scaler [real]

      base color,
      ambient color, luminance color, diffuse color, specular color,
      reflective color, transparent color, transmision color

      ambient fraction, diffuse fraction, specular fraction,
      reflective fraction, transmission fraction, invis fraction
      specular cooefficient, mixing fraction, secondary specularity 
      index of refraction
      bump amplitude
      generic projection scaling xy
      { diffuse reflection roughness }

      and that's it (after #DEFINED constants)

   1-22-92 DW
   */

thing
{
   char
      surf_name[StrNAME],
      alt_name[StrNAME];
   int
      attr,
      lc_code;
   shurt
      tm_count, tsa_count,
      genp_proj, genp_repx, genp_repy;
   ep_mapping
      *maplist;
   ep_solidtx
      *proslist;
   colS
      color, 
      ca, cl, cd, cs,
      cr, ct, ci;
   float
      xa, xl, xd, xs,
      xr, xt, xi,
      spec, mxspec, altspec,
      wetl, weth,
      indx, bumpy, alpha,
      genp_scalx, genp_scaly;
   } ep_surface;

/*
   /// blending entry
      blend time
      mix ratio
      first and second pointer

   /// blending lead
      entry count
      list pointer

   **/

thing
{
   shurt
      bpm_code,
      bpm_repx, bpm_repy, bpm_repz;
   double
      bpm_scalx,
      bpm_scaly,
      bpm_scalz;
   } ep_blendmap;

thing
{
   int btime;
   char
      namfir[StrNAME],
      namsec[StrNAME];
   double bmix;
   ep_blendmap
      blmfir,
      blmsec;
   } ep_blend;

thing
{
   int bcount;
   ep_blend *blist;
   } ep_blending;

thing
{
   double blend;
   ep_surface
      *bfir, *bnext;
   ep_blendmap
      *mfir, *mnext;
   } ep_blenduse;

/*
   /// basis
	 projection info, tiling and scaling
	 basis matrix, inverse and offset

   /// general object structure --
	 attributes,
	 count of verticies, normals, surfaces, basis matrix, faces, 
	    sides, insides, datasides and ?
	 vectors arrays of verteces and normals
	 list of basis vectors
	 array of surfaces and maps
	 array of face info

   /// sphere ball
	 surface ref, basis ref
	 transform matrix
   /// general sphere group
	 attributes
	 count of surfaces, basis and entities
	 list of basis vectors
	 array of surfaces and maps
	 entity list

   **/

thing
{
   shurt
      pjt_code, surface_ref,
      pjt_repx, pjt_repy, pjt_repz,
      pjtxx;
   double
      pjt_scalx,
      pjt_scaly,
      pjt_scalz;
   vector
      basx, basy, basz,
      basw;
} ep_basis;

thing
{
   char name[StrNAME];
   shurt
      attrib,
      ct_surf, ct_basis,
      ct_blending;
   long
      ct_face,
      ct_vert, ct_norm,
      ct_side, ct_iside, ct_dside;
   vector
      *h_vert, *h_norm;
   ep_basis *eba_base;
   ep_surface *surf_internal;
   ep_mapping *map_base;
   ep_solidtx *sod_base;
   byte *solid_block;
   ep_blending *blen_list;
   ep_blend *blenent_base;
   shurt *face_list;
} ep_object;

thing
{
   short
      surfi, bassi;
   vekU shx, shy, shz, shw;
   } epe_sphere;

thing
{
   char name[StrNAME];
   shurt
      attrib,
      ct_surf, ct_basis,
      ct_blending,
      ct_ent;
   ep_basis *eba_base;
   ep_surface *surf_internal;
   ep_mapping *map_base;
   ep_solidtx *sod_base;
   byte *solid_block;
   ep_blending *blen_list;
   ep_blend *blenent_base;
   epe_sphere *ent_list;
   } ep_ball;

thing
{
   realU array[12];
   } ep_matrix;

   /***** scene elements */

#define	AV_ASPECT	0x10
#define	AV_WINDOW	0x20
#define AV_WIDEVIEW	0x100

#define	AV_WARPVIEW	0x200
#define	AV_FIXPOS	0x1000
#define	AV_FIXTAR	0x2000

#define	ELM_LIGHT	0x1000
#define	ELM_SPOT	0x2000
#define	ELM_BEAM	0x3000
#define	ELM_SURF	0x7000
#define	ELM_OB		0x4000
#define	ELM_AREA	0x5000
#define	ELM_SPHERE	0x6000

#define ELM_RGB		0x4
#define ELM_HUV		0xc

#define	ELM_COLOR	0x10
#define ELM_SHADOW	0x20
#define ELM_SOFT	0x40
#define ELM_ATTEN	0x80

#define	ELM_NEARFAR	0x800
#define	ELM_LENS	0x800
#define	ELM_CONE	0x2
#define	ELM_POWER	0x1

#define	ELM_MORPH	0x400
#define ELM_EXIST	0x200

#define	FALL_0		0x100
#define FALL_1		0x200
#define FALL_2		0x400
#define FALL_X		0x700

#define	ELAUX_FIXPOS	0x10
#define	ELAUX_SHSOLID	0x100
#define ELAUX_SHSAFE	0x200

#define ELAUX_BUFFER	0x1000

#define	ELME_NEARFAR	0x0100
#define	ELME_LENS	0x0200
#define	ELME_DYNMAT	0x0400

#define	MAX_ATTAT	16
#define	EPTK_HALT	0xffff

#define	OLDESM_NORM	0x0100
#define	OLDESM_ALT	0x0200
#define OLDESM_TEXTURE	0x0400

#define	OLDEST_LUMEN	0x0001
#define	OLDEST_DIFF	0x0002
#define	OLDEST_SPEC	0x0004
#define	OLDEST_REFLECT	0x0010
#define	OLDEST_GLASS	0x0020
#define	OLDEST_INDEX	0x0040
#define	OLDEST_INVIS	0x0080
#define	OLDEST_ALT	0x0200
#define	OLDEST_MIX	0x0400
#define	OLDEST_ALL	0x00b7

#define	ESP_iFLIP	1
#define ESP_fBOX	1
#define ESP_fTRI	2
#define ESP_fGAUSS	3
#define ESP_fLANCZOS	4
#define ESP_NONAME	0
#define ESP_NAME	1
#define ESP_NAMES	2
#define ESP_NAMEMASK	3

#ifdef	MAC
#define	ESP_DEFAULT	{ "image %d", 0, 0, 0, 1, 1, 3, EV_PICT, 0, ESP_NAMEMASK, \
				ESP_BABBLE, 0.001, 0.02, 0.333, 1.95, {0.0, 0.0, 0.0} }
#else
#ifdef	DOS
#define	ESP_DEFAULT	{ "pic%d.tga", 0, 0, 0, 1, 1, 3, EV_TGA + EVT_TP10 + EVT_BT32, 1, \
				ESP_NAMEMASK, ESP_BABBLE, 0.001, 0.02, 0.333, 1.95, {0,0,0} }
#else
#define	ESP_DEFAULT	{ "pic%d.vZ", 0, 0, 0, 1, 1, 3, EV_VID, 0, ESP_NAMEMASK, \
				ESP_BABBLE, 0.001, 0.02, 0.333, 1.95, { 0.0, 0.0, 0.0} }
#endif
#endif

   /****
      scene preferences for core 1.1
      image name
      scene length
      general bit register
      adaptive sampling rate
      filter type
      filter window
      maximum reflect and glass ray depth
      threshold for ray pruning
      image format, up direction
      image naming sequence
      message level
      abekas control step
      texture memory model [tene,tiny,norm,large]
      ambient light level

	++++ image channel set size
	++++ image motion special fields
	++++ image depth special fields

      */

thing EPPexec
{
   char cmdline[255];
   } epp_exec;

thing
{
   char iname[64];
   shurt
      bits, adapt, filtype, filwin,
      rdepth, gdepth,
      iformat, idir, inamecode,
      messages;
   realU
      adelta, subdelta, 
      ambient, gamma;
   colU
      new_ambient;
   epp_exec *prepare;
   } ep_pref;

thing
{
   realU itrate;
   colU
      basec, altbasec,
      topc, bottomc;
   } ep_evcolor;

thing
{
   char tm_name[64];
   ep_basis project;
   } ep_evmap;

thing
{
   union { ep_evcolor ehoriz;  ep_evmap esphere; } ev;
   } ep_evunion;

thing
{
   shurt
      fogatt,
      backatt, reflatt, xx;
   realU
      fogmin, fogmax;
   colU fogc;
   ep_evunion
      evback, evrefl;
   } ep_envir;

thing
{
	shurt
		bits,
		trak, fwarp_trak;
	short
		size_x, size_y,
		window_lx, window_ly,
		window_hx, window_hy;
	double
		global_a,
		aspect;
	vector
		fixpos, fixtarget;
} ep_camera;

   /**** light parts
      soft light primary and secondary sampling
      switch on/off times
      brightness, soft radius, spot attenuation
      falloff vector, static color
      */

thing
{
	unsigned short
		auxbits, samp,
		is_on, is_off, bfx, bfy;
	double
		brite, soft_d, atten, safe;
	vector
		fall,
		fixpos;
	colU color;
} epu_lit;

   /**** surface parts
      name, alternate surface mix, texture, 8bit texture
      atributes, texture code
      amb, dif, spec, ref, tranp, invi
      specular co, reflection specular, 
      index of refraction, plus precal help
      indexparts for fresnell transmittance
      */

thing
{
   char name[StrNAME];
   shurt  cs, colid;
   long sodsz;
   ep_surface *suse;
   ep_mapping *muse;
   ep_solidtx *duse;
   byte *bcuse;
   } ep_furf;

thing
{
   char name[StrNAME];
   unsigned short
      hdr, bits;
   } ep_file;

typedef struct EPEexist
{
   unsigned int t, tx;
   ep_file *fi_object;
   ep_furf *si_object;
   } epe_exist;

thing
{
   shurt
      atrak[MAX_ATTAT];
   epe_exist *exist_ob;
   } epu_act;

thing { shurt code, track; } epa_extension;

   /***** 
      bits - hold for properties of elements
      hitrak - highest entry for lights and obj
      markup - scratch space for marking
      ct_extent - extensions/morphing count
      */

thing
{
   char  name[StrWORD];
   shurt
      bits, hitrak, markup, ct_extent;
   epa_extension *modif;
   epu_lit u_light;  
   epu_act u_actor;
   } ep_element;

   /***** world list */

thing 
{ 
   shurt
      sx, sy, sz, n;
   long s_total;
   ep_element  **my_act;
   /*** VVV static cell array */
   } ep_grid;

thing
{
   shurt
      ct_element, ct_grid,
      ct_file, ct_surf;
   ep_element *actors;
   ep_file *dict_objects;
   ep_furf *dict_surfaces;
   ep_grid *grids;
   } ep_world;

	/*** _T lookup **/
#define	BF_TAG(eee)	trag_lookup= *trag_step; \
			if (*trog_step) { trag_lookup += global_time; } \
			eee= (realU) *trag_lookup, trag_step++, trog_step++;

thing
{
   shurt
      ct_tracks, 
      ct_static, ct_nonstat,
      ct_length;
   byte *nonstat;
   float **data;
   } ep_trag;

   /****
      ep_mem usage
      . scene elements
      . . object exist, light extension
      ..  file entries, surface entries
      ... track pointer, static, data
      ... grid list, content lists, spheres
      */

thing
{
   char 
      *a1, *a2, *a3, *a4,
      *b1, *b2, *b3, *b4,
      *c1, *c2, *c3, *c4,
      *d1, *d2, *d3, *d4,
      *e1, *e2, *e3, *e4;
   } ep_mem;

thing
{
   ep_pref	pref;
   ep_envir	envir;
   ep_camera	camera;
   ep_world	world;
   ep_trag	tracgs;
   ep_mem	mallo;
   } ep_scene;

   /** eprep version */
#define EVERSION	"Eprep release 2.1  Copyright (c) 1992 Video Bits"

   /** tools version */
#define ESTRATVERS	"Estrat 2.0 alpha 1  Copyright (c) 1993 Video Bits"
#define VARVERS		"Varga release 1  Copyright (c) 1992,3 Video Bits"
