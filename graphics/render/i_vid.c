#include "basic.h"

	/****
		Copyright 1997 Twin Forces
		All Rights Reserved

		i_vid.c -- vid image loader
	*/

	/***** global */
static byte	libuf[16384];
extern char *mem_name(int);

	/*****/

int vid_header(char *fname, ehdr_image *brec)
{
	FILE *fl_video;
	short
		siz_x= 0, siz_y= 0,
		bit_h= 0;
	long 
		data_size, *block;
	char c, *p;
#if RAEL_UNIK
	char *ptail, scmd[1024];
#endif

	brec-> fp= NULL;
#if RAEL_UNIK
	ptail= strrchr(fname, '.');
	if (ptail && (! strcmp(ptail, "vZ") || !strcmp(ptail, "vz")))
	{
		sprintf(scmd, "zcat %s", fname);
		fl_video= popen(scmd, "r");
	}
		else
#endif
		fl_video= fopen(fname, "rb");


	if (fl_video== NULL) return 1;

	fgets((char *) libuf, sizeof(libuf), fl_video);
	p= (char *) libuf;  P_space P_word
	bit_h= atoi(p);  P_word
	siz_x= atoi(p);  P_word
	siz_y= atoi(p);

	if (! siz_x || ! siz_y) return 1;

	data_size= siz_x * siz_y, block= EMEM(data_size, long);
	brec-> fp= fl_video;
	brec-> depth= 24,
	brec-> bits= bit_h;
	brec-> width= siz_x; 
	brec-> height= siz_y;
	brec-> data= (im_pixel *) block;

	if (block== NULL)
	{
		siz_x= 0;
		printf(" >> Unable to create %s for loading image %s\n", 
			mem_name((long) (sizeof(long) * data_size)), fname);
		return 1;
	}

	strcpy(brec-> filename, fname);
	return 0;
}

int	vid_scanline(vih, rowp)
	ehdr_image *vih;
	long *rowp;
{
	short isize, istep, iret;
	byte *dbr, *dbg, *dbb;
	long sum;
	FILE *fin;

	fin= vih-> fp,
	isize= vih-> width;

	iret= fread((char *) libuf, 3 * sizeof(char), isize, fin);
	if (iret != isize)
		{ printf(" >> out of data on scanline\n");  return 1; }

	dbr= libuf, dbg= libuf + isize, dbb= libuf +isize +isize;
	for (istep= isize; (istep --); dbr ++, dbg ++, dbb ++)
		{ sum= *dbr + (*dbg << 8) + (*dbb << 16), *(rowp ++)= sum; }

	return 0;
} 

int	wvid_header(char *fname, ehdr_image *brec, int dorder, int dcompress)
{
	FILE *fwri;

#if RAEL_UNIK
	char scmd[1024];

	if (dcompress)
	{
		sprintf(scmd, "compress > %s.Z", fname);
		fwri= popen(scmd, "w");
	}
		else
#endif
		{ fwri= fopen(fname, "wb"); }

	if (NULL == fwri)
		return 1;

	fprintf(fwri, "Frame 1 %d %d\n", brec-> width, brec-> height);

	brec-> fp= fwri;
	return 0;
}

int	wvid_scanline(unsigned long *im, ehdr_image *brec)
{
	int iret;
	unsigned char *db;
	unsigned int istep, isize;
	unsigned long
		*imstep, val ;
	FILE *fwri;

	isize= brec-> width,
	fwri= brec-> fp;
	db= libuf;

	for (istep= isize, imstep= im; (istep --); imstep ++, db ++)
		{ val= *imstep, *db= 0xff & val; }
	for (istep= isize, imstep= im; (istep --); imstep ++, db ++)
		{ val= *imstep, *db= 0xff & (val >> 8); }
	for (istep= isize, imstep= im; (istep --); imstep ++, db ++)
		{ val= *imstep, *db= 0xff & (val >> 16); }

	iret= fwrite((char *) libuf, 3 * sizeof(byte), isize, fwri);
	if (iret != isize)
		{ printf(" >> scanline write failed [VID]\n");  return 1; }
	return 0;
}

#define 	COMPRESS	1

/**
	brec-> width= x,
	brec-> height= y,
	brec-> data= image
**/

int	wvid_image(char *fname, ehdr_image *brec)
{
	int iv, iy;
	unsigned long *brow;

	if (iv= wvid_header(fname, brec, 0, COMPRESS)) return iv;
	for (iy= brec-> height, brow= (unsigned long *) brec-> data; (iy --); brow += brec-> width)
		{ if (iv= wvid_scanline(brow, brec)) return iv; }

#ifdef RAELUNIK
#if COMPRESS
	pclose(brec-> fp);
#else
	fclose(brec-> fp);
#endif
#else
	fclose(brec-> fp);
#endif

	return 0;
}

