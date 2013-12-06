#ifndef	ITARGA_DEF
#	define	ITARGA_DEF	1

typedef	struct { unsigned char r,g,b,a ; } im_pixel ;

typedef struct
{
	FILE *fp;
	char filename[64];
	unsigned short
		imtype, bits,
		depth, width, height,
		cur_y;
	im_pixel *data;
	unsigned char *scanline;
} image_data ;

#ifdef __cplusplus
	extern "C"
	{
#endif

		int tga_header(const char *, image_data *, int allocate);
		int tga_headerf(image_data *, int allocate);
		int	tga_scanline(image_data *, unsigned long *);
		int	tga_image(const char *, image_data *);

		int	wtga_header(const char *, image_data *, int dorder, int dcompress);
		int	wtga_headerex(const char *, image_data *, int dorder, int dcompress, const char *comment);
		int	wtga_headerFILE(FILE * fwri, image_data *brec, int dorder, int dcompress) ;
		int	wtga_scanline(unsigned long *, image_data *);
		int	wtga_image(const char *, image_data *);

#ifdef __cplusplus
	} ;
#endif

#endif	// ITARGA_DEF