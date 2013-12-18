#include "rrun.h"
#include "rvytable.h"

thing CPmapitem
{
	char name[128];
	ev_smap rec;
	struct CPmapitem *next;
} cp_mapitem;

int				mapdirection= 0 ;
cp_mapitem	*imaplist= NULL;

extern FILE   *cto_fileopen(char *);
	/** i_vid */
extern int vid_header(char *, ehdr_image *) ;
extern int	vid_scanline(ehdr_image *, long *) ;
	/** i_targa */
extern int	tga_headerf(ehdr_image *, int allocate);
extern int	tga_scanline(ehdr_image *, unsigned long *);


void	cpmap_release(void), cpmap_destroy(void);

void	cpmap_start(void)
{
	if (! imaplist) return;

	cpmap_release();
}

#define	ZEM(zz)	bzero(& (zz-> rec), sizeof(ev_smap))

ev_smap	*cpmap_lookup(char *name, int use)
{
	char *p;
	ehdr_image bim;
	cp_mapitem *cstep;

	if (! use) return NULL;

	for (cstep= imaplist; (cstep); cstep= cstep-> next)
	{
		if (! strcmp(cstep-> name, name))
		{
			cstep-> rec.marked |= use;
			return & (cstep-> rec);
		}
	}

	p= strchr(name, '.');  if (p) p ++;
	if (! strcmp(p, "vid") || ! strcmp(p, "vZ") || ! strcmp(p, "vz"))
	{
		if (vid_header(name, &bim)) return NULL;
#ifdef RAELUNIK
		if (p && ('v' == p[0]) && (('z' == p[1]) || ('Z' == p[1])))
			pclose(bim.fp); else
#endif
			fclose(bim.fp);
		EFREE(bim.data);
	}
	else
	{
		bim.fp= cto_fileopen(name);
		if (! bim.fp) return NULL;
		if (tga_headerf(&bim, 0)) return NULL;
		fclose(bim.fp);
	}
	if (! bim.width || ! bim.height)
	{
		printf(" >> texture map loaded with zero dimension [%d x %d].\n",
			bim.width, bim.height);
		return NULL;
	}

	cstep= EMEM(1, cp_mapitem);
	if (NULL == cstep) return NULL;
	ZEM(cstep);

	strcpy(cstep-> name, name);
	cstep-> rec.marked= use;
	cstep-> rec.imw= bim.width, cstep-> rec.imwp= bim.width +1,
	cstep-> rec.imh= bim.height, cstep-> rec.imhp= bim.height +1;
	cstep-> rec.iscax= 1.0 / (bim.width +0.5),
	cstep-> rec.iscay= 1.0 / (bim.height +0.5);
	cstep-> rec.array_base= NULL;
	cstep-> next= imaplist, imaplist= cstep;

	return & (cstep-> rec);
}

static int	cpl_vid(cp_mapitem *mdt, long **im)
{
	int istep,
		(* tclose)();
	char *p;
	ehdr_image bim;
	long *lrow;

	if (vid_header(mdt-> name, &bim)) RE
	p= strchr(mdt-> name, '.');
	tclose= fclose;
#ifdef RAELUNIK
	if (p) { p ++;  if (('v' == p[0]) && (('z' == p[1]) || ('Z' == p[1]))) tclose= pclose; }
#endif
	for (istep= bim.height, lrow= (long *) bim.data; (istep --); )
	{
		if (vid_scanline(&bim, lrow)) { (* tclose)(bim.fp);  EFREE(bim.data);  RE }
		lrow += bim.width;
	}

	(* tclose)(bim.fp);
	*im= (long *) bim.data;
	return 0;
}

static int	cpl_tga(cp_mapitem *mdt, long **im)
{
	int istep;
	ehdr_image bim;
	long *lrow;

	bim.fp= cto_fileopen(mdt-> name);
	if (! bim.fp || tga_headerf(& bim, 1)) RE

	if (mapdirection || (1 == bim.cur_y))
	{
		for (istep= bim.height, lrow= (long *) bim.data; (istep --); )
		{
			if (tga_scanline(&bim, lrow)) { fclose(bim.fp);  EFREE(bim.data);  RE }
			lrow += bim.width;
		}
	}
	else
	{
		for (istep= bim.height, lrow= (long *) (bim.data + (bim.height * bim.width)); (istep --); )
		{
			lrow -= bim.width;
			if (tga_scanline(&bim, lrow)) { fclose(bim.fp);  EFREE(bim.data);  RE }
		}
	}

	fclose(bim.fp);
	*im= (long *) bim.data;
	return 0;
}

static void	cpl_put(cp_mapitem *mdt, long *im, int opt)
{
	int istep, jstep;
	long iv, iy;
	byte
		*bfirstc, *bstepc,
		*bfirstv, *bstepv,
		*bfirsta, *bstepa;

	bstepc= mdt-> rec.arrayc,
	bstepv= mdt-> rec.arrayv,
	bstepa= mdt-> rec.arrayam;

	switch (opt)
	{
		case EMP_COLOR:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstc= bstepc; (jstep --); )
				{
					iv= *(im ++);
					*(bstepc ++)= iv & 0xff,
					*(bstepc ++)= (iv >> 8) & 0xff,
					*(bstepc ++)= (iv >> 16) & 0xff;
				}

				*(bstepc ++)= bfirstc[0],
				*(bstepc ++)= bfirstc[1],
				*(bstepc ++)= bfirstc[2];
			}
			break;
		case EMP_COLOR | EMP_A:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstc= bstepc, bfirsta= bstepa; (jstep --); )
				{
					iv= *(im ++);
					*(bstepc ++)= iv & 0xff,
					*(bstepc ++)= (iv >> 8) & 0xff,
					*(bstepc ++)= (iv >> 16) & 0xff;
					*(bstepa ++)= iv >> 24;
				}

				*(bstepc ++)= bfirstc[0],
				*(bstepc ++)= bfirstc[1],
				*(bstepc ++)= bfirstc[2];
				*(bstepa ++)= bfirsta[0];
			}
			break;
		case EMP_V:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstv= bstepv; (jstep --); )
				{
					iv= *(im ++);
					iy= y_red[(iv >> 16) & 0xff] + y_green[(iv >> 8) & 0xff] + y_blue[iv & 0xff];
					*(bstepv ++)= (iy >> 16) & 0xff;
				}
				*(bstepv ++)= bfirstv[0];
			}
			break;
		case EMP_V | EMP_A:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstv= bstepv, bfirsta= bstepa; (jstep --); )
				{
					iv= *(im ++);
					iy= y_red[(iv >> 16) & 0xff] + y_green[(iv >> 8) & 0xff] + y_blue[iv & 0xff];
					*(bstepv ++)= (iy >> 16) & 0xff;
					*(bstepa ++)= iv >> 24;
				}
				*(bstepv ++)= bfirstv[0];
				*(bstepa ++)= bfirsta[0];
			}
			break;
		case EMP_COLOR | EMP_V:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstc= bstepc, bfirstv= bstepv; (jstep --); )
				{
					iv= *(im ++);
					iy= y_red[(iv >> 16) & 0xff] + y_green[(iv >> 8) & 0xff] + y_blue[iv & 0xff];
					*(bstepc ++)= iv & 0xff,
					*(bstepc ++)= (iv >> 8) & 0xff,
					*(bstepc ++)= (iv >> 16) & 0xff;
					*(bstepv ++)= (iy >> 16) & 0xff;
				}

				*(bstepc ++)= bfirstc[0],
				*(bstepc ++)= bfirstc[1],
				*(bstepc ++)= bfirstc[2];
				*(bstepv ++)= bfirstv[0];
			}
			break;
		case EMP_COLOR | EMP_V | EMP_A:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirstc= bstepc, bfirstv= bstepv, bfirsta= bstepa;
					(jstep --); )
				{
					iv= *(im ++);
					iy= y_red[(iv >> 16) & 0xff] + y_green[(iv >> 8) & 0xff] + y_blue[iv & 0xff];
					*(bstepc ++)= iv & 0xff,
					*(bstepc ++)= (iv >> 8) & 0xff,
					*(bstepc ++)= (iv >> 16) & 0xff;
					*(bstepv ++)= (iy >> 16) & 0xff;
					*(bstepa ++)= iv >> 24;
				}

				*(bstepc ++)= bfirstc[0],
				*(bstepc ++)= bfirstc[1],
				*(bstepc ++)= bfirstc[2];
				*(bstepv ++)= bfirstv[0];
				*(bstepa ++)= bfirsta[0];
			}
			break;
		case EMP_A:
			for (istep= mdt-> rec.imh; (istep --); )
			{
				for (jstep= mdt-> rec.imw, bfirsta= bstepa; (jstep --); )
				{
					iv= *(im ++);
					*(bstepa ++)= iv >> 24;
				}
				*(bstepa ++)= bfirsta[0];
			}
			break;
	}
}

static void	cpl_grad(cp_mapitem *mdt, byte *src)
{
	unsigned char
		*imstepf, *bholdptr,
		*imsteplow, *imstepcent, *imstepabv,
		*imholdabv, *bmap;
	short delth, iva, ivb, ivc, ive, ivf;
	unsigned short
		istep, icolstep, imaxx;

	if (mdt-> rec.imh < 1) return;
	imaxx= mdt-> rec.imwp;
	bmap= mdt-> rec.arraybmp;

	if (mdt-> rec.imh == 1)
	{
		if (mdt-> rec.imw == 1) { bmap[0]= bmap[1]= 0;  return; }
		iva= src[mdt-> rec.imw -1];
		imstepf= src;
		ivb= *(imstepf++),
		ivc= *(imstepf++);
		bholdptr= bmap;
		for (istep= mdt-> rec.imw; (istep --); iva= ivb, ivb= ivc, ivc= *(imstepf++))
			{ delth= ivc -iva;  *(bmap++)= delth >> 1, *(bmap++)= 0; }
		*(bmap ++)= *(bholdptr ++), *bmap= 0;
		return;
		}

	imsteplow= src + (imaxx * (mdt-> rec.imh -1)), imstepcent= src, imstepabv= src + imaxx;
	for (icolstep= mdt-> rec.imh; (icolstep --);
		imsteplow= imstepcent, imstepcent= imholdabv, imstepabv ++)
	{
		if (!icolstep) { imstepabv= src; }  imholdabv= imstepabv;
		iva= imstepcent[mdt-> rec.imw -1];
		imstepf= imstepcent, ivb= *(imstepf ++), ivc= *(imstepf ++);
		for (istep= mdt-> rec.imw; (istep --); iva= ivb, ivb= ivc, ivc= *(imstepf++))
		{
			delth= ivc -iva;  *(bmap ++)= delth >> 1;
			ive= *(imsteplow ++), ivf= *(imstepabv ++);
			delth= ivf -ive;  *(bmap ++)= delth >> 1;
		}
	}
}

int	cpmap_load(void)
{
	char *p;
	byte *imdata, *imtmp;
	long
		pneed, tneed, mneed,
		lsz, ldepth,
		*lbuf;
	cp_mapitem *cstep;

	for (cstep= imaplist; (cstep); cstep= cstep-> next)
	{
		pneed= cstep-> rec.marked;
		if (! pneed) continue;

				// check if already loaded
		if (cstep-> rec.array_base)
			continue ;

		tneed= pneed;
		if (pneed & EMP_AB) tneed |= EMP_A;
			else if (pneed & EMP_BMP) tneed |= EMP_V;
		mneed= tneed - pneed;
		lsz= cstep-> rec.imwp * cstep-> rec.imh;
		if (! lsz)
			{ ZEM(cstep);  continue; }

		ldepth= 0;
		if (pneed & EMP_COLOR) ldepth += 3;
		if (pneed & EMP_V) ldepth ++;
		if (pneed & EMP_A) ldepth ++;
		if (pneed & EMP_BMP) ldepth += 2;

		imdata= EMEM((lsz * ldepth), byte);
		if (NULL == imdata)
			{ ZEM(cstep);  continue; }

		cstep-> rec.array_base= imdata;
		if (pneed & EMP_COLOR) { cstep-> rec.arrayc= imdata, imdata += (3 * lsz); }
		if (pneed & EMP_V) { cstep-> rec.arrayv= imdata, imdata += lsz; }
		if (pneed & EMP_A) { cstep-> rec.arrayam= imdata, imdata += lsz; }
		if (pneed & EMP_BMP) { cstep-> rec.arraybmp= imdata; }

			/*** load original */
		p= strchr(cstep-> name, '.');  if (p) p ++;
		if (! strcmp(p, "vid") || ! strcmp(p, "vZ") || ! strcmp(p, "vz"))
		{
			if (cpl_vid(cstep, &lbuf))
				{ EFREE(cstep-> rec.array_base);  ZEM(cstep);  continue; }
		}
		else
		{
			if (cpl_tga(cstep, &lbuf))
				{ EFREE(cstep-> rec.array_base);  ZEM(cstep);  continue; }
		}

			/*** convert to primary */
		if (mneed)
		{
			ldepth= 0;
			imtmp= EMEM(lsz, byte);
			if (NULL == imtmp)
				{ EFREE(cstep-> rec.array_base);  ZEM(cstep);  continue; }
			if (mneed & EMP_V) cstep-> rec.arrayv= imtmp;
				else if (mneed & EMP_A) cstep-> rec.arrayam= imtmp;
				else { EFREE(imtmp);  mneed= 0; }
		}
		cpl_put(cstep, lbuf, tneed & 0xff);
		EFREE(lbuf);

		/*** create bump */
		if (pneed & EMP_BMP)
			cpl_grad(cstep, (pneed & EMP_AB) ? cstep-> rec.arrayam : cstep-> rec.arrayv);

		if (mneed)
		{
			if (mneed & EMP_V) cstep-> rec.arrayv= NULL;
				else if (mneed & EMP_A) cstep-> rec.arrayam= NULL;
			EFREE(imtmp);
			imtmp= NULL, mneed= 0;
		}
	}

	return 0;
}

void	cpmap_release(void)
{
	cp_mapitem *cstep;

	for (cstep= imaplist; (cstep); cstep= cstep-> next)
	{
		if (! cstep-> rec.marked) continue;

		EFREE(cstep-> rec.array_base);
		cstep-> rec.marked= 0,
		cstep-> rec.array_base= NULL,
		cstep-> rec.arrayv= NULL,
		cstep-> rec.arrayc= NULL,
		cstep-> rec.arrayam= NULL,
		cstep-> rec.arraybmp= NULL;
	}
}

void	cpmap_destroy(void)
{
	cp_mapitem *cstep, *cfor;

	for (cstep= imaplist; (cstep); cstep= cfor)
	{
		cfor= cstep-> next;
		EUFREE(cstep-> rec.array_base) ;
		EFREE(cstep);
	}

	imaplist= NULL;
}

