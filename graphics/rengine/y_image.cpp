#include "ye.h"

#include	"rqdata.h"
#include	"aart_post.h"

extern "C"
{
	void	cg_flipop(long *) ;

		// dx_post
	extern int	aa_samp, aa_width, aa_stretch, aa_window ;
	extern int	t_aafilter(ehdr_image *imptr) ;
	extern void	t_aapfinal() ;
} ;

extern "C"
{
		// artgeom
	extern char	globaltstring[];

		// i targa
	extern int	wtga_header(char *, ehdr_image *, int dorder, int dcompress);
	extern int	wtga_headerex(char *, ehdr_image *, int dorder, int dcompress, char *comment);
	extern int	wtga_scanline(unsigned long *, ehdr_image *);

		// y_setup
	extern int		pa_width, pa_height, pa_slot, pa_flip;
	extern long	*imbuffer ;
} ;

void	cg_flipop(long *imline)
{
	int istep;
	long *la, *lb, lt;

	for (istep= pa_width /2, la= imline, lb= imline + pa_width -1;
			(istep -- && (la != lb)); la ++, lb --)
		{ lt= *la, *la= *lb, *lb= lt; }
}

bool	yzor::save()
{
	int iret,
		irowstep, ipixstep ;
	long	* lfill ;
	unsigned char	* rb, * gb, * bb, * ab ;
	char	destname[256] ;
	const aart_post	* prec ;

		// header setup
	imheader.bits= 10 ;
	imheader.depth= 32 ;
	imheader.data= (im_pixel *) imbuffer ;

	prec= aart_post::getpost(mnode) ;
	generalstring::sequence(destname, prec-> destpath, t) ;

	if (* globaltstring)
		iret= wtga_headerex(destname, & imheader, 0, 1, globaltstring) ;
	else
		iret= wtga_header(destname, & imheader, 0, 1) ;

	if (iret)
		return false ;

	if (aa_samp)
	{
		iret= t_aafilter(& imheader) ;
		if (iret)
			return false ;

		t_aapfinal();
	}
	else
	{
		rb= workset.image, gb= rb +workset.page, bb= gb +workset.page, ab= bb +workset.page;

		for (irowstep= pa_height; (irowstep --); )
		{
			for (ipixstep= pa_width, lfill= imbuffer; (ipixstep --); lfill ++)
				{ *lfill= *(rb ++) | (*(gb ++) << 8) | (*(bb ++) << 16) | (*(ab ++) << 24); }

	//		if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline((unsigned long *) imbuffer, & imheader);
				if (iret) return false ;
		}
	}

	fclose(imheader.fp);
	return true ;
}