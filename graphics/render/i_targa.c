#include "basic.h"

	/****
		Copyright 1997 Twin Forces, Inc.
		All Rights Reserved

		i_targa.c -- targa image loader

		int tga_header(char *, ehdr_image *, int allocate);
		int tga_headerf(ehdr_image *, int allocate);
		int	tga_scanline(ehdr_image *, long *);
		int	tga_image(char *, ehdr_image *);

		int	wtga_header(char *, ehdr_image *, int dorder, int dcompress);
		int	wtga_headerex(char *, ehdr_image *, int dorder, int dcompress, char *comment);
		int	wtga_scanline(unsigned long *, ehdr_image *);
		int	wtga_image(char *, ehdr_image *);
	*/

	/***** global */
static unsigned char	libuf[16384];
extern char *mem_name(int);

	/*****/

#define	T24ALWAYSALPHA	if (sum)
#define	T24ALWAYSALPHAX

int tga_header(char *fname, ehdr_image *brec, int allocate)
{
	FILE *fl_video;
	short
		siz_x= 0, siz_y= 0, orig_y= 0,
		bit_h= 0;
	unsigned char hdr[20];
	long 
		data_size, *block;

	brec-> fp= NULL;
	fl_video= fopen(fname, "rb");
	if (fl_video== NULL) return 1;

	fread(hdr, sizeof(unsigned char), 18, fl_video);
	bit_h= hdr[0];
		/**** Time Arts lumina bug hack XXXX
			ok, actually, there is more stuff like colormap
			[0] is the text label size
			[2] image storage mode
			[1] is the color map type
			[3,4] is the color map origin, [5,6] the lenght, [7] the bit size
				tims hack is fseek(f, cmap_size * cmap_length / 8, 1);
				"ahh size must be bits, length is entries"
			the origin is at x[8,9] y[10,11]
			the size is x[12,13], y[14,15]
			bit size is [16]
			the descriptor at [17]
		*/
	if (hdr[1] == 1) 
		bit_h+= (hdr[7] * (hdr[5] +(hdr[6] <<8)) / 8);
	if (bit_h) 
		fseek(fl_video, (long) bit_h, 1);
	bit_h= hdr[2];
	orig_y= hdr[10] + (hdr[11] << 8) ;
	siz_x= hdr[12] + (hdr[13] << 8);
	siz_y= hdr[14] + (hdr[15] << 8);
	brec-> fp= fl_video;
	brec-> depth= hdr[16]; 
	brec-> bits= bit_h;
	brec-> width= siz_x; 
	brec-> height= siz_y;
	brec-> cur_y= ((0x20 == hdr[17]) || ! orig_y) ? 1 : siz_y;

	if (allocate)
	{
		data_size= siz_x * siz_y,
		block= EMEM(data_size, long);
		brec-> data= (im_pixel *) block;

		if (block == NULL)
		{
			siz_x= 0;
			printf(" >> Unable to create %s for loading image %s.\n", 
				mem_name((long) (sizeof(long) * data_size)), fname);
			return 1;
		}
	}

	strcpy(brec-> filename, fname);
	return 0;
}

int tga_headerf(ehdr_image *brec, int allocate)
{
	FILE *fl_video;
	short
		siz_x= 0, siz_y= 0,
		bit_h= 0;
	unsigned char hdr[20];
	long 
		data_size, *block;

	fl_video= brec-> fp;
	fread(hdr, sizeof(unsigned char), 18, fl_video);
	bit_h= hdr[0];
		/**** Time Arts lumina bug hack XXXX
			ok, actually, there is more stuff like colormap
			[0] is the text label size
			[2] image storage mode
			[1] is the color map type
			[3,4] is the color map origin, [5,6] the lenght, [7] the bit size
				tims hack is fseek(f, cmap_size * cmap_length / 8, 1);
				"ahh size must be bits, length is entries"
			the origin is at x[8,9] y[10,11]
			the size is x[12,13], y[14,15]
			bit size is [16]
			the descriptor at [17]
		*/
	if (hdr[1] == 1) 
		bit_h+= (hdr[7] * (hdr[5] +(hdr[6] <<8)) / 8);
	if (bit_h) 
		fseek(fl_video, (long) bit_h, 1);
	bit_h= hdr[2];
	siz_x= hdr[12] + (hdr[13] << 8);
	siz_y= hdr[14] + (hdr[15] << 8);
	brec-> fp= fl_video;
	brec-> depth= hdr[16]; 
	brec-> bits= bit_h;
	brec-> width= siz_x; 
	brec-> height= siz_y;
	brec-> cur_y= (0x20 == hdr[17]) ? 1 : siz_y;

	if (allocate)
	{
		data_size= siz_x * siz_y,
		block= EMEM(data_size, long);
		brec-> data= (im_pixel *) block;

		if (block == NULL)
		{
			siz_x= 0;
			printf(" >> Unable to create %s for loading image %s.\n", 
				mem_name((long) (sizeof(long) * data_size)), brec-> filename);
			return 1;
		}
	}

	return 0;
}

int	tga_scanline(ehdr_image *vih, unsigned long *rowp)
{
	short 
		i, iw, ig;
	unsigned char 
		packet[4], *dp;
	unsigned long
		sum, *tranf;
	FILE *fin;

	fin= vih-> fp,
	iw= vih-> width;
	tranf= rowp;

	switch (vih-> bits)
	{
		case 2:
			switch (vih-> depth)
			{
				case 16:
					ig= fread((char *) libuf, 2 * sizeof(unsigned char), iw, fin);
					if (ig!= iw) 
						goto loSv2_reEr;
					for (i= iw, dp= libuf; i--; tranf++)
					{
						sum= (*dp & 0x1f) << 19;
						sum+= (*dp & 0xe0) << 6, dp++;
						sum+= (*dp & 0x03) << 14;
						sum+= (*dp & 0x7c) << 1, dp++;
						if (sum) 
							sum |= 0xff000000;
						*tranf= sum;
					}

					break;
				case 24:
					ig= fread((char *) libuf, 3 * sizeof(unsigned char), iw, fin);
					if (ig!= iw) 
						goto loSv2_reEr;
					for (i= iw, dp= libuf; i--; tranf++)
					{
						sum= (*(dp++)) << 16, sum+= (*(dp++)) << 8, sum+= *(dp++);
						T24ALWAYSALPHA
							sum |= 0xff000000;
						*tranf= sum;
#if 0
						printf("the BGRA element %d\n",sum);
#endif
					}

					break;
				case 32:
					ig= fread((char *) libuf, 4 * sizeof(unsigned char), iw, fin);
					if (ig!= iw) 
						goto loSv2_reEr;
					for (i= iw, dp= libuf; i--; tranf++)
					{  
						/**** 32bit  BGRA */
						sum= (*(dp++)) << 16, sum+= (*(dp++)) << 8; 
						sum+= *(dp++); 
						sum+= *(dp++) << 24;  
						*tranf= sum;
#if 0
						printf("the BGRA element %d\n",sum);
#endif
					}

					break;
				}
			break;
		case 3:
			ig= fread((char *) libuf, sizeof(unsigned char), iw, fin);
			if (ig != iw)
				goto loSv2_reEr;
			for (i= iw, dp= libuf; i--; tranf++)
			{
				sum= *(dp ++);
				if (sum) { *tranf= 0xff000000 | (sum << 16) | (sum << 8) | (sum); }
					else *tranf= 0;
			}
			break;

loSv2_reEr:
				/**** some kind of read error */
			printf(" >> targa load\n\tout of data.\n");
			return 1;

		case 10:
			switch (vih-> depth)
			{
				case 16:
					for (i= iw; i; )
					{
						if (!fread(packet, sizeof(unsigned char), 1, fin)) 
							goto loSvt_reEr;
						ig= packet[0];
						if (ig & 0x80)
						{
							ig -= 127;
							if (!fread(packet, 2 *sizeof(unsigned char), 1, fin)) 
								goto loSvt_reEr;
							sum= ((packet[0] & 0x1f) << 19) + ((packet[0] & 0xe0) << 6)
								+ ((packet[1] & 0x03) << 14) + ((packet[1] & 0x7c) << 1);
							if (sum) 
								sum |= 0xff000000;
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								*tranf= sum; 
							}

							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
						}
						else
						{
							ig++;
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								if (!fread(packet, 2 *sizeof(unsigned char), 1, fin)) 
									goto loSvt_reEr;
								sum= ((packet[0] & 0x1f) << 19) + ((packet[0] & 0xe0) << 6)
									+ ((packet[1] & 0x03) << 14) + ((packet[1] & 0x7c) << 1);
								if (sum) 
									sum |= 0xff000000;
								*tranf= sum; 
							}
							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
						}
					}

					break;
				case 24:
					for (i= iw; i; )
					{
						if (!fread(packet, sizeof(unsigned char), 1, fin)) 
							goto loSvt_reEr;
						ig= packet[0];
						if (ig & 0x80)
						{
							ig -= 127;
							if (!fread(packet, 3 *sizeof(unsigned char), 1, fin)) 
								goto loSvt_reEr;
							sum= (packet[0] << 16) + (packet[1] << 8) + packet[2];
							T24ALWAYSALPHA
								sum |= 0xff000000;
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								*tranf= sum; 
							}
							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
						}
						else
						{
							ig++;
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								if (!fread(packet, 3 *sizeof(unsigned char), 1, fin)) 
									goto loSvt_reEr;
								sum= (packet[0] << 16) + (packet[1] << 8) + packet[2];
								T24ALWAYSALPHA
									sum |= 0xff000000;
								*tranf= sum; 
							}
							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
						}
					}

					break;
				case 32:
					for (i= iw; i; )
					{
						if (!fread(packet, sizeof(unsigned char), 1, fin)) 
							goto loSvt_reEr;
						ig= packet[0];
						if (ig & 0x80)
						{
							ig -= 127;
							if (!fread(packet, 4 *sizeof(unsigned char), 1, fin)) 
								goto loSvt_reEr;
							sum= (packet[3] << 24) + (packet[0] << 16) + (packet[1] << 8) + packet[2];
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								*tranf= sum; 
							}
							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
						}
						else
						{
							ig++;
							for (; ig && i; ig--, i--, tranf++) 
							{ 
								if (!fread(packet, 4 *sizeof(unsigned char), 1, fin)) 
									goto loSvt_reEr;
								sum= (packet[3] << 24) + (packet[0] << 16)
									+ (packet[1] << 8) + packet[2];
								*tranf= sum; 
							}
							if (ig)
							{
								printf(">> decoding targa file\n");
								printf("\t%d leftover pixels during scan line uncompress", ig);
									/** return 3; */
							}
					}
				}

					break;
			}
			break;
		case 11:
			for (i= iw; i; )
			{
				if (!fread(packet, sizeof(unsigned char), 1, fin)) 
					goto loSvt_reEr;
				ig= packet[0];
				if (ig & 0x80)
				{
					ig -= 127;
					if (!fread(packet, sizeof(unsigned char), 1, fin)) 
						goto loSvt_reEr;
					sum= *packet;
					if (sum) 
						{ sum|= 0xff000000 | (sum << 16) | (sum << 8); }
					for (; ig && i; ig--, i--, tranf++) 
						{ *tranf= sum; }
					if (ig)
					{
						printf(">> decoding targa file\n");
						printf("\t%d leftover pixels during scan line uncompress", ig);
							/** return 3; */
					}
				}
				else
				{
					ig++;
					for (; ig && i; ig--, i--, tranf++) 
					{ 
						if (!fread(packet, sizeof(unsigned char), 1, fin)) 
							goto loSvt_reEr;
						sum= *packet;
						if (sum) 
							{ sum|= 0xff000000 | (sum << 16) | (sum << 8); }
						*tranf= sum; 
					}
					if (ig)
					{
						printf(">> decoding targa file\n");
						printf("\t%d leftover pixels during scan line uncompress", ig);
							/** return 3; */
					}
				}
			}
			break;

loSvt_reEr:
				/**** some kind of read error */
			printf(" >> loading targa image (type 10)\n\tout of data.\n");
			return 1;
	}
	return 0;
} 

int	tga_image(char *fname, ehdr_image *brec)
{
	int iv, iy;
	long l;
	unsigned long *brow;

	if (iv= tga_header(fname, brec, 1)) return iv;

	if (1 == brec-> cur_y)
	{
		for (iy= brec-> height, brow= (unsigned long *) brec-> data; (iy --); brow += brec-> width)
			{ if (iv= tga_scanline(brec, brow)) return iv; }
	}
	else
	{
		l= brec-> height * brec-> width;
		for (iy= brec-> height, brow= (unsigned long *) brec-> data + l; (iy --); )
		{
			brow -= brec-> width;
			if (iv= tga_scanline(brec, brow)) return iv;
		}
	}

	fclose(brec-> fp);
	return 0;
}

int	wtga_header(char *fname, ehdr_image *brec, int dorder, int dcompress)
{
	FILE *f;
	
	f= fopen(fname, "wb");
	if (NULL == f)
		return 1;

	strcpy(brec-> filename, fname);
	return wtga_headerFILE(f, brec, dorder, dcompress);
}

int	wtga_headerFILE(FILE * fwri, ehdr_image *brec, int dorder, int dcompress)
{
	int iret;
	unsigned char hdr[20];
	
	memset(hdr, 0, 20 * sizeof(char));

	if ((16 != brec-> depth) && (24 != brec-> depth) && (32 != brec-> depth))
	{
		if (8 == brec-> depth)
		{
			hdr[2]= (dcompress) ? 11 : 3;
		}
			else
		return 1;
	}
		else hdr[2]= (dcompress) ? 10 : 2;
	hdr[12]= 0xff & brec-> width,
	hdr[13]= 0xff & (brec-> width >> 8),
	hdr[14]= 0xff & brec-> height,
	hdr[15]= 0xff & (brec-> height >> 8);
	hdr[16]= brec-> depth,
	hdr[17]= (dorder) ? 0x20 : 0;
	
	iret= fwrite((char *) hdr, sizeof(char), 18, fwri);
	if (18 != iret)
		return 1;

	brec-> bits= hdr[2];
	brec-> fp= fwri;
	return 0;
}

int	wtga_headerex(char *fname, ehdr_image *brec, int dorder, int dcompress, char *comment)
{
	int iret, icomlen;
	FILE *fwri;
	unsigned char hdr[20];
	
	fwri= fopen(fname, "wb");
	if (NULL == fwri)
		return 1;

	memset(hdr, 0, 20 * sizeof(char));

	if ((16 != brec-> depth) && (24 != brec-> depth) && (32 != brec-> depth))
	{
		if (8 == brec-> depth)
		{
			hdr[2]= (dcompress) ? 11 : 3;
		}
			else
		return 1;
	}
		else hdr[2]= (dcompress) ? 10 : 2;
	hdr[12]= 0xff & brec-> width,
	hdr[13]= 0xff & (brec-> width >> 8),
	hdr[14]= 0xff & brec-> height,
	hdr[15]= 0xff & (brec-> height >> 8);
	hdr[16]= brec-> depth,
	hdr[17]= (dorder) ? 0x20 : 0;
	hdr[0]= icomlen= strlen(comment) +1;

	iret= fwrite((char *) hdr, sizeof(char), 18, fwri);
	if (18 != iret)
		return 1;

	iret= fwrite(comment, sizeof(char), icomlen, fwri);
	if (icomlen != iret)
		return 1;

	strcpy(brec-> filename, fname);
	brec-> bits= hdr[2];
	brec-> fp= fwri;
	return 0;
}

int	wtga_scanline(unsigned long *im, ehdr_image *brec)
{
	int iret;
	unsigned char
		*p1, *p2;
	unsigned int
		istep, isize,
		ia, ic, k;
	unsigned long
		is, inw;
	FILE *fwri;

	fwri= brec-> fp;
	if (brec-> bits == 10)
	{
		switch (brec-> depth)
		{
			case 8:
				inw= 0xff000000;
				for (p1= libuf, istep= brec-> width, ia= 0, ic= 0, isize= 0; (istep --); im ++)
				{
					is= (*im) & 0xff;
					switch (ia)
					{
						case 0: ia= 1, p2= p1 +1, inw= is;  break;
						case 1:
							*(p2++)= 0xff & inw, isize+= 2;
							if (is!= inw) { ia= 3, ic= 1, inw= is;  break; }
							ia= 2, ic= 2;  break;
						case 2:
							if (is!= inw)
								{ *p1= 0x80 + (ic-1), p1= p2, p2++; inw= is, ia= 1;  break; }
								else ic++;
							if (ic== 128) { *p1= 0xff, p1= p2, ia= 0; }
							break;
						case 3:
							if (is== inw)
							{
								*p1= (ic-1), p1= p2, p2++, ia= 2, ic= 2;
								*(p2++)= 0xff & inw, isize += 2;
								break;
							}
							*(p2++)= 0xff & inw;
							ic++, isize+= 1, inw= is;
							if (ic== 128) { *p1= 0x7f, p1= p2, p2++, ia= 1; }
							break;
					}
				}
				switch (ia)
				{
					case 0:  break;
					case 1:
						*p1= 0x00, *(p2++)= 0xff & inw;
						isize += 2;  break;
					case 2: *p1= 0x80 +(ic-1);  break;
					case 3:
						*p1= ic, *(p2++)= 0xff & inw;
						isize += 1;  break;
				}
				iret= fwrite((char *) libuf, sizeof(unsigned char), isize, fwri);
				if (isize != iret)
					return 1;
				break;
			case 16:
				inw= 0xffffffff;
				for (p1= libuf, istep= brec-> width, ia= 0, ic= 0, isize= 0; (istep --); im ++)
				{
					is= (*im) & 0xffffffff;
					switch (ia)
					{
						case 0: ia= 1, p2= p1 +1, inw= is;  break;
						case 1:
							k= ( inw & 0xfc0000) >> 9,
								k+= ( inw & 0xfc00 ) >> 6, k+= ( inw & 0xfc ) >> 3;
							if (is!= inw) { ia= 3, ic= 1, inw= is;  break; }
							ia= 2, ic= 2;  break;
						case 2:
							if (is != inw)
								{ *p1= 0x80 + (ic-1), p1= p2, p2++; inw= is, ia= 1;  break; }
								else ic++;
							if (ic== 128) { *p1= 0xff, p1= p2, ia= 0; }
							break;
						case 3:
							if (is == inw)
							{
								*p1= (ic-1), p1= p2, p2++, ia= 2, ic= 2;
								k= ( inw & 0xfc0000) >> 9,
									k+= ( inw & 0xfc00 ) >> 6, k+= ( inw & 0xfc ) >> 3;
								*(p2++)= k >> 8, *(p2++)= 0xff & k, isize += 3;
								break;
							}
							k= ( inw & 0xfc0000) >> 9,
								k+= ( inw & 0xfc00 ) >> 6, k+= ( inw & 0xfc ) >> 3;
							*(p2++)= k >> 8, *(p2++)= 0xff & k;
							ic++, isize+= 2, inw= is;
							if (ic== 128) { *p1= 0x7f, p1= p2, p2++, ia= 1; }
							break;
					}
				}
				switch (ia)
				{
					case 0:  break;
					case 1:
						k= ( inw & 0xfc0000) >> 9,
							k+= ( inw & 0xfc00 ) >> 6, k+= ( inw & 0xfc ) >> 3;
						*p1= 0x00, *(p2++)= k >> 8, *(p2++)= k, isize += 3;
						break;
					case 2: *p1= 0x80 +(ic-1);  break;
					case 3:
						k= ( inw & 0xfc0000) >> 9,
							k+= ( inw & 0xfc00 ) >> 6, k+= ( inw & 0xfc ) >> 3;
						*p1= ic;  *(p2++)= k >> 8, *(p2++)= k, isize += 2;
						break;
				}
				iret= fwrite((char *) libuf, sizeof(unsigned char), isize, fwri);
				if (isize != iret)
					return 1;
				break;
			case 24:
				inw= 0xff000000;
				for (p1= libuf, istep= brec-> width, ia= 0, ic= 0, isize= 0; (istep --); im ++)
				{
					is= (*im) & 0xffffff;
					switch (ia)
					{
						case 0: ia= 1, p2= p1 +1, inw= is;  break;
						case 1:
							*(p2++)= inw >> 16, *(p2++)= inw >> 8, *(p2++)= inw, isize+= 4;
							if (is!= inw) { ia= 3, ic= 1, inw= is;  break; }
							ia= 2, ic= 2;  break;
						case 2:
							if (is!= inw)
								{ *p1= 0x80 + (ic-1), p1= p2, p2++; inw= is, ia= 1;  break; }
								else ic++;
							if (ic== 128) { *p1= 0xff, p1= p2, ia= 0; }
							break;
						case 3:
							if (is== inw)
							{
								*p1= (ic-1), p1= p2, p2++, ia= 2, ic= 2, isize += 4;
								*(p2++)= inw >> 16, *(p2++)= inw >> 8, *(p2++)= inw;
								break;
							}
							*(p2++)= inw >> 16, *(p2++)= inw >> 8, *(p2++)= inw;
							ic++, isize+= 3, inw= is;
							if (ic== 128) { *p1= 0x7f, p1= p2, p2++, ia= 1; }
							break;
					}
				}
				switch (ia)
				{
					case 0:  break;
					case 1:
						*p1= 0x00, *(p2++)= inw >> 16, *(p2++)= inw >> 8, *(p2++)= inw;
						isize += 4;  break;
					case 2: *p1= 0x80 +(ic-1);  break;
					case 3:
						*p1= ic;  *(p2++)= inw >> 16, *(p2++)= inw >> 8, *(p2++)= inw;
						isize += 3;  break;
				}
				iret= fwrite((char *) libuf, sizeof(unsigned char), isize, fwri);
				if (isize != iret)
					return 1;
				break;
			case 32:
				inw= 0xffffffff;
				for (p1= libuf, istep= brec-> width, ia= 0, ic= 0, isize= 0; (istep --); im ++)
				{
					is= (*im);
					switch (ia)
					{
						case 0: ia= 1, p2= p1 +1, inw= is;  break;
						case 1:
							*(p2++)= inw >> 16, *(p2++)= inw >> 8,
							*(p2++)= inw, *(p2++)= inw >> 24, isize+= 5;
							if (is!= inw) { ia= 3, ic= 1, inw= is;  break; }
							ia= 2, ic= 2;  break;
						case 2:
							if (is!= inw)
								{ *p1= 0x80 + (ic-1), p1= p2, p2++; inw= is, ia= 1;  break; }
								else ic++;
							if (ic== 128) { *p1= 0xff, p1= p2, ia= 0; }
							break;
						case 3:
							if (is== inw)
							{
								*p1= (ic-1), p1= p2, p2++, ia= 2, ic= 2, isize += 5;
								*(p2++)= inw >> 16, *(p2++)= inw >> 8,
								*(p2++)= inw, *(p2++)= inw >> 24;
								break;
							}
							*(p2++)= inw >> 16, *(p2++)= inw >> 8,
							*(p2++)= inw, *(p2++)= inw >> 24;
							ic++, isize+= 4, inw= is;
							if (ic== 128) { *p1= 0x7f, p1= p2, p2++, ia= 1; }
							break;
					}
				}
				switch (ia)
				{
					case 0:  break;
					case 1:
						*p1= 0x00, *(p2++)= inw >> 16, *(p2++)= inw >> 8,
						*(p2++)= inw, *(p2++)= inw >> 24;
						isize += 5;  break;
					case 2: *p1= 0x80 +(ic-1);  break;
					case 3:
						*p1= ic;  *(p2++)= inw >> 16, *(p2++)= inw >> 8,
						*(p2++)= inw, *(p2++)= inw >> 24;
						isize += 4;  break;
				}
				iret= fwrite((char *) libuf, sizeof(unsigned char), isize, fwri);
				if (isize != iret)
					return 1;
				break;
		}
		return 0;
	}

	switch (brec-> depth)
	{
		case 8:
			for (p1= libuf, istep= brec-> width; (istep --); im++)
			{
				is= *im;
				*(p1++)= 0xff & is;
			}
			iret= fwrite(libuf, sizeof(unsigned char), brec-> width, fwri);
			if (iret!= brec-> width)
				return 1;
			break;
		case 16:
			for (p1= libuf, istep= brec-> width; (istep --); im++)
			{
				is= *im;
				inw= ( is & 0xfc0000) >> 9,
				inw+= ( is & 0xfc00 ) >> 6;
				inw+= ( is & 0xfc ) >> 3;
				*(p1++)= inw >> 8, *(p1++)= inw;
			}
			iret= fwrite(libuf, 2 * sizeof(unsigned char), brec-> width, fwri);
			if (iret!= brec-> width)
				return 1;
			break;
		case 24:
			for (p1= libuf, istep= brec-> width; (istep --); im++)
			{
				is= *im;
				*(p1++)= is >> 16;
				*(p1++)= is >> 8;
				*(p1++)= is;
			}
			iret= fwrite(libuf, 3 * sizeof(unsigned char), brec-> width, fwri);
			if (iret!= brec-> width)
				return 1;
			break;
		case 32:
			for (p1= libuf, istep= brec-> width; (istep --); im++)
			{
				is= *im;
				*(p1++)= is >> 16;
				*(p1++)= is >> 8;
				*(p1++)= is;
				*(p1++)= is >> 24;
			}
			iret= fwrite(libuf, 4 * sizeof(unsigned char), brec-> width, fwri);
			if (iret!= brec-> width)
				return 1;
			break;
	}
	return 0;
}

#define 	COMPRESS	1

/**
	brec-> width= x,
	brec-> height= y,
	brec-> depth= 8, 16, 24, 32
	brec-> bits= 1, 2, 9, 10
	brec-> data= image
**/

int	wtga_image(char *fname, ehdr_image *brec)
{
	int iv, iy;
	unsigned long *brow;
	
	if (iv= wtga_header(fname, brec, 0, COMPRESS)) return iv;
	for (iy= brec-> height, brow= (unsigned long *) brec-> data; (iy --); brow += brec-> width)
		{ if (iv= wtga_scanline(brow, brec)) return iv; }
	fclose(brec-> fp);

	return 0;
}

