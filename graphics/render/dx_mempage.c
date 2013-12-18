#include "rrun.h"
#include "rqdata.h"

svector		*workspace;
dx_point	*workpoint;
dx_edge		*workedge, *workefar;

q_image		workset ;

extern	int		aa_samp, aa_width, aa_stretch, aa_window;

int	w_tilermem(long nv, long nsi)
{
	workefar= NULL;
	workspace= EMEM(nv, svector);
		if (NULL == workspace) RE
	workpoint= EMEM(nsi, dx_point);
		if (NULL == workpoint) { EFREE(workspace);  RE }
	workedge= EMEM(nsi, dx_edge);
		if (NULL == workedge) { EFREE(workspace); EFREE(workpoint);  RE }

	workefar= workedge +(nsi-1);
	return 0;
}

int	w_tilerimage(long x, long y, long slot, int filter, int adapt)
{
	long l;

	memset(& workset, 0, sizeof(q_image)) ;
	if (adapt)
		{ l= adapt * adapt * (x + 2 * aa_width) * (slot + 2 * aa_width); }
		else l= x * slot;

	workset.image= EMEM((4 * l), unsigned char);
	workset.depth= EMEM(l, unsigned long);
	workset.ref= EMEM(l, unsigned long);
	workset.normal= EMEM(l, svector);

	if ((NULL == workset.image) || (NULL == workset.depth) ||
		(NULL == workset.ref) || (NULL == workset.normal))
	{
		EUFREE(workset.image);  EUFREE(workset.ref);
		EUFREE(workset.depth);  EUFREE(workset.normal);
		return 1;
	}

	workset.width= x, workset.page= l, workset.height= y;
	if (adapt)
	{
		workset.width *= adapt, workset.height *= adapt;
		if (filter)
			{ workset.width += 2 * filter * adapt, workset.height += 2 * filter * adapt; }
	}
	return 0;
}

void	w_tilerclear(void)
{
	long l;
	lung *dstep;

	memset(workset.image, 0, workset.page * 4 * sizeof(unsigned char)) ;
	memset(workset.ref, 0, workset.page * sizeof(unsigned long)) ;
	for (l= workset.page, dstep= workset.depth; (l --); )
		{ *(dstep ++)= 0xfffffffe; }
}

void	w_tilepadrel(void)
{
	EUFREE(workspace);
		workspace= NULL;
	EUFREE(workpoint);
		workpoint= NULL;
	EUFREE(workedge);
		workedge= NULL;
}

void	w_tilerrel(void)
{
	EFREE(workset.image);
	EFREE(workset.ref);
	EFREE(workset.depth);
	EFREE(workset.normal);

	memset(& workset, 0, sizeof(q_image)) ;
}

