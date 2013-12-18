#include "rrun.h"
#include "efilter.h"
#include "rqdata.h"

/***
	future optimization list
		*	tile the front faces of an object first, then back.
*/

extern long		*imbuffer;
extern int			pa_width, pa_height, pa_slot, pa_flip;

int		aa_samp, aa_width, aa_stretch, aa_window ;
static	aa_filter = 0 ;

void	t_setup(unsigned int adapt, unsigned int afilter, unsigned int filterwindow)
{
	if (adapt)
	{
		aa_samp= adapt ;
		aa_width= filterwindow ;
		aa_stretch= aa_samp * aa_width ;
		aa_window= aa_samp * (2 * aa_width + 1) ;
		aa_filter= afilter ;
	}
	else
	{
		aa_samp= 0, aa_width= 0 ;
		aa_stretch= 0, aa_window= 0 ;
		aa_filter= 0 ;
	}
}

int	t_aafilter(ehdr_image *imptr)
{
	byte *rstep, *gstep, *bstep, *astep;
	shurt
		ibs, ihs, ie, ift,
		cmx, cmy,
		jsize, jlittle,
		istep, ivstep;
	int i, ifiy, iret;
	long li, lj, lstep;
	sampI
		*raf_linespace,
		*raf_yspace,
		*step_line;
	short
		*rbase_filtere,
		**rbase_filtert;

	cmx= pa_width,
	cmy= pa_height;

	ift= aa_filter;

	if ((FILTER_BOX == ift) || (FILTER_TRI == ift))
		{ aa_width= 0; }
	jlittle= aa_samp;
	ie= aa_width;
	ibs= (2 * ie +1) * jlittle, ihs= ibs / 2;  if (jlittle & 1) ihs ++;
	jsize= jlittle *jlittle;

	li= jlittle * jlittle * (cmx +(2 * ie)), lj= cmx * ibs;
	raf_linespace= EMEM((li +lj), sampI);
	if (NULL == raf_linespace) return 1;
	raf_yspace= raf_linespace + li;

	if (FILTER_BOX == ift)
	{
		rbase_filtere= EMEM(256, short),
		rbase_filtert= NULL;
		if (NULL == rbase_filtere)
			{ EFREE(raf_linespace); return 1; }
	}
	else
	{
		li= (256 + 4096) * ihs;
		if (ie) li += 4096;  // window edge summation scratch
		rbase_filtere= EMEM(li, short),
		rbase_filtert= EMEM(ibs, short *);
		if ((NULL == rbase_filtere) || (NULL == rbase_filtert))
		{
			EFREE(raf_linespace);
			EUFREE(rbase_filtere); EUFREE(rbase_filtert);
			return 1;
		}
	}

	ei_init(raf_yspace);
	while (lj--)
	{
		raf_yspace-> csr= 0.0, raf_yspace-> csg= 0.0,
		raf_yspace-> csb= 0.0, raf_yspace-> csa= 0.0; 
		raf_yspace++;
	}
	
	rstep= workset.image, gstep= rstep +workset.page, bstep= gstep +workset.page, astep= bstep +workset.page;
	li= jlittle * jlittle * cmx;

	if (FILTER_BOX == ift)
	{
		ei_genbox((int) cmx, (int) jlittle, rbase_filtere);
		for (i= cmy, istep= 0; (i--); istep++)
		{
				/** load first pix row into sample */
			step_line= raf_linespace;
			for (lj= li; (lj--); step_line++)
			{
				step_line-> csr= *(rstep++),
				step_line-> csg= *(gstep++),
				step_line-> csb= *(bstep++),
				step_line-> csa= *(astep++);
			}
			ei_box(raf_linespace, (Pixel_Rec *) imbuffer);
			if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline(imbuffer, imptr);
				if (iret) RE
		}
	}
	else
	{
		li += 2 * aa_stretch * aa_samp;
		ei_genfilter(ift, (int) cmx, (int) cmy, (int) ie, (int) jlittle, rbase_filtere, rbase_filtert);
		ei_reset();

		for (i= cmy, ifiy= ie, istep= 1, ivstep= 0; (i--); istep++)
		{
				/** load first pix row into sample */
			step_line= raf_linespace;
			for (lstep= li; (lstep--); step_line++)
			{
				step_line-> csr= *(rstep++),
				step_line-> csg= *(gstep++),
				step_line-> csb= *(bstep++),
				step_line-> csa= *(astep++);
			}

				// call int_xrow for each
			ei_filtx(raf_linespace);		
				// call int_yrow when window is passed
			if (ifiy) ifiy --;
			else
			{
				ei_filty(istep, (Pixel_Rec *) imbuffer);
				if (pa_flip) cg_flipop(imbuffer);
				iret= wtga_scanline(imbuffer, imptr);
					if (iret) RE
			}
		}
		
		for (ifiy= ie; (ifiy --); )
		{
				// cleanup loop for end of window
				// output end pixels
			ei_finaly(ifiy, (Pixel_Rec *) imbuffer);
			if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline(imbuffer, imptr);
				if (iret) RE
		}
		EFREE(rbase_filtert);
	}

	EFREE(raf_linespace);
	EFREE(rbase_filtere);

	return 0;
}

static int	pw_ift;
static sampI	*pwi_linespace;
static short	*pwe_basefilter, **pwt_basefilter;

int	t_aapstart(ehdr_image *imptr)
{
	sampI	*pyspace;
	unsigned short
		ihs,
		jsize;
	long li, lj;

	pw_ift= aa_filter;
	if ((FILTER_BOX == pw_ift) || (FILTER_TRI == pw_ift))
		{ aa_width= 0; }

	ihs= aa_window / 2;  if (aa_samp & 1) ihs ++;
	jsize= aa_samp *aa_samp;

	li= aa_samp * aa_samp * (pa_width +(2 * aa_width)), lj= pa_width * aa_window;
	pwi_linespace= EMEM((li +lj), sampI);
	if (NULL == pwi_linespace) return 1;
	pyspace= pwi_linespace + li;

	if (FILTER_BOX == pw_ift)
	{
		pwe_basefilter= EMEM(256, short),
		pwt_basefilter= NULL;
		if (NULL == pwe_basefilter)
			{ EFREE(pwi_linespace); return 1; }
	}
	else
	{
		li= (256 + 4096) * ihs;
		if (aa_width) li += 4096;  // window edge summation scratch
		pwe_basefilter= EMEM(li, short),
		pwt_basefilter= EMEM(aa_window, short *);
		if ((NULL == pwe_basefilter) || (NULL == pwt_basefilter))
		{
			EFREE(pwi_linespace);
			EUFREE(pwe_basefilter); EUFREE(pwt_basefilter);
			return 1;
		}
	}

	ei_init(pyspace);
	while (lj--)
	{
		pyspace-> csr= 0.0, pyspace-> csg= 0.0,
		pyspace-> csb= 0.0, pyspace-> csa= 0.0; 
		pyspace ++;
	}
	
	li= aa_samp * aa_samp * pa_width;
	if (FILTER_BOX == pw_ift)
	{
		ei_genbox((int) pa_width, (int) aa_samp, pwe_basefilter);
	}
	else
	{
		li += 2 * aa_stretch * aa_samp;
		ei_genfilter(pw_ift, (int) pa_width, (int) pa_slot,
			(int) aa_width, (int) aa_samp,
			pwe_basefilter, pwt_basefilter);
		ei_reset();
	}

	return 0;
}

int	t_aapfilter(ehdr_image *imptr)
{
	byte *rstep, *gstep, *bstep, *astep;
	shurt
		cmx, cmy,
		istep, ivstep;
	int i, ifiy, iret;
	long li, lj, lstep;
	sampI *step_line;

	cmx= pa_width, cmy= pa_height;
	rstep= workset.image, gstep= rstep +workset.page, bstep= gstep +workset.page, astep= bstep +workset.page;

	if (FILTER_BOX == pw_ift)
	{
		li= aa_samp * aa_samp * pa_width;
		for (i= pa_slot, istep= 0; (i--); istep++)
		{
				/** load first pix row into sample */
			step_line= pwi_linespace;
			for (lj= li; (lj--); step_line++)
			{
				step_line-> csr= *(rstep++),
				step_line-> csg= *(gstep++),
				step_line-> csb= *(bstep++),
				step_line-> csa= *(astep++);
			}
			ei_box(pwi_linespace, (Pixel_Rec *) imbuffer);
			if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline(imbuffer, imptr);
				if (iret) RE
		}
	}
	else
	{
		li= aa_samp * aa_samp * (pa_width +(2 * aa_width));
		ei_reset();

		for (i= pa_slot, ifiy= aa_width, istep= 1, ivstep= 0; (i--); istep++)
		{
				/** load first pix row into sample */
			step_line= pwi_linespace;
			for (lstep= li; (lstep--); step_line++)
			{
				step_line-> csr= *(rstep++),
				step_line-> csg= *(gstep++),
				step_line-> csb= *(bstep++),
				step_line-> csa= *(astep++);
			}

				// call int_xrow for each
			ei_filtx(pwi_linespace);		
				// call int_yrow when window is passed
			if (ifiy) ifiy --;
			else
			{
				ei_filty(istep, (Pixel_Rec *) imbuffer);
				if (pa_flip) cg_flipop(imbuffer);
				iret= wtga_scanline(imbuffer, imptr);
					if (iret) RE
			}
		}
		
		for (ifiy= aa_width; (ifiy --); )
		{
				// cleanup loop for end of window
				// output end pixels
			ei_finaly(ifiy, (Pixel_Rec *) imbuffer);
			if (pa_flip) cg_flipop(imbuffer);
			iret= wtga_scanline(imbuffer, imptr);
				if (iret) RE
		}
	}

	return 0;
}

void	t_aapfinal()
{
	EFREE(pwi_linespace);
	EFREE(pwe_basefilter);
	EUFREE(pwt_basefilter);

	pwi_linespace= NULL,
	pwe_basefilter= NULL,
	pwt_basefilter= NULL;
}
