#include "rrun.h"

extern ray_element	*current;

static	void	cv_proj(ev_mapdat *sdat, realU *tu, realU *tv)
{
	shurt j, k;
	realU
		dtmp,
		x,y,z,
		mu, mv;
	vekU
		obwor, obspace,
		nhu;

	if (sdat-> pjcode)
	{
		obwor= current-> hit_p,
		obspace.x= M_dot(obwor, sdat-> matx),
		obspace.y= M_dot(obwor, sdat-> maty),
		obspace.z= M_dot(obwor, sdat-> matz);
		M_plus(obspace, sdat-> matw)
			/*** mappings */
		j = sdat-> pjcode;
		switch(k = j & 0x0f)
		{
			case ERT_PPLANAR:
				mu = obspace.x*sdat-> pscalex;
				mv = obspace.y*sdat-> pscaley;
				break;
			case ERT_PCUBIC:
				obwor= current-> hit_n,
				dtmp= nhu.x= M_dot(obwor, sdat-> imatx), x= fabs(dtmp),
				dtmp= nhu.y= M_dot(obwor, sdat-> imaty), y= fabs(dtmp),
				dtmp= nhu.z= M_dot(obwor, sdat-> imatz), z= fabs(dtmp);
				if (x > y) {
					if (x > z) {
						/** big x **/
						if (nhu.x > 0) { mu= obspace.z * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
							 else { mu= -obspace.z * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
					 } else {
						/** big z **/
						if (nhu.z < 0) { mu= obspace.x * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
							else { mu= -obspace.x * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
					}
				} else {
					 if (y > z) {
						/** big y **/
						if (nhu.y > 0) { mu= obspace.x * sdat-> pscalex, mv= obspace.z * sdat-> pscaley; }
							else { mu= obspace.x * sdat-> pscalex, mv= -obspace.z * sdat-> pscaley; }
					} else {
						/** big z **/
						if(nhu.z < 0) { mu= obspace.x * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
							else { mu= -obspace.x * sdat-> pscalex, mv= obspace.y * sdat-> pscaley; }
					 }
				}
				mu += sdat-> plat,
				mv += sdat-> plng;
				break;
			case ERT_PSPHER:
/*			case ERT_PSQCYLN: */
				M_selfnorm(obspace);
				mu= sdat-> pscalex * atan2(obspace.x,obspace.z) + sdat-> plat;
				mv= sdat-> pscaley * asin(obspace.y) + sdat-> plng;
				break;
			case ERT_PCYLN:
				mu= sdat-> pscalex * atan2(obspace.x,obspace.z) + sdat-> plat;
				mv= obspace.y * sdat-> pscaley;
				break;
			case ERT_PENV:
				obwor= current-> hit_n;
				M_selfnorm(obwor);
				mu= sdat-> pscalex * atan2(obwor.x,obwor.z) + sdat-> plat;
				mv= sdat-> pscaley * asin(obwor.y) + sdat-> plng;
				break;
		}
		*tu= mu, *tv= mv;
	}
		else { *tu= current-> paint_u, *tv= current-> paint_v; }
}

void	cv_texture(ev_surfbsi *sbi, er_surface *snew, int tmark, lung *thash)
{
	shurt
		i, j,
		k, kc, kmst, kl, klm;
	long iu, iv, lm;
	lung
		tash,
		tashm;
	realU
		dtmp,
		mu, mv, nu, nv,
		yu, yv,
		c1, c2, c3, ca, cea;
	double
		ska, skl, skd, sks, skr, skt, ski,
		ssgi, smtmp;
	vekU
		vupaxis,
		vnorm,
		vcrossn,
		vdownn;
	colU basecol, baseup, ambcol;
	ev_mapping *mapstep;
	ev_mapdat *evmstep;
	er_surface *sufp;
	
	sufp= sbi-> surfp;
	*snew= *sufp;
	tashm= 0;

	evmstep= sbi-> maplist;
	kl= 0, klm= 0,
	basecol= evmstep-> basemix, ambcol= evmstep-> ambmix;
	ska= evmstep-> cx_a,
	skl= evmstep-> cx_l,
	skd= evmstep-> cx_d,
	sks= evmstep-> cx_s,
	skr= evmstep-> cx_r,
	skt= evmstep-> cx_t,
	ski= evmstep-> cx_i,
	ssgi= sufp-> indx;

	for (i= sbi-> mapct; (i--); evmstep++)
	{
		kc= evmstep-> pjcode,
		k= kc & 0x0f;
		if (k) { cv_proj(evmstep, &mu, &mv); }
		else
		{
			if (!tmark) continue;
			yu= current-> paint_u, yv= current-> paint_v;
			mu= yu * evmstep-> matx.x + yv * evmstep-> matx.y + evmstep-> matw.x,
			mv= yu * evmstep-> maty.x + yv * evmstep-> maty.y + evmstep-> matw.y;
			}
	
		/****/
		for (j= evmstep-> mapct, mapstep= evmstep-> maplist; (j--); mapstep++)
		{
 			yu= mu, yv= mv;
#if 0
			if (k && (k != ERT_PSPHER) && (k != ERT_PSQCYLN))
			{
				if (k != ERT_PCYLN) yu*= mapstep-> tm_image-> imscalex;
				yv*= mapstep-> tm_image-> imscaley;
			}
#endif
			if (kc & (ERT_PTILE | ERT_PTILEMIR))
			{
			 	iu= floor(yu), iv= floor(yv);
			 	if (evmstep-> prepx) { if ((yu < 0.0) || (evmstep-> prepx <= iu)) { if (!k) break; else continue; } }
			 	if (evmstep-> prepy) { if ((yv < 0.0) || (evmstep-> prepy <= iv)) { if (!k) break; else continue; } }
				yu-= iu, yv-= iv;
				if (yu < 0.0) { yu+= 1.0; }
				if (yv < 0.0) { yv+= 1.0; }
			}

		    lm= mapstep-> mapt, kmst= mapstep-> mapstencil;
		    if (4 == lm)
			{
				texuv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arraybmp, &nu, &nv, &tash);
				if ((0.0 == nu) && (0.0 == nv)) continue;
				nu*= sufp-> bend, nv*= sufp-> bend;
				if (kmst)
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &ca, NULL);
					if (kmst & TRS_XSTENCIL) ca= 1.0 -ca;
					if (0.0 == ca) continue;
					if (ca < 1.0) { nu *= ca, nv *= ca; }
				}
				tashm= (tashm << 5) ^ tash;
				M_cpy(vnorm, current-> hit_n)
				if ((vnorm.x == 0.0) && (vnorm.z == 0.0)) continue;
		      	/* worry about top and bottom plates of cubic projection */
		      	/* real bumps in the night with uv textures */
				vupaxis= evmstep-> imaty;  M_selfnorm(vupaxis)
				M_cross(vcrossn, vnorm, vupaxis)  M_selfnorm(vcrossn)
				M_cross(vdownn, vnorm, vcrossn)

		      	/* handle mirrowing normal */
				M_plus(vnorm, nu * vcrossn)
				M_plus(vnorm, nv * vdownn)
				dtmp= M_selfdot(vnorm);
				if (dtmp > 0.0)
				{
					dtmp= 1.0 / sqrt(dtmp);
					M_cpy(current-> hit_n, dtmp * vnorm);
					}
				continue;
			}

			if (1 == lm)
			{
				texc_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayc, &c1, &c2, &c3, &tash);
				if (kmst)
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &ca, NULL);
					if (kmst & TRS_XSTENCIL) ca= 1.0 -ca;
					cea= 1.0 -ca;
				}
					else ca= 1.0;
				tashm= (tashm << 2) ^ tash;

#define	rvtASSIGN(ccc)	{ ccc.r= c1, ccc.g= c2, ccc.b= c3; }
#define rvtMIX(ccc, eee)	{ ccc.r= cea * eee.r + ca * c1, ccc.g= cea * eee.g + ca * c2, ccc.b= cea * eee.b + ca * c3; }

				lm= mapstep-> mapcode;
				if (lm & TRS_TCOLOR)
				{
			      	if ((ca == 1.0) || !(kmst & TRS_TCOLOR))  rvtASSIGN(basecol)
			      		else if (ca > 1e-3) { rvtMIX(baseup, basecol)  basecol= baseup; }
					kl= evmstep-> mapfin;
					if (lm & TRS_TMIX)
						{ C_mult(basecol, evmstep-> basemix) }
				}

				if (lm & TRS_TAMBIENT)
				{
					kl &= ~TRS_HAMBIENT;
					if ((ca == 1.0) || !(kmst & TRS_TAMBIENT)) rvtASSIGN(snew-> col_a)
						else if (0.0 != ca) rvtMIX(snew-> col_a, sufp-> col_a)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_a, evmstep-> cma) }
				}
				if (lm & TRS_TLUM)
				{
					kl &= ~TRS_HLUMEN;
					if ((ca == 1.0) || !(kmst & TRS_TLUM)) rvtASSIGN(snew-> col_l)
						else if (0.0 != ca) rvtMIX(snew-> col_l, sufp-> col_l)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_l, evmstep-> cml) }
				}
				if (lm & TRS_TDIFF)
				{
					kl &= ~TRS_HDIFF;
					if ((ca == 1.0) || !(kmst & TRS_TDIFF)) rvtASSIGN(snew-> col_d)
						else if (0.0 != ca) rvtMIX(snew-> col_d, sufp-> col_d)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_d, evmstep-> cmd) }
				}
				if (lm & TRS_TSPEC)
				{
					kl &= ~TRS_HSPEC;
					if ((ca == 1.0) || !(kmst & TRS_TSPEC)) rvtASSIGN(snew-> col_s)
						else if (0.0 != ca) rvtMIX(snew-> col_s, sufp-> col_s)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_s, evmstep-> cms) }
				}
				if (lm & TRS_TREFLECT)
				{
					kl &= ~TRS_HREFLECT;
					if ((ca == 1.0) || !(kmst & TRS_TREFLECT)) rvtASSIGN(snew-> col_r)
						else if (0.0 != ca) rvtMIX(snew-> col_r, sufp-> col_r)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_r, evmstep-> cmr) }
				}
				if (lm & TRS_TGLASS)
				{
					kl &= ~TRS_HGLASS;
					if ((ca == 1.0) || !(kmst & TRS_TGLASS)) rvtASSIGN(snew-> col_t)
						else if (0.0 != ca) rvtMIX(snew-> col_t, sufp-> col_t)
					if (lm & TRS_XGLASS)
					{
						snew-> col_t.r= 1.0 - snew-> col_t.r,
						snew-> col_t.g= 1.0 - snew-> col_t.g,
						snew-> col_t.b= 1.0 - snew-> col_t.b;
					}
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_t, evmstep-> cmt) }
				}
				if (lm & (TRS_TTRANS | TRS_TENVIR))
				{
					if ((ca == 1.0) || !(kmst & TRS_TTRANS)) rvtASSIGN(snew-> col_i)
						else if (0.0 != ca) rvtMIX(snew-> col_i, sufp-> col_i)
					if (lm & TRS_TMIX)
						{ C_mult(snew-> col_i, evmstep-> cmi) }
				}

				continue;
				}
			else
			if (2 & lm)
			{
				if (2 == lm)
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayv, &c1, &tash);
					if (kmst)
					{
						texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &ca, NULL);
						if (kmst & TRS_XSTENCIL) ca= 1.0 -ca;
						if ((kmst == lm) && (ca == 0.0)) continue;
						cea= 1.0 -ca;
						} else ca= 1.0;
					}
				else
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &c1, &tash);
					if (kmst) { ca= (kmst & TRS_XSTENCIL) ? (1.0 - c1) : c1, cea= 1.0 -ca; } else ca= 1.0;
					}
				lm= mapstep-> mapcode;
				klm |= (0x7f & (lm >> 20));
					
				if (lm & TRS_TAMBIENT)
				{
					kl |= TRS_HAMBIENT;
					if ((ca == 1.0) || !(kmst & TRS_TAMBIENT)) ska= c1;
						else if (0.0 != ca) { dtmp= ska, ska= dtmp * cea + c1 * ca; }
					}
				if (lm & TRS_TLUM)
				{
					kl |= TRS_HLUMEN;
					if ((ca == 1.0) || !(kmst & TRS_TLUM)) skl= c1;
						else if (0.0 != ca) { dtmp= skl, skl= dtmp * cea + c1 * ca; }
					}
				if (lm & TRS_TDIFF)
				{
					kl |= TRS_HDIFF;
					if ((ca == 1.0) || !(kmst & TRS_TDIFF)) skd= c1;
						else if (0.0 != ca) { dtmp= skd, skd= dtmp * cea + c1 * ca; }
					}
				if (lm & TRS_TSPEC)
				{
					kl |= TRS_HSPEC;
					if ((ca == 1.0) || !(kmst & TRS_TSPEC)) sks= c1;
						else if (0.0 != ca) { dtmp= sks, sks= dtmp * cea + c1 * ca; }
					}
				if (lm & TRS_TREFLECT)
				{
					kl |= TRS_HREFLECT;
					if ((ca == 1.0) || !(kmst & TRS_TREFLECT)) skr= c1;
						else if (0.0 != ca) { dtmp= skr, skr= dtmp * cea + c1 * ca; }
					}
				if (lm & TRS_TGLASS)
				{
					kl |= TRS_HGLASS;
					if ((ca == 1.0) || !(kmst & TRS_TGLASS)) skt= c1;
						else if (0.0 != ca) { dtmp= skt, skt= dtmp * cea + c1 * ca; }
					if (lm & TRS_XGLASS) { skt= ((0.0 < skt) && (skt < 1.0)) ? (1.0 -skt) : ((skt > 0.5) ? 0.0 : 1.0); }
					}
				if (lm & TRS_TTRANS)
				{
					if ((ca == 1.0) || !(kmst & TRS_TTRANS)) ski= c1;
						else if (0.0 != ca) { dtmp= ski, ski= dtmp * cea + c1 * ca; }
					}

				if ((lm & TRS_TGLOS) && (c1 < 1.0))
				{
					if (c1 < 1.0)
					{
						if (!(kmst & TRS_TTRANS)) { smtmp= c1; }
							else { smtmp= (ca > 0.0) ? cea + c1 * ca : 0.0; }
						snew-> spec= sufp-> spec * smtmp +1.0;
						}
					}
				if ((lm & TRS_TINDX) && (c1 < 1.0))
				{
					smtmp= (kmst & TRS_TTRANS) ? (cea + c1 * ca) : c1;
					if (smtmp < 1.0)
					{
						if (ssgi > 1.0)
						{
							dtmp= smtmp * (ssgi -1.0) +1.0;
							snew-> indx= dtmp, snew-> sqindx= dtmp * dtmp;
							smtmp= 1.0 / dtmp;
							snew-> rindx= smtmp, snew-> rqindx= smtmp * smtmp;
							}
						else
						{
							dtmp= smtmp * (sufp-> rindx -1.0) +1.0;
							snew-> rindx= dtmp, snew-> rqindx= dtmp * dtmp;
							smtmp= 1.0 / dtmp;
							snew-> indx= smtmp, snew-> sqindx= smtmp * smtmp;
							}
						}
					}
				if (lm & TRS_TALPHA)
				{
					dtmp= (kmst & TRS_TALPHA) ? (sufp-> alpha * cea + c1 * ca) : c1;
					if (!(lm & TRS_XALPHA)) { dtmp= 1.0 -dtmp; }
					snew-> alpha= dtmp;
					}
				tashm= (tashm << 2) ^ tash;
			}
		}
	}

	if (kl && klm) { kl &= ~klm; }
	if (kl)
	{
		if (kl & TRS_HAMBIENT)
		{
			snew-> col_a.r= ska * ambcol.r * basecol.r,
			snew-> col_a.g= ska * ambcol.g * basecol.g,
			snew-> col_a.b= ska * ambcol.b * basecol.b;
		}
		if (kl & TRS_HLUMEN) { C_cpy(snew-> col_l, skl * basecol) }
		if (kl & TRS_HDIFF) { C_cpy(snew-> col_d, skd * basecol) }
		if (kl & TRS_HSPEC) { C_cpy(snew-> col_s, sks * basecol) }
		if (kl & TRS_HREFLECT) { C_cpy(snew-> col_r, skr * basecol) }
		if (kl & TRS_HGLASS) { C_cpy(snew-> col_t, skt * basecol) }
	}
	if (klm)
	{
		evmstep= sbi-> maplist;
		if (klm & TRS_HLUMEN) { C_cpy(snew-> col_l, skl * evmstep-> cml) }
		if (klm & TRS_HDIFF) { C_cpy(snew-> col_d, skd * evmstep-> cmd) }
		if (klm & TRS_HSPEC) { C_cpy(snew-> col_s, sks * evmstep-> cms) }
		if (klm & TRS_HREFLECT) { C_cpy(snew-> col_r, skr * evmstep-> cmr) }
		if (klm & TRS_HGLASS) { C_cpy(snew-> col_t, skt * evmstep-> cmt) }
	}

	*thash ^= tashm;
}

void	cv_shadow(ev_surfbsi *sbi, er_surface *snew, int tmark, lung *thash)
{
	shurt
		i, j,
		k, kc, km, kmst, kl, klm;
	long
		iu, iv,
		lm;
	lung
		tash,
		tashm;
	realU
		dtmp,
		mu, mv,
		yu, yv,
		c1, c2, c3, ca, cea;
	double
		skt, ski,
		ssgi, smtmp;
	colU basecol, baseup;
	ev_mapping *mapstep;
	ev_mapdat *evmstep;
	er_surface *sufp;
	
	sufp= sbi-> surfp;
	*snew= *sufp;
	tashm= 0;

	evmstep= sbi-> maplist;
	kl= 0, basecol= evmstep-> basemix;
	skt= evmstep-> cx_t,
	ski= evmstep-> cx_i,
	ssgi= sufp-> indx;

	for (i= sbi-> mapct; (i--); evmstep++)
	{
		kc= evmstep-> pjcode,
		k= kc & 0x0f;
		if (k) { cv_proj(evmstep, &mu, &mv); }
		else
		{
			if (!tmark) continue;
			yu= current-> paint_u, yv= current-> paint_v;
			mu= yu * evmstep-> matx.x + yv * evmstep-> matx.y + evmstep-> matw.x,
			mv= yu * evmstep-> maty.x + yv * evmstep-> maty.y + evmstep-> matw.y;
			}
	
		for (j= evmstep-> mapct, mapstep= evmstep-> maplist; (j--); mapstep++)
		{
 			yu= mu, yv= mv;
			 if (kc & (ERT_PTILE | ERT_PTILEMIR))
			 {
			 	iu= floor(yu), iv= floor(yv);
			 	if (evmstep-> prepx) { if ((yu < 0.0) || (evmstep-> prepx <= iu)) { if (!k) break; else continue; } }
			 	if (evmstep-> prepy) { if ((yv < 0.0) || (evmstep-> prepy <= iv)) { if (!k) break; else continue; } }
				yu-= iu, yv-= iv;
				if (yu < 0.0) { yu+= 1.0; }
				if (yv < 0.0) { yv+= 1.0; }
				}

		      km= mapstep-> mapt, kmst= mapstep-> mapstencil, lm= mapstep-> mapcode;
		      if (!(lm & (TRS_TGLASS | TRS_TTRANS | TRS_TINDX)) && !((lm & TRS_TCOLOR) && (TRS_HGLASS & evmstep-> mapfin))) continue;

			if (1 == km)
			{
				texc_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayc, &c1, &c2, &c3, &tash);
				if (kmst)
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &ca, NULL);
					if (kmst & TRS_XSTENCIL) ca= 1.0 -ca;
					cea= 1.0 -ca;
					}
					else ca= 1.0;
				tashm= (tashm << 2) ^ tash;

#define	rvtASSIGN(ccc)	{ ccc.r= c1, ccc.g= c2, ccc.b= c3; }
#define rvtMIX(ccc, eee)	{ ccc.r= cea * eee.r + ca * c1, ccc.g= cea * eee.g + ca * c2, ccc.b= cea * eee.b + ca * c3; }

				if (lm & TRS_TGLASS)
				{
					kl &= ~TRS_HGLASS;
					if ((ca == 1.0) || !(kmst & TRS_TGLASS)) rvtASSIGN(snew-> col_t)
						else if (0.0 != ca) rvtMIX(snew-> col_t, sufp-> col_t)
					if (lm & TRS_XGLASS)
					{
						snew-> col_t.r= 1.0 - snew-> col_t.r,
						snew-> col_t.g= 1.0 - snew-> col_t.g,
						snew-> col_t.b= 1.0 - snew-> col_t.b;
						}
					}
				if (lm & TRS_TTRANS)
				{
					if ((ca == 1.0) || !(kmst & TRS_TTRANS)) rvtASSIGN(snew-> col_i)
						else if (0.0 != ca) rvtMIX(snew-> col_i, sufp-> col_i)
					}
			      if (lm & TRS_TCOLOR)
			      {
			      	if ((ca == 1.0) || !(kmst & TRS_TCOLOR))  rvtASSIGN(basecol)
			      		else if (ca > 1e-3) { rvtMIX(baseup, basecol)  basecol= baseup; }
					kl= evmstep-> mapfin;
					}

				continue;
				}
			else
			if (2 & km)
			{
				if (2 == km)
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayv, &c1, &tash);
					if (kmst)
					{
						texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &ca, NULL);
						if (kmst & TRS_XSTENCIL) ca= 1.0 -ca;
						if ((kmst == km) && (ca == 0.0)) continue;
						cea= 1.0 -ca;
						} else ca= 1.0;
					}
				else
				{
					texv_lookup(yu, yv, mapstep-> pixmapref, mapstep-> pixmapref-> arrayam, &c1, &tash);
					if (kmst) { ca= (kmst & TRS_XSTENCIL) ? (1.0 - c1) : c1, cea= 1.0 -ca; } else ca= 1.0;
					}

				klm= 0xf0 & (lm >> 20);
				if (lm & TRS_TGLASS)
				{
					kl |= TRS_HGLASS;
					if ((ca == 1.0) || !(kmst & TRS_TGLASS)) skt= c1;
						else if (0.0 != ca) { dtmp= skt, skt= dtmp * cea + c1 * ca; }
					if (lm & TRS_XGLASS) { skt= ((0.0 < skt) && (skt < 1.0)) ? (1.0 -skt) : ((skt > 0.5) ? 0.0 : 1.0); }
					}
				if (lm & TRS_TTRANS)
				{
					if ((ca == 1.0) || !(kmst & TRS_TTRANS)) ski= c1;
						else if (0.0 != ca) { dtmp= ski, ski= dtmp * cea + c1 * ca; }
					}

				if ((lm & TRS_TINDX) && (c1 < 1.0))
				{
					smtmp= (kmst & TRS_TTRANS) ? (cea + c1 * ca) : c1;
					if (smtmp < 1.0)
					{
						if (ssgi > 1.0)
						{
							dtmp= smtmp * (ssgi -1.0) +1.0;
							snew-> indx= dtmp, snew-> sqindx= dtmp * dtmp;
							smtmp= 1.0 / dtmp;
							snew-> rindx= smtmp, snew-> rqindx= smtmp * smtmp;
							}
						else
						{
							dtmp= smtmp * (sufp-> rindx -1.0) +1.0;
							snew-> rindx= dtmp, snew-> rqindx= dtmp * dtmp;
							smtmp= 1.0 / dtmp;
							snew-> indx= smtmp, snew-> sqindx= smtmp * smtmp;
							}
						}
					}
				tashm= (tashm << 2) ^ tash;
				}
			}
		}

	if (kl && klm) { kl &= ~klm; }
	if (kl & TRS_HGLASS) { C_cpy(snew-> col_t, skt * basecol) }
	if (klm)
	{
		evmstep= sbi-> maplist;
		if (klm & TRS_HGLASS) { C_cpy(snew-> col_t, skt * evmstep-> cmt) }
	}

	*thash ^= tashm;
	}
