#include "basic.h"
#include "escene.h"
#include "tfren.h"
#include "ebf.h"

	/*************/

extern bf_flat	*x_initial;

	/*************/

char	globaltstring[1024];

	/*************/

static void	tb_expandv(vector *dest, float *src, int n)
{
	while (n --)
	{
		dest-> x= *(src ++),
		dest-> y= *(src ++),
		dest-> z= *(src ++);
		dest ++;
	}
}
static void	tb_compressv(float *dest, vector *src, int n)
{
	while (n --)
	{
		*(dest ++)= src-> x,
		*(dest ++)= src-> y,
		*(dest ++)= src-> z;
		src ++;
	}
}

static void	tb_expandsurf(tro_surface *dest, bf_surface *src, int n)
{
	bzero(dest, n * sizeof(tro_surface));
	for ( ;(n --); src ++, dest ++)
	{
		strcpy(dest-> name, src-> name);
		dest-> base.r= src-> r,
		dest-> base.g= src-> g,
		dest-> base.b= src-> b;
	}
}
static void	tf_compresssurf(bf_surface *dest, tro_surface *src, int n)
{
	for ( ;(n --); src ++, dest ++)
	{
		strcpy(dest-> name, src-> name);
		dest-> r= src-> base.r,
		dest-> g= src-> base.g,
		dest-> b= src-> base.b;
	}
}

#define	LNUM	*(list ++), n --

static long	tb_sizeface(unsigned short *list, int n, vector *v)
{
	int plt, plsrf, plsize,
		kset, ksize, ksurf;
	long ln ;

	ln= 0, ksurf= -1;
	while (n)
	{
		if (n < 6) { printf(" >> illegal remainder.\n");  break; }
		plt= LNUM, plsrf= LNUM, plsize= LNUM;
		if (! plsize || (plsize < 3))
			{ printf(" >> zero poly.\n");  RE }
		switch (plt)
		{
			case 0: kset= 0, ksize= plsize; break;
			case 1: kset= 1, ksize= 2 * plsize; break;
			case 3: kset= 2, ksize= 2 * plsize; break;
			case 5: kset= 3, ksize= 3 * plsize; break;
			default:
				printf(" >> unknown poly code %d\n", plt);
				RE
		}
		if (ksize > n)
			{ printf(" >> poly missing data.\n");  RE }
		if (plsrf != ksurf)
			{ ln ++, ksurf= plsrf; }
		if (3 == plsize)
			{ ln += 1 + 3 * (1 + kset); }
		else
		if (4 == plsize)
			{ if (kset) { ln += 2 + 6 * (1 + kset); } else { ln += 5; } }
		else
		{
			ln += (kset +1) * epbf_sizeface(list, plsize, v);
		}
		list += ksize, n -= ksize;
		if (! kset || (2 == kset)) { list ++, n --; }
	}

	return ln;
}

#define	IFn	if (kset & EPB_FACTN)
#define	IFt	if (kset & EPB_FACTT)

static void	tb_lookupuv(int pt, float *set, us *du, us *dv)
{
	long itmp;

	if (NULL == set)
		{ *du= *dv= 0;  return; }
	set += (pt -1) * 2;
	itmp= 32768L * *(set ++);
	if (itmp < 0) itmp= 0;
		else if (itmp > 0x8000) itmp= 0x8000;
	*du= (us) itmp;
	itmp= 32768L * *(set ++);
	if (itmp < 0) itmp= 0;
		else if (itmp > 0x8000) itmp= 0x8000;
	*dv= (us) itmp;
}

static void	epbft_flat(int n, us * site, float * ptr)
{
	unsigned long
		attrib, sz;

	while (n --)
	{
		attrib= *(site ++);
		sz= attrib & 0x3ff;

		site += sz;
		if (attrib & EPB_FACTN) site += sz;
		if (attrib & EPB_FACTT)
		{
			while (sz --)
				{ attrib= * site;  tb_lookupuv(attrib, ptr, site, site +1);  site += 2; }
		}
	}
}

static int	tb_expandface(us *dest, us *list, int n, float *uptr, vector *v)
{
	us *listl, *listn, *listt;
	int plt, plsrf, plsize,
		kset, ksize, lresu,
		ksurf, kstep;
	long polycount, polycplex;

	polycount= 0,
	ksurf= -1;

	while (n)
	{
		if (n < 6) break;
		plt= LNUM, plsrf= LNUM, plsize= LNUM;
		if (! plsize || (plsize < 3))
			return 0;
		switch (plt)
		{
			case 0: kset= 0, ksize= plsize; break;
			case 1: kset= EPB_FACTN, ksize= 2 * plsize; break;
			case 3: kset= EPB_FACTT, ksize= 2 * plsize; break;
			case 5: kset= EPB_FACTN | EPB_FACTT, ksize= 3 * plsize; break;
			default:
				return 0;
		}
		if (ksize > n)
			return 0;
		if (plsrf != ksurf)
			{ *(dest ++)= EPB_SURF + plsrf, ksurf= plsrf; }
		listl= list +plsize;
		if (kset & EPB_FACTT) { listt= listl, listl += plsize; }
		if (kset & EPB_FACTN) { listn= listl, listl += plsize; }

		if (3 == plsize)
		{
			*(dest ++)= EPB_FACE | kset | plsize;
			for (kstep= plsize; (kstep --); dest ++) *dest= *(list ++);
			IFn for (kstep= plsize; (kstep --); dest ++) *dest= *(listn ++);
			IFt
			{
				for (kstep= plsize; (kstep --); listt ++, dest += 2)
					{ tb_lookupuv(*listt, uptr, dest, dest +1); }
			}
			polycount ++;
		}
		else
		if (4 == plsize)
		{
			if (! kset)
			{
				*(dest ++)= EPB_FACE | kset | plsize;
				for (kstep= plsize; (kstep --); dest ++) *dest= *(list ++);
				polycount ++;
			}
			else
			{
				*(dest ++)= EPB_FACE | kset | 3;
				*(dest ++)= list[0], *(dest ++)= list[1], *(dest ++)= list[2];
				IFn { *(dest ++)= listn[0], *(dest ++)= listn[1], *(dest ++)= listn[2]; }
				IFt
				{
					tb_lookupuv(listt[0], uptr, dest, dest +1);  dest += 2;
					tb_lookupuv(listt[1], uptr, dest, dest +1);  dest += 2;
					tb_lookupuv(listt[2], uptr, dest, dest +1);  dest += 2;
				}

				*(dest ++)= EPB_FACE | kset | 3;
				*(dest ++)= list[3], *(dest ++)= list[0], *(dest ++)= list[2];
				IFn { *(dest ++)= listn[3], *(dest ++)= listn[0], *(dest ++)= listn[2]; }
				IFt
				{
					tb_lookupuv(listt[3], uptr, dest, dest +1);  dest += 2;
					tb_lookupuv(listt[0], uptr, dest, dest +1);  dest += 2;
					tb_lookupuv(listt[2], uptr, dest, dest +1);  dest += 2;
				}

				polycount += 2;
			}
		}
		else
		{
				/** split faces */
			polycplex= 0;
			lresu= epbf_createface(dest, & polycplex, list, (int) kset, (int) plsize, v);
			if (polycplex)
				{ epbft_flat(polycplex, dest, uptr); }
			polycount += polycplex;
			dest += lresu;
		}
		list= listl, n-= ksize;
		if (! plt || (3 == plt)) { list ++, n --; }
	}

	return polycount;
}

static int	tbs_sizeuv(us *src, int fct)
{
	us ka, ksz;
	long luct;

	for (luct= 0; (fct); )
	{
		ka= *(src ++);
		switch (ka & 0xc000)
		{
			case EPB_ATTRIB:
				ka &= 0xfc00;
				if (ka == EPB_BASIS) src ++;
				continue;

			case EPB_FACE:
				ksz= 0xfff & ka, src += ksz;
				if (ka & EPB_FACTN) { src += ksz; }
				if (ka & EPB_FACTT) { luct += ksz, src += 2 * ksz; }
				fct --;
				break;

			default:
				printf(" >>< huh sizeuv\n");
				return luct;
		}
	}
	return luct;
}

static int	tbs_sizef(us *src, int fct)
{
	us ka, ksz;
	long luct;

	for (luct= 0; (fct); )
	{
		ka= *(src ++);
		switch (ka & 0xc000)
		{
			case EPB_ATTRIB:
				ka &= 0xfc00;
				if (ka == EPB_BASIS) src ++;
				continue;

			case EPB_FACE:
				ksz= 0xfff & ka,
				src += ksz;
				luct += 3 + ksz;
				if (ka & EPB_FACTN) { src += ksz, luct += ksz; }
					else { luct ++; }
				if (ka & EPB_FACTT) { src += 2 * ksz, luct += ksz; }
				fct --;
				break;

			default:
				printf(" >>< huh sizef\n");
				return luct;
		}
	}
	return luct;
}

static void	tb_compressu(float *ptr, us *src, int fct)
{
	us ka, ksz;
	unsigned int istep;

	while (fct)
	{
		ka= *(src ++);
		switch (ka & 0xc000)
		{
			case EPB_ATTRIB:
				ka &= 0xfc00;
				if (ka == EPB_BASIS) src ++;
				continue;

			case EPB_FACE:
				ksz= 0xfff & ka, src += ksz;
				if (ka & EPB_FACTN) { src += ksz; }
				if (ka & EPB_FACTT)
				{
					for (istep= 2 * ksz; (istep --); )
						{ ka= *(src ++), *(ptr ++)= (float) 0.000030517578 * ka; }
				}
				fct --;
				break;

			default:
				return;
		}
	}
}

static void	tb_compressf(us *ptr, us *src, int fct)
{
	us ka, ksz;
	unsigned int istep, isurf, iu, ix;
	us *pfirst, *nsrc;

	isurf= 0, iu= 1, ix= 0;
	while (fct)
	{
		ka= *(src ++);
		switch (ka & 0xc000)
		{
			case EPB_ATTRIB:
				if (EPB_SURF == (0xfc00 & ka)) isurf= ka & 0x3ff;
					else if (ka == EPB_BASIS) src ++;
				continue;

			case EPB_FACE:
				ksz= 0xfff & ka, ix= 0;
				pfirst= ptr ++;
				*(ptr ++)= (us) isurf,
				*(ptr ++)= (us) ksz;
				for (istep= ksz; (istep --); ptr ++) { *ptr= *(src ++); }
				if (ka & EPB_FACTT)
				{
					if (ka & EPB_FACTN) { nsrc= src, src += ksz; }
					for (istep= ksz; (istep --); iu ++, src += 2)
						{ *(ptr ++)= (us) iu; }
					if (ka & EPB_FACTN)
					{
						for (istep= ksz; (istep --); ptr ++) { *ptr= *(nsrc ++); }
						ix= 5;
					}
						else ix= 3;
				}
				else
				if (ka & EPB_FACTN)
				{
					for (istep= ksz; (istep --); ptr ++) { *ptr= *(src ++); }
					ix= 1;
				}
				if (! (ka & EPB_FACTN))
					{ *(ptr ++)= 0; }

				*pfirst= (us) ix;
				fct --;
				break;

			default:
				return;
		}
	}
}

	/***************/

int	tobj_conv(bf_container * asrc, tro_polys * dest)
{
	int iret ;
	long lsz;
	char *block, *fblock;

	fblock= asrc-> fptr;
	if (! asrc-> fcount && dest-> poly_c && dest-> ilist)
		{ asrc-> fcount= dest-> poly_c, fblock= (char *) dest-> ilist; }

	if (! asrc-> vcount || ! asrc-> scount || ! asrc-> fcount)
		return -1;

		/** allocate & transfer point arrays */
	lsz= 3 * sizeof(double) * asrc-> vcount + 3 * sizeof(float) * asrc-> ncount;
	block= malloc(lsz);
	if (NULL == block)
		RE
	dest-> vector_c= asrc-> vcount,
	dest-> vlist= (vector *) block;
	tb_expandv((vector *) block, (float *) asrc-> vptr, asrc-> vcount);
	if (asrc-> ncount)
	{
		block= (char *) (dest-> vlist + dest-> vector_c);
		memcpy(block, asrc-> nptr, 3 * sizeof(float) * asrc-> ncount);
		dest-> normal_c= asrc-> ncount,
		dest-> nlist= (svector *) block;
	}
		else dest-> normal_c= 0, dest-> nlist= NULL;

		/** create & translate surfaces */
	lsz= sizeof(tro_surface) * asrc-> scount;
	block= malloc(lsz);
	if (NULL == block)
		RE
	dest-> surface_c= (us) asrc-> scount,
	dest-> slist= (tro_surface *) block;
	tb_expandsurf(dest-> slist, (bf_surface *) asrc-> sptr, asrc-> scount);

	/** deal with faces */
	lsz= tb_sizeface((unsigned short *) fblock, asrc-> fcount, dest-> vlist -1);
	if (! lsz) RE
	block= malloc(lsz *sizeof(unsigned short));
	if (NULL == block)
		RE

	dest-> ilist= (unsigned short *) block;
	dest-> poly_c= tb_expandface(dest-> ilist,
		(unsigned short *) fblock, asrc-> fcount, (float *) asrc-> uptr, dest-> vlist -1);

	if (asrc-> timesize)
	{
		globaltstring[1023]= '\0';
		strncpy(globaltstring, asrc-> tptr, 1023);
	}

	return 0 ;
}

int	tobj_load(FILE *fptr, tro_polys *dest)
{
	int iret;
	bf_container bcon;

	bzero(& bcon, sizeof(bf_container));
	iret= load_embf(fptr, & bcon);
		if (iret) return iret;

	iret= tobj_conv(& bcon, dest) ;
		if (iret) return iret;

	clear_embf(& bcon);
	return 0;
}

int	tobj_mem(long sz, char * data, tro_polys *dest)
{
	int iret;
	long lsz;
	bf_container bcon;
	char *block, *fblock;

	bzero(& bcon, sizeof(bf_container));
	iret= mem_embf(sz, data, & bcon);
	if (iret) return iret;

	fblock= bcon.fptr;
	if (! bcon.fcount && dest-> poly_c && dest-> ilist)
		{ bcon.fcount= dest-> poly_c, fblock= (char *) dest-> ilist; }

	if (! bcon.vcount || ! bcon.scount || ! bcon.fcount)
		return -1;

		/** allocate & transfer point arrays */
	lsz= 3 * sizeof(double) * bcon.vcount + 3 * sizeof(float) * bcon.ncount;
	block= malloc(lsz);
	if (NULL == block)
		RE
	dest-> vector_c= bcon.vcount,
	dest-> vlist= (vector *) block;
	tb_expandv((vector *) block, (float *) bcon.vptr, bcon.vcount);
	if (bcon.ncount)
	{
		block= (char *) (dest-> vlist + dest-> vector_c);
		memcpy(block, bcon.nptr, 3 * sizeof(float) * bcon.ncount);
		dest-> normal_c= bcon.ncount,
		dest-> nlist= (svector *) block;
	}
		else dest-> normal_c= 0, dest-> nlist= NULL;

		/** create & translate surfaces */
	lsz= sizeof(tro_surface) * bcon.scount;
	block= malloc(lsz);
	if (NULL == block)
		RE
	dest-> surface_c= (us) bcon.scount,
	dest-> slist= (tro_surface *) block;
	tb_expandsurf(dest-> slist, (bf_surface *) bcon.sptr, bcon.scount);

	/** deal with faces */
	lsz= tb_sizeface((unsigned short *) fblock, bcon.fcount, dest-> vlist -1);
	if (! lsz) RE
	block= malloc(lsz *sizeof(unsigned short));
	if (NULL == block)
		RE

	dest-> ilist= (unsigned short *) block;
	dest-> poly_c= tb_expandface(dest-> ilist,
		(unsigned short *) fblock, bcon.fcount, (float *) bcon.uptr, dest-> vlist -1);

	if (bcon.timesize)
	{
		globaltstring[1023]= '\0';
		strncpy(globaltstring, bcon.tptr, 1023);
	}
	clear_embf(& bcon);

	return 0;
}

int	tobj_save(FILE *fptr, tro_polys *src)
{
	long lcount;
	bf_container bcon;

	bzero(& bcon, sizeof(bf_container));
	/**  VERT., NORM., UTEX., SURF., FACE., TIME. */

		/** v */
	lcount= src-> vector_c;
	bcon.vcount= lcount, bcon.vptr= malloc(3 * sizeof(float) * lcount);
	tb_compressv((float *) bcon.vptr, src-> vlist, lcount);
		/** n */
	bcon.ncount= src-> normal_c, bcon.nptr= (char *) src-> nlist;
		/** u */
	lcount= tbs_sizeuv(src-> ilist, src-> poly_c);
	bcon.ucount= lcount, bcon.uptr= malloc(2 * sizeof(float) * lcount);
	tb_compressu((float *) bcon.uptr, src-> ilist, src-> poly_c);
		/** s */
	lcount= src-> surface_c;
	bcon.scount= lcount, bcon.sptr= malloc(sizeof(bf_surface) * lcount);
	tf_compresssurf((bf_surface *) bcon.sptr, src-> slist, lcount);
		/** f */
	lcount= tbs_sizef(src-> ilist, src-> poly_c);
	bcon.fcount= lcount, bcon.fptr= malloc(sizeof(short) * lcount);
	tb_compressf((us *) bcon.fptr, src-> ilist, src-> poly_c);
		/** t */
	if (* globaltstring)
		{ bcon.timesize= strlen(globaltstring), bcon.tptr= globaltstring; }

	if (store_embf(fptr, & bcon)) RE

	EFREE(bcon.vptr);
	EUFREE(bcon.uptr);
	EFREE(bcon.sptr);
	EFREE(bcon.fptr);
	return 0;
}

int	tobj_loadname(char *name, tro_polys *dest)
{
	int iret;
	FILE *fsrc;

	fsrc= fopen(name, "rb");
	if (NULL == fsrc) RE

	iret= tobj_load(fsrc, dest);
	if (iret) return iret;

	fclose(fsrc);
	return 0;
}

int	tobj_savename(char *name, tro_polys *dest)
{
	int iret;
	FILE *fdst;

	fdst= fopen(name, "wb");
	if (NULL == fdst) RE

	iret= tobj_save(fdst, dest);
	if (iret) RE

	fclose(fdst);
	return 0;
}

void	tobj_release(tro_polys *dest)
{
	if (dest-> vlist)
	{
		EFREE(dest-> vlist);
		dest-> vector_c= dest-> normal_c= 0;
		dest-> vlist= NULL,
		dest-> nlist= NULL;
	}
	if (dest-> slist)
	{
		EFREE(dest-> slist);
		dest-> surface_c= 0;
		dest-> slist= NULL;
	}
	if (dest-> ilist)
	{
		EFREE(dest-> ilist);
		dest-> poly_c= 0,
		dest-> ilist= NULL;
	}
}

