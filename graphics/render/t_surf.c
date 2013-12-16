#include "trun.h"
#include "rodef.h"

   /***/

extern tlist_scene	tfocus;
extern shurt	etr_log;

extern void	csolid_setup();
extern ev_smap *cpmap_lookup(char *, int);

extern char	*mem_name();
extern int	csolid_prop(),
		(*rbuild_idle)();

extern double	val_ambient;

#define	RIDLE	if ((NULL != rbuild_idle) && (*rbuild_idle) ()) return 1;

   /***/

#define	EM0	el_search-> markup= 0;
#define	CSZ(XXX)	else { C_zero(ptr-> XXX) }

void	tsurf_convertr(er_surface *ptr, tscene_surface *src)
{
	long
		ka, kb, kn;
	double
		dtmp, c2, mix;
	color basecol;

	ka= src-> attr,
	kb= src-> amix;
	kn= 0;

	ptr-> alpha= 0.0;
	if (ka & TRS_HAMBIENT)
		{ C_blend(ptr-> col_a, tfocus.setup.ambient, src-> ca)  kn|= ER_AMBIENT; } CSZ(col_a)
	if (ka & TRS_HLUMEN) { C_cpy(ptr-> col_l, src-> cl)  kn|= ER_LUMEN; } CSZ(col_l)
	if (ka & TRS_HDIFF) { C_cpy(ptr-> col_d, src-> cd)  kn|= ER_DIFFUSE; } CSZ(col_d)
	if (ka & TRS_HSPEC) { C_cpy(ptr-> col_s, src-> cs)  kn|= ER_SPECULAR; } CSZ(col_s)
	if (ka & TRS_HREFLECT) { C_cpy(ptr-> col_r, src-> cr)  kn|= ER_REFLECT; } CSZ(col_r)
	if (ka & TRS_HGLASS) { C_cpy(ptr-> col_t, src-> ct)  kn|= ER_GLASS; } CSZ(col_t)
	if (ka & TRS_INVIS) kn |= ERS_INVIS;

/*
/ *
	double c1;

	if (ka & TRS_HWET)
	{
		dtmp= src-> wetl * M_PI / 180.0, c1= cos(dtmp);
		dtmp= src-> weth * M_PI / 180.0, c2= cos(dtmp);
		dtmp= c1 - c2;  if (dtmp > 1e-10) { mix= 1.0 / dtmp; } else mix= 1.0;
		ptr-> wet_in= c1,
		ptr-> wet_out= c2,
		ptr-> wet_rate= mix;
		kn |= ER_LIQUID;
	}
* /
*/

	if (ka & TRS_ALPHALITE)
	{
		kn |= ERS_ALITE;
		if (src-> alpha > 1e-8)
			ptr-> alpha= 1.0 / src-> alpha;
			else ptr-> alpha= 1.0;
	}
	else
		if (ka & TRS_ALPHA) { kn |= ER_ALPHA;  ptr-> alpha= src-> alpha; }

/*	if (ka & TRS_ALPHVAP)
	{
		kn |= ER_ALVAP, kn &= ~ER_ALPHA;
		ptr-> alpha= src-> alpha;
	}
*/
	if (kb)
	{
		C_cpy(basecol, src-> base)
		if (kb & TRS_HAMBIENT)
		{
			dtmp= src-> ka,
			C_blend(ptr-> col_a, dtmp * basecol, tfocus.setup.ambient)
			kn |= ER_AMBIENT;
		}
		if (kb & TRS_HLUMEN)
		{
			dtmp= src-> kl,
			C_cpy(ptr-> col_l, dtmp * basecol)
			kn |= ER_LUMEN;
		}
		if (kb & TRS_HDIFF)
		{
			dtmp= src-> kd,
			C_cpy(ptr-> col_d, dtmp * basecol)
			kn |= ER_DIFFUSE;
		}
		if (kb & TRS_HSPEC)
		{
			dtmp= src-> ks,
			C_cpy(ptr-> col_s, dtmp * basecol)
			kn |= ER_SPECULAR;
		}
		if (kb & TRS_HREFLECT)
		{
			dtmp= src-> kr,
			C_cpy(ptr-> col_r, dtmp * basecol)
			kn |= ER_REFLECT;
		}
		if (kb & TRS_HGLASS)
		{
			dtmp= src-> kt,
			C_cpy(ptr-> col_t, dtmp * basecol)
			kn |= ER_GLASS;
		}
	}
	if (kn & ER_SPECULAR)
	{
		ptr-> spec= src-> spec;
/* // lost feature
		if (ka & TRS_DUALSPEC)
		{
			kn|= ER_DUALSPEC;
			ptr-> dualmix0= 1.0 -src-> dma,
			ptr-> dualmix1= src-> dma,
			ptr-> dualspec= src-> dspec;
		}
*/
	}
	if (kn & ER_GLASS)
	{
		/*** /// setup indx -2,-1,+1,+2 and fresnell lens equations */
		dtmp= src-> indx;
		if (dtmp > 1.00001)
		{
			ptr-> indx= dtmp, ptr-> rindx= 1.0 / dtmp;
			c2= dtmp * dtmp, ptr-> sqindx= c2, ptr-> rqindx= 1.0 / c2;
			ptr-> in_t1= c2 +1.0;
			mix= 1.0 / (8 * c2) + 0.5 / ( 1 + c2 * (2 + c2)),
			ptr-> in_t2= (1 - mix * (3 * c2 + 1)) / mix;
			dtmp= C_bright(ptr-> col_t) ;
			if (dtmp < 1.0) { ptr-> ut= dtmp, ptr-> uit= 1.0 - dtmp; }
				else { ptr-> ut= 1.0, ptr-> uit= 0.0; }
		}
		else { kn &= ~ER_GLASS, kn |= ER_VAPOR; }
	}
	else
		{ if (kn & ER_TRANSMT) kn &= ~ER_TRANSMT; }

	ptr-> attrib= kn;
	if (ka & TRS_BUMPY)
		ptr-> bend= src-> bend; else ptr-> bend= 1.0;
}

int	tsurf_convertmap(ev_mapping *ptr, ev_mapdat *master, tsc_texture *texref, tscene_surface *src)
{
	unsigned long
		ka, kb, kta;
	ev_smap *epptr;

	ka= src-> attr, kb= src-> amix;
	kta= texref-> lapply;

	if (kta & TRS_TCOLOR)
	{
		if (! kb) RE
		epptr= cpmap_lookup(texref-> name, EMP_COLOR);
		if (epptr == NULL) RE
		ptr-> mapt= 1,
		ptr-> mapcode= TRS_TCOLOR,
		ptr-> mapstencil= 0,
		ptr-> pixmapref= epptr;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> basemix, src-> base)
		}
		if (kb & TRS_HAMBIENT) { master-> cx_a= src-> ka * val_ambient; }
		if (kb & TRS_HLUMEN) { master-> cx_l= src-> kl; }
		if (kb & TRS_HDIFF) { master-> cx_d= src-> kd; }
		if (kb & TRS_HSPEC) { master-> cx_s= src-> ks; }
		if (kb & TRS_HREFLECT) { master-> cx_r= src-> kr; }
		if (kb & TRS_HGLASS) { master-> cx_t= src-> kt; }
		return 0;
	}

	if (kta & TRS_TAMBIENT)
	{
		ptr-> mapcode= TRS_TAMBIENT;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_blend(master-> cma, tfocus.setup.ambient, src-> ca)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HAMBIENT)) RE
			master-> cx_a= src-> ka * val_ambient;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HAMBIENT)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}

	if (kta & TRS_TLUM)
	{
		ptr-> mapcode= TRS_TLUM;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> cml, src-> cl)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HLUMEN)) RE
			master-> cx_l= src-> kl;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HLUMEN)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TDIFF)
	{
		ptr-> mapcode= TRS_TDIFF;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> cmd, src-> cd)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HDIFF)) RE
			master-> cx_d= src-> kd;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HDIFF)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TSPEC)
	{
		ptr-> mapcode= TRS_TSPEC;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> cms, src-> cs)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HSPEC)) RE
			master-> cx_s= src-> ks;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HSPEC)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TREFLECT)
	{
		ptr-> mapcode= TRS_TREFLECT;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> cmr, src-> cr)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HREFLECT)) RE
			master-> cx_r= src-> kr;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HREFLECT)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TGLASS)
	{
		ptr-> mapcode= TRS_TGLASS;
		if (kta & TRS_TMIX)
		{
			ptr-> mapcode |= TRS_TMIX;
			C_cpy(master-> cmt, src-> ct)
		}
		if (kta & TRS_TCOMP)
		{
			if (!(kb & TRS_HGLASS)) RE
			master-> cx_t= src-> kt;
			epptr= cpmap_lookup(texref-> name, EMP_V);
			ptr-> mapt= 2,
			ptr-> mapcode |= TRS_TCOMP;
		}
		else
		{
			if (!(ka & TRS_HGLASS)) RE
			epptr= cpmap_lookup(texref-> name, EMP_COLOR);
			ptr-> mapt= 1;
		}
		if (epptr == NULL) RE
		ptr-> mapstencil= 0;
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TINDX)
	{
		if (!(ka & TRS_HGLASS) && !(kb & TRS_HGLASS)) RE
		epptr= cpmap_lookup(texref-> name, EMP_V);
		if (epptr == NULL) RE
		ptr-> mapt= 2,
		ptr-> mapcode= TRS_TINDX,
		ptr-> mapstencil= 0,
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TGLOS)
	{
		if (!(ka & TRS_HSPEC) && !(kb & TRS_HSPEC)) RE
		epptr= cpmap_lookup(texref-> name, EMP_V);
		if (epptr == NULL) RE
		ptr-> mapt= 2,
		ptr-> mapcode= TRS_TGLOS,
		ptr-> mapstencil= 0,
		ptr-> pixmapref= epptr;
		return 0;
	}
	if (kta & TRS_TBUMP)
	{
		epptr= cpmap_lookup(texref-> name, EMP_BMP);
		if (epptr == NULL) RE
		ptr-> mapt= 4,
		ptr-> mapcode= TRS_TBUMP,
		ptr-> mapstencil= 0,
		ptr-> pixmapref= epptr;

		M_zero(master-> imatx)  master-> imatx.x= 1.0;
		M_zero(master-> imaty)  master-> imaty.y= 1.0;
		M_zero(master-> imatz)  master-> imatz.z= 1.0;
		return 0;
	}
	if (kta & TRS_TENVIR)
	{
		epptr= cpmap_lookup(texref-> name, EMP_COLOR);
		if (epptr == NULL) RE
		ptr-> mapt= 1,
		ptr-> mapcode= TRS_TENVIR,
		ptr-> mapstencil= 0,
		ptr-> pixmapref= epptr,

		master-> pjcode= ERT_PENV;
		M_zero(master-> matx) M_zero(master-> maty)
			M_zero(master-> matz) M_zero(master-> matw)
		master-> pscalex= 0.5 / M_PI,
		master-> pscaley= 1.0 / M_PI;
		master-> plat= 0.5, master-> plng= 0.5;
		return 0;
	}

	return -1;
}

int	tsurf_build(tscene_surface *surfptr, int n, ev_surfbsi *ebptr, er_creatsrec *recptr)
{
	unsigned int istep;
	int iret;
	int
		nw, nww, nwd,
		ka, km, kpj;
	long nwb;

	tsc_texture *txstep;
	tsc_map *mapptr;
	tscene_surface *tsstep;
	er_surface
		*esbase,
		*esuse, *esdup;
	ev_mapping 
		*embase,
		*emuse;
	ev_mapdat
		*edbase,
		*edstep;
	ev_surfbsi *ebstep;
	byte *eobase;

	nw= n, nww= 0, nwd= 0, nwb= 0;
	for (istep= n, tsstep= surfptr, ebstep= ebptr; (istep--); tsstep++, ebstep++)
	{
		ka= tsstep-> mapcount;
		if (ka)
		{
			nw++;
			for (km= 0, mapptr= tsstep-> maplist; (ka --); mapptr ++)
			{
				if (! mapptr-> mapcount) continue ;
				km+= mapptr-> mapcount,
				nwd++;
			}
			nww += km;
			ebstep-> mapct= km;
		}
		/*
		kb= sfstep-> tsa_count;
		if (kb) { nw++, nwd++, nwb+= sfstep-> proslist-> pos_resource; }
		*/
	}
	if (nww) nww++;

	esbase= EMEM(nw, er_surface),
	embase= EUMEM(nww, ev_mapping),
	edbase= EUMEM(nwd, ev_mapdat),
	eobase= EUMEM(nwb, byte);
	if ((esbase == NULL) || (nww && (embase == NULL))
			|| (nwd && (edbase == NULL)) || (nwb && (eobase == NULL)))
		return 2;

	for (istep= nww, emuse= embase; (istep--); emuse++)
		{ emuse-> imadjx= emuse-> imadjy= 0.0; }

		/*** master conversion loop */
	for (istep= n, esuse= esbase, emuse= embase, edstep= edbase,
		ebstep= ebptr, tsstep= surfptr; 
		(istep--); esuse++, ebstep++, tsstep++)
	{
		tsurf_convertr(esuse, tsstep);

		ebstep-> surfp= esuse,
		ebstep-> surfmix= NULL;
		ebstep-> mapct= 0, ebstep-> mapcomb= 0,
		ebstep-> maplist= NULL;

		if (kpj= tsstep-> mapcount)
		{
			for (mapptr= tsstep-> maplist; (kpj --); mapptr ++)
			{
				edstep-> maplist= emuse,
				edstep-> mapct= 0, edstep-> mapfin= 0,
				edstep-> xsolid= 0,
				edstep-> cx_a= edstep-> cx_d= edstep-> cx_s=
					edstep-> cx_r= edstep-> cx_t= edstep-> cx_i= 0.0,
				edstep-> pjcode= edstep-> prepx= edstep-> prepy= 0,
				edstep-> pscalex= edstep-> pscaley= 0.0;
				edstep-> matx.x= 1.0, edstep-> matx.y= 0.0, edstep-> matx.z= 0.0,
				edstep-> maty.x= 0.0, edstep-> maty.y= 1.0, edstep-> maty.z= 0.0,
				edstep-> matz.x= 0.0, edstep-> matz.y= 0.0, edstep-> matz.z= 1.0,
				edstep-> matw.x= 0.0, edstep-> matw.y= 0.0, edstep-> matw.z= 0.0;

				/*
				bv_usetcol= 0, bv_fintcol= kb, bv_ovsolidcol= 0;
				*/

				if (! (km= mapptr-> mapcount))
					continue ;
				for (txstep= mapptr-> imagelist; (km --); txstep ++)
				{
					iret= tsurf_convertmap(emuse, edstep, txstep, tsstep);
					if (! iret)
					{
						if (TRS_TENVIR == txstep-> lapply)
							{ esuse-> attrib |= ER_ENVIR; }
						edstep-> mapct ++; emuse++;
					}
				}
				if (edstep-> mapct)
				{
					C_cpy(edstep-> basemix, tsstep-> base)
					C_cpy(edstep-> ambmix, tfocus.setup.ambient)
					edstep-> mapfin= tsstep-> amix; /*** BAD !! FIX ***/
					ebstep-> mapcomb= ERSA_MAP;

					if (! ebstep-> mapct) { ebstep-> maplist= edstep; }
					ebstep-> mapct ++;
					edstep ++;
				}
			}
		}

		if (ebstep-> mapcomb)
		{
			esdup= esuse +1, *esdup= *esuse;
			ebstep-> surfmix= esdup,
			esuse ++;
		}
	}

	if (nww)
	{
		emuse-> mapt= 0, emuse-> mapcode= 0,
		emuse-> imadjx= 0.0, emuse-> imadjy= 0.0;
		emuse-> pixmapref= NULL;
	}

	recptr-> cts= nw,
	recptr-> ctm= nww,
	recptr-> cto= nwb;
	recptr-> newsurf= esbase,
	recptr-> newmap= embase,
	recptr-> newref= edbase,
	recptr-> newblock= eobase;

	return 0;
}

#if 0
	byte
		*eostep;

	if (sfstep-> tsa_count)
	{
		epsodstep= sfstep-> proslist, j= epsodstep-> pos_resource;
		edstep-> xsolid= k= epsodstep-> pos_code,
		csolid_prop(k, &kc, &km);
		edstep-> xmajor= kc,
		edstep-> xfin= km,
		edstep-> solidblock= eostep;
		edstep-> mapct= 0, edstep-> mapfin= 0,
		edstep-> pjcode= 0, edstep-> prepx= 0, edstep-> prepy= 0,
		edstep-> maplist= NULL;
		if (km & TRS_HAMBIENT)
		{
			if ((ka & TRS_HAMBIENT) && !(kb & TRS_HAMBIENT))
			{
				C_cpy(edstep-> ambmix, esuse-> col_a)
				edstep-> xfin|= ES_CAMBIENT;
			}
			else { edstep-> cx_a= src-> ka * val_ambient; }
		}
		if (km & TRS_HLUMEN) { edstep-> cx_l= src-> kl; }
		if (km & TRS_HDIFF) { edstep-> cx_d= src-> kd; }
		if (km & TRS_HSPEC) { edstep-> cx_s= src-> ks; }
		if (km & TRS_HREFLECT) { edstep-> cx_r= src-> kr; }
		if (km & TRS_HGLASS) { edstep-> cx_t= src-> kt; }
		if (km & TRS_HLENS) { edstep-> cx_i= src-> ki; }

		csolid_setup(k, epsodstep-> resblock, eostep);
		eostep+= j;
		C_cpy(edstep-> basemix, sfstep-> color)
		ebstep-> mapcomb|= ERSA_SOLID;
	}
#endif

