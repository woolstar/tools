#include "basic.h"
#include "tscene.h"
#include "eart.h"
#include "epart_word.h"

extern int		epart_holdc;
extern char		epart_hold[SLINE];

extern unsigned int	eactm_map, eactm_mimage;

static	tsc_map	*usemapping;
static	tsc_texture	*usetexture;

void	epartsu_installmap(tsc_map *p1, tsc_texture *p2)
{
	usemapping= p1,
	usetexture= p2;
}

static int	epm_checkmap(void)
{
	int unset;
	long i, av;

	unset= 1;
	while (SCAN)
	{
		if (3 != i) RE
		if (_SURF_MAP == av)
		{
			hcSCAN;
			if isTOK(_SURF_MIX) { hcSCAN; }
			switch (av)
			{
				case _COLOR:
				case _AMBIENT:
				case _SURF_LUMINANT:
				case _SURF_DIFFUSE:
				case _SURF_SPECULAR:
				case _SURF_REFLECT:
				case _SURF_GLASS:
				case _SURF_KA:
				case _SURF_KL:
				case _SURF_KD:
				case _SURF_KS:
				case _SURF_KR:
				case _SURF_KT:
				case _SURF_ALPHA:
				case _SURF_GLOSS:
				case _SURF_INDEX:
				case _SURF_BUMP:
				case _SURF_RENVIR:
					break;
				default:
					RE
			}
			hcSCAN;
			if (! isWORD) RE
			hcUSE
			eactm_mimage ++;
			unset= 0;
		}
		else
		if (_SURF_PROJECT == av)
		{
			hcSCAN;
			switch (av)
			{
				case _SURF_CUBIC:
				case _SURF_CYLNDER:
				case _SURF_PLANAR:
				case _SURF_SPHERE:
				case _SURF_SQUARECYL:
					break;
				default:
					RE
			}
			hcUSE
			if ( epart_checkval(10)) RE
		}
		else
		if (_SURF_WRAP == av)
		{
			hcSCAN;
			switch (av)
			{
				case _SURF_NONE:
				case _SURF_NORMAL:
				case _SURF_MIRROR:
					hcUSE
					break;
			}
			if ( epart_checkval(4)) RE
		}
		else break;
	}

	if (unset) RE

	eactm_map ++;
	return 0;
}

static void	epm_initmap(tsc_map *ptr)
{
	bzero(ptr, sizeof(tsc_map));
	M_load(ptr-> scale, 1.0)
}

static int	epm_rulemap(tscene_surface *container)
{
	int unset,
		mpcode, mpmodif, mpdepth;
	long i, av;
	tsc_map *curmap;

	unset= 1;
	curmap= usemapping;
	epm_initmap(curmap);

	while (SCAN)
	{
		if (3 != i) RE
		if (_SURF_MAP == av)
		{
			mpcode= 0,
			mpmodif= 0,
			mpdepth= 0;

			hcSCAN;
			if isTOK(_SURF_MIX) { mpmodif= TRS_TMIX;  hcSCAN; }
			switch (av)
			{
				case _COLOR:	mpcode= TRS_TCOLOR, mpdepth= 3; break;
				case _AMBIENT:	mpcode= TRS_TAMBIENT, mpdepth= 3; break;
				case _SURF_LUMINANT:	mpcode= TRS_TLUM, mpdepth= 3; break;
				case _SURF_DIFFUSE:	mpcode= TRS_TDIFF, mpdepth= 3; break;
				case _SURF_SPECULAR:	mpcode= TRS_TSPEC, mpdepth= 3; break;
				case _SURF_REFLECT:	mpcode= TRS_TREFLECT, mpdepth= 3; break;
				case _SURF_GLASS:	mpcode= TRS_TGLASS, mpdepth= 3; break;
				case _SURF_KA:	mpcode= TRS_TAMBIENT, mpdepth= 1; break;
				case _SURF_KL:	mpcode= TRS_TLUM, mpdepth= 1; break;
				case _SURF_KD:	mpcode= TRS_TDIFF, mpdepth= 1; break;
				case _SURF_KS:	mpcode= TRS_TSPEC, mpdepth= 1; break;
				case _SURF_KR:	mpcode= TRS_TREFLECT, mpdepth= 1; break;
				case _SURF_KT:	mpcode= TRS_TGLASS, mpdepth= 1; break;
				case _SURF_ALPHA:	mpcode= TRS_TALPHA, mpdepth= 1; break;
				case _SURF_GLOSS:	mpcode= TRS_TGLOS, mpdepth= 1; break;
				case _SURF_INDEX:	mpcode= TRS_TINDX, mpdepth= 1; break;
				case _SURF_BUMP:	mpcode= TRS_TBUMP, mpdepth= 2; break;
				case _SURF_RENVIR:
					mpcode= TRS_TENVIR, mpdepth= 3; break;
				default:
					RE
			}
			hcSCAN;
			if (! isWORD) RE
			hcUSE
			usetexture-> lapply= mpcode | mpmodif,
			usetexture-> lset= mpdepth;
			strcpy(usetexture-> name, epart_hold);
			if (unset)
				{ curmap-> imagelist= usetexture, unset= 0; }
			curmap-> mapcount ++,
			usetexture ++;
		}
		else
		if (_SURF_PROJECT == av)
		{
			mpcode= 0;
			hcSCAN;
			switch (av)
			{
				case _SURF_CUBIC:	mpcode= 2; break;
				case _SURF_CYLNDER:	mpcode= 3; break;
				case _SURF_PLANAR:	mpcode= 1; break;
				case _SURF_SPHERE:	mpcode= 5; break;
				case _SURF_SQUARECYL:	mpcode= 4; break;
				default:
					RE
			}
			hcSCAN;	curmap-> scale.x= AHOLDF,
			hcSCAN;	curmap-> scale.y= AHOLDF,
			hcSCAN;	curmap-> scale.z= AHOLDF;
			hcSCAN;	curmap-> offset.x= AHOLDF,
			hcSCAN;	curmap-> offset.y= AHOLDF,
			hcSCAN;	curmap-> offset.z= AHOLDF;
			hcSCAN;	curmap-> postsx= AHOLDF,
			hcSCAN;	curmap-> postsy= AHOLDF;
			hcSCAN;	curmap-> postx= AHOLDF,
			hcSCAN;	curmap-> posty= AHOLDF;
			curmap-> projcode= (curmap-> projcode & 0xff00) + mpcode;
			hcUSE
		}
		else
		if (_SURF_WRAP == av)
		{
			hcSCAN;
			switch (av)
			{
				case _SURF_NONE:
				case _SURF_NORMAL:
				case _SURF_MIRROR:
					hcUSE
					break;
			}
			if ( epart_checkval(4)) RE
		}
		else break;
	}

	if (unset) RE
	if (! container-> mapcount)
		{ container-> maplist= curmap; }
	container-> mapcount ++,
	usemapping ++;
	return 0;
}

static int	epm_checksolid(void)
{
	RE
}

int	epartck_material(void)
{
	long i, av;

	while (SCAN)
	{
		if (3 != i) break;
		switch (av)
		{
			case _COLOR:
			case _AMBIENT:
			case _SURF_LUMINANT:
			case _SURF_DIFFUSE:
			case _SURF_SPECULAR:
			case _SURF_REFLECT:
			case _SURF_GLASS:
				if ( epart_checkval(3)) RE
				break;
			case _SURF_MIXGLOSS:
				if ( epart_checkval(2)) RE
				break;
			case _SURF_KA:
			case _SURF_KL:
			case _SURF_KD:
			case _SURF_KS:
			case _SURF_KR:
			case _SURF_KT:
			case _SURF_ALPHA:
			case _SURF_ALPHALITE:
			case _SURF_GLOSS:
			case _SURF_INDEX:
				if ( epart_checkval(1)) RE
				break;
			case _INVIS:
				hcUSE
				break;
			case _SURF_TEXTURE:
				hcUSE
				if ( epm_checkmap()) RE
				break;
			case _SOLID:
				if (epm_checksolid()) RE
				break;

			case _SURFACE:
			case _SCENE_VIEW:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				return 0;
			default:
				RE
		}
	}
	return 0;
}

#define	hcCOLOR		hcSCAN; fa= AHOLDF;  hcSCAN; fb= AHOLDF;  hcSCAN; fc= AHOLDF; hcUSE
#define Cset(xx)	xx.r= fa, xx.g= fb, xx.b= fc;
#define	Cvalid		if ((fa >= 0.0) && (fb >= 0.0) && (fc >= 0.0) && ((fa+fb+fc) > 0.0))
#define Cprop(xx, yy)	hcCOLOR  Cvalid { Cset(xx)  lbit |= yy; }
#define Kprop(xx, yy)	hcSCAN; fa= AHOLDF; hcUSE if (fa > 0.0) { xx= fa;  lbit |= yy, llink |= yy; }
#define Xprop(xx, yy)	hcSCAN; fa= AHOLDF; hcUSE if (fa >= 0.0) { xx= fa;  lbit |= yy; }

int	epartru_material(tscene_surface *dest)
{
	long i, av,
		lbit, llink, lbase;
	float fa, fb, fc;

	lbit= llink= lbase= 0;
	dest-> mapcount= 0,
	dest-> maplist= NULL;

	while (SCAN)
	{
		if (3 != i) break;
		switch (av)
		{
			case _COLOR:
				hcCOLOR
				if ((fa >= 0.0) && (fb >= 0.0) && (fc >= 0.0) && ((fa+fb+fc) > 0.0))
				{
					Cset(dest-> base)
					lbase= 1;
				}
				break;

			case _AMBIENT:			Cprop(dest-> ca, TRS_HAMBIENT) break;
			case _SURF_LUMINANT:	Cprop(dest-> cl, TRS_HLUMEN) break;
			case _SURF_DIFFUSE:		Cprop(dest-> cd, TRS_HDIFF) break;
			case _SURF_SPECULAR:	Cprop(dest-> cs, TRS_HSPEC) break;
			case _SURF_REFLECT:		Cprop(dest-> cr, TRS_HREFLECT) break;
			case _SURF_GLASS:		Cprop(dest-> ct, TRS_HGLASS) break;
			case _SURF_MIXGLOSS:
				if ( epart_checkval(2)) RE
				break;
			case _SURF_KA:	Kprop(dest-> ka, TRS_HAMBIENT) break;
			case _SURF_KL:	Kprop(dest-> kl, TRS_HLUMEN) break;
			case _SURF_KD:	Kprop(dest-> kd, TRS_HDIFF) break;
			case _SURF_KS:	Kprop(dest-> ks, TRS_HSPEC) break;
			case _SURF_KR:	Kprop(dest-> kr, TRS_HREFLECT) break;
			case _SURF_KT:	Kprop(dest-> kt, TRS_HGLASS) break;
			case _SURF_ALPHA:
				Xprop(dest-> alpha, TRS_ALPHA)
				break;
			case _SURF_ALPHALITE:
				Xprop(dest-> alpha, TRS_ALPHALITE)
				break;
			case _SURF_GLOSS:
				hcSCAN;  fa= AHOLDF;
				hcUSE
				if (fa > 0.0) { dest-> spec= fa; }
				break;
			case _SURF_INDEX:
				hcSCAN;  fa= AHOLDF;
				hcUSE
				if (fa > 0.0) { dest-> indx= fa; }
				break;
			case _INVIS:
				hcUSE
				lbit |= TRS_INVIS;
				break;
			case _SURF_TEXTURE:
				hcUSE
				if ( epm_rulemap(dest)) RE
				break;
			case _SURF_BUMPY:
				Xprop(dest-> bend, TRS_BUMPY)
				break;
			case _SOLID:
				if (epm_checksolid()) RE
				break;

			default:
				goto Lartsurfrulefin;
		}
	}

Lartsurfrulefin:
	if (llink && ! lbase) RE
	dest-> attr= lbit, dest-> amix= llink;
	return 0;
}

#define	PT(eee)	fprintf(fwrite, eee)
#define PTA(uuu,vvv) if (3 == tpicmap-> lset) { PT(uuu); } else { PT(vvv); }
static int	es_printtex(tsc_map * ref, FILE * fwrite)
{
	unsigned int istep;
	long lc;
	tsc_texture * tpicmap;

	for (istep= ref-> mapcount, tpicmap= ref-> imagelist; (istep --); tpicmap ++)
	{
		fprintf(fwrite, "\t\t\tmap ");
		lc= tpicmap-> lapply;
		if (lc & TRS_TMIX)
		{
			fprintf(fwrite, "mix ");
			lc &= ~ TRS_TMIX;
		}
		switch (lc)
		{
			case TRS_TCOLOR :	PT("color");  break;
			case TRS_TAMBIENT:  PTA("ambient", "ka");  break;
			case TRS_TLUM:  PTA("luminant", "kl");  break;
			case TRS_TDIFF:  PTA("diffuse", "kd");  break;
			case TRS_TSPEC:  PTA("specular", "ks");  break;
			case TRS_TREFLECT:  PTA("reflect", "kr");  break;
			case TRS_TGLASS:  PTA("glass", "kt");  break;

			case TRS_TALPHA :	PT("alpha");  break;
			case TRS_TGLOS :	PT("gloss");  break;
			case TRS_TINDX :	PT("index");  break;
			case TRS_TBUMP :	PT("bump");  break;
			case TRS_TENVIR :	PT("envir");  break;
		}
		fprintf(fwrite, " %s\n", tpicmap-> name);
	}

	switch (ref-> projcode)
	{
		case 1:	PT("\t\t\tproject planar");  break ;
		case 2:	PT("\t\t\tproject cubic");  break ;
		case 3:	PT("\t\t\tproject cylnder");  break ;
		case 4:	PT("\t\t\tproject squarecyl");  break ;
		case 5:	PT("\t\t\tproject sphere");  break ;
		default:
			return 0;
	}

	fprintf(fwrite, "%g %g %g %g %g %g ", M_expand(ref->scale), M_expand(ref->offset));
	fprintf(fwrite, "%g %g ", ref->postsx, ref->postsy);
	fprintf(fwrite, "%g %g\n", ref->postx, ref->posty);

	return 0;
}

#define	PC(nnn,vvv)	fprintf(fwrite, "\t\t%s %g %g %g\n", nnn, C_expandl(vvv))
#define PK(nnn, vvv) fprintf(fwrite, "\t\t%s %g\n", nnn, vvv)
int	epart_printsurf(tscene_surface * ref, FILE * fwrite)
{
	int istep, iret;
	long ke, ks, kc;
	tsc_map * mapstep;

	ke= ref-> attr, kc= ref-> amix;
	ks = ke & ~ kc;

	fprintf(fwrite, "\tsurface %s\n", ref-> sname);
	if (kc) { PC("color", ref-> base); }
	if (kc & TRS_HAMBIENT) { PK("ka", ref-> ka); }
	if (kc & TRS_HLUMEN) { PK("kl", ref-> kl); }
	if (kc & TRS_HDIFF) { PK("kd", ref-> kd); }
	if (kc & TRS_HSPEC) { PK("ks", ref-> ks); }
	if (kc & TRS_HREFLECT) { PK("kr", ref-> kr); }
	if (kc & TRS_HGLASS) { PK("kt", ref-> kt); }

	if (ks & TRS_HAMBIENT) { PC("ambient", ref-> ca); }
	if (ks & TRS_HLUMEN) { PC("luminant", ref-> cl); }
	if (ks & TRS_HDIFF) { PC("diffuse", ref-> cd); }
	if (ks & TRS_HSPEC) { PC("specular", ref-> cs); }
	if (ks & TRS_HREFLECT) { PC("reflect", ref-> cr); }
	if (ks & TRS_HGLASS) { PC("glass", ref-> ct); }

	if (ke & TRS_HSPEC) { fprintf(fwrite, "\t\tgloss %g\n", ref-> spec); }
	if (ke & TRS_HGLASS) { fprintf(fwrite, "\t\tindex %g\n", ref-> indx); }

	if (ke & TRS_ALPHA) { fprintf(fwrite, "\t\talpha %g\n", ref-> alpha); }
		else if (ke & TRS_ALPHALITE) { fprintf(fwrite, "\t\talphalite %g\n", ref-> alpha); }
	if (ke & TRS_INVIS) { fprintf(fwrite, "\t\tinvis\n"); }
	if (ke & TRS_BUMPY) { fprintf(fwrite, "\tbumpy %g\n", ref-> bend); }

	if (ref-> mapcount && ref-> maplist)
	{
		for (istep= ref-> mapcount, mapstep= ref-> maplist; (istep --); mapstep ++)
		{
			fprintf(fwrite, "\t\ttexture\n");
			iret= es_printtex(mapstep, fwrite);
				if (iret) return iret;
		}
	}

	return 0;
}
