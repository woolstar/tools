#include "trun.h"
#include "rqdata.h"

	extern int	global_time;

	static char	ixpress[512];
	static vector	camera_dir, camera_at;
	static double	camera_zpos;

	long	*imbuffer= NULL,
			bfreswidth= 0, cfreswidth;
	int		aa_samp, aa_width, aa_stretch, aa_window,
			pa_width, pa_height, pa_slot, pa_flip;

	extern tlist_scene	tfocus;

		/** epbf_conv.c */
	extern char	globaltstring[];

	FILE	*fimsav;
	ehdr_image destsect;

void	cg_flipop(long *imline)
{
	int istep;
	long *la, *lb, lt;

	for (istep= pa_width /2, la= imline, lb= imline + pa_width -1;
			(istep -- && (la != lb)); la ++, lb --)
		{ lt= *la, *la= *lb, *lb= lt; }
}

int	cp_prepbuffer(void)
{
	long l, lv;
	int sl;

	if (tfocus.setup.adapt)
	{
		aa_samp= tfocus.setup.adapt,
		aa_width= tfocus.setup.filterwidth,
		aa_stretch= aa_samp * aa_width,
		aa_window= aa_samp * (2 * aa_width +1);
	}
		else { aa_samp= aa_width= aa_stretch= aa_window= 0; }

	if (! tfocus.setup.curcam)
		return 1;

	pa_width= tfocus.setup.curcam-> x,
	pa_height= tfocus.setup.curcam-> y;
	lv= pa_width * pa_height;
		if (aa_samp) { lv *= aa_samp * aa_samp; }
	for(sl= 0, l= lv; (l > 6291456); ) { sl ++, l= lv / sl; }
	if (sl)
	{
		pa_slot= pa_height / sl;
		lv= pa_slot * sl;
		if (lv < pa_height) pa_slot ++;
	}
		else pa_slot= pa_height;
	pa_flip= tfocus.setup.image.iformat & EV_FLIP;

	w_tilerimage(pa_width, pa_height, pa_slot, aa_width, aa_samp);

	l= bfreswidth,
	lv= pa_width;
	if (! lv) RE
	if (lv > l)
	{
		if (l)
		{
			EFREE(imbuffer);
			imbuffer= NULL, bfreswidth= 0;
		}

		imbuffer= EMEM(lv, long);
		if (NULL == imbuffer)
			{ printf(" >> buffer allocation error for (%d bytes)\n", 8 * lv);  RE }
		
		bfreswidth= pa_width;
	}

	cfreswidth= pa_width;
	return 0;
}

#define	E_BGN	wp_beginpoly();
#define E_END	wp_endpoly();

static void	cs_sequencename(char *s)
{
	switch (tfocus.setup.image.icode)
	{
		case 1:
			strcpy(s, tfocus.setup.image.imageform);
			break;
		case 3:
			sprintf(s, tfocus.setup.image.imageform,
				global_time +tfocus.setup.image.ioffset);
			break;
		default:
			strcpy(s, "test.tga");
			break;
	}
}

int	cphong_simple(ehdr_image *imptr)
{
	int
		iret, iformat,
		irowstep, ipixstep;
	long *lfill;
	byte *rb, *gb, *bb, *ab;

	rb= workset.image, gb= rb +workset.page, bb= gb +workset.page, ab= bb +workset.page;

	for (irowstep= tfocus.setup.curcam-> y, lfill= imbuffer; (irowstep --); )
	{
		for (ipixstep= tfocus.setup.curcam-> x, lfill= imbuffer; (ipixstep --); lfill ++)
			{ *lfill= *(rb ++) | (*(gb ++) << 8) | (*(bb ++) << 16) | (*(ab ++) << 24); }

		if (pa_flip) cg_flipop(imbuffer);
		iret= wtga_scanline(imbuffer, imptr);
			if (iret) RE
	}

	return 0;
}

/*
void	cphong_testl()
{
	char testname[1024];
	int
		iret, iformat,
		irowstep, ipixstep, ex, ey;
	long *lfill, *lpage;
	byte *rb, *gb, *bb, *ab;
	ehdr_image iu;

	cs_sequencename(testname);
	lpage= EMEM(16384, long);

	iu.bits= 2,
	iu.depth= 32,
	iu.width= aa_samp * tfocus.setup.curcam-> x + 2 * aa_stretch,
	iu.height= aa_samp * tfocus.setup.curcam-> y + 2 * aa_stretch,
	iu.data= (im_pixel *) lpage;

	wtga_header(testname, &iu, 0, 1);

	rb= workset.image, gb= rb +workset.page, bb= gb +workset.page, ab= bb +workset.page;
	for (irowstep= iu.height; (irowstep --); )
	{
		for (ipixstep= iu.width, lfill= lpage; (ipixstep --); lfill ++)
			{ *lfill= *(rb ++) | (*(gb ++) << 8) | (*(bb ++) << 16) | (*(ab ++) << 24); }
		if (pa_flip) cg_flipop(imbuffer);
		wtga_scanline(lpage, &iu);
	}
	fclose(iu.fp);
}
*/

int	cphong_pfirst(void)
{
	int iret, iformat;
	char destname[1024];

	iformat= EV_VID;
	if ((NULL == imbuffer) || !cfreswidth || !tfocus.setup.curcam-> y) RE
	cs_sequencename(destname);
	if (tfocus.setup.image.iformat) iformat= tfocus.setup.image.iformat;

	destsect.bits= 2,
	destsect.depth= 32,
	destsect.width= cfreswidth,
	destsect.height= tfocus.setup.curcam-> y;
	destsect.data= (im_pixel *) imbuffer;
	if (* globaltstring) { iret= wtga_headerex(destname, &destsect, 0, 1, globaltstring); }
		else { iret= wtga_header(destname, &destsect, 0, 1); }

	if (iret)
		{ printf(" >> image open failed.\n");  RE }
	if (tfocus.setup.adapt)
	{
		iret= t_aapstart(& destsect);
		if (iret)
			{ printf(" >> filter init failed.\n");  RE }
	}

	return 0;
}

int	cphong_part(void)
{
	int
		iret, iformat,
		irowstep, ipixstep;
	long *lfill;
	byte *rb, *gb, *bb, *ab;

	if (tfocus.setup.adapt)
	{
		iret= t_aapfilter(& destsect);
			if (iret) RE
	}
	else
	{
		rb= workset.image, gb= rb +workset.page, bb= gb +workset.page, ab= bb +workset.page;

		for (irowstep= pa_slot, lfill= imbuffer; (irowstep --); )
		{
			for (ipixstep= tfocus.setup.curcam-> x, lfill= imbuffer;
				(ipixstep --); lfill ++)
			{
				*lfill= *(rb ++) | (*(gb ++) << 8) |
					(*(bb ++) << 16) | (*(ab ++) << 24);
			}

			if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline(imbuffer, & destsect);
				if (iret) RE
		}
	}

	return 0;
}

void	cphong_pfinal(void)
{
	fclose(destsect.fp);
	t_aapfinal();
}

int cphong_save(void)
{
	int iret, iformat;
	char destname[1024];

	if (cphong_pfirst()) RE
	if (tfocus.setup.adapt)
	{
		iret= t_aafilter(& destsect);
			if (iret) RE
		cphong_pfinal();
	}
	else
	{
		iret= cphong_simple(& destsect);
			if (iret) RE
		fclose(destsect.fp);
	}

	return 0;
}

