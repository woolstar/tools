   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_PIC
#	define	TFBASIC_PIC	1

   /***** image things */

#define	EV_VID		0x100
#define EV_RGB		0x200
#define EV_TGA		0x300
#define	EVT_TP2		0x010
#define EVT_TP10	0x020
#define	EVT_TP1		0x040
#define	EVT_TP3		0x050
#define	EVT_TP11	0x060
#define	EVT_BT8		0x000
#define	EVT_BT16	0x001
#define EVT_BT24	0x002
#define EVT_BT32	0x003
#define	EV_VAD		0x400
#define EV_TEU		0x500
#define EV_PICT		0x800
#define EV_YUV		0xa00
#define EV_PCX		0xb00
#define EV_IMAGE	0xc00
#define EV_JPEG		0xd00
#define	EV_FLIP		0x8

#define	EVP_ZCOMP	0x10

/**  7/14/92  New improved internal pixel format:
///  Components stored as separate bytes.
**/

typedef struct { unsigned char r,g,b,a; } im_pixel;

typedef struct
{
   short
      depth, bits,
      width, height;
   long
      *data;
} ehdr_ima;

   /*** new improved image stuff,
      file, filename, image type, some flags, color depth,
      image type, some flags, color depth,
      im width and height, current scanline,
      scanline data in internal format (packed long)
      scanline buffer
      and flag if data and scanline were auto-alloc'ed.
      */

typedef struct
{
	FILE *fp;
	char filename[256];
	unsigned short
		imtype, bits,
		depth, width, height,
		cur_y;
	im_pixel *data;
	unsigned char *scanline;
	unsigned char free_on_close;
	} ehdr_image;

   /** second generation image stuff */

typedef struct
{
	char source[256];
	unsigned short
		tipe, depth,
		properties, stepper,
		width, height;
	long
		*h_table,
		*data;
} ev_header;

#endif

