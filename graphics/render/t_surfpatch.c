#include "trun.h"

extern long		cobjects;
extern tr_object	*tolist;
extern tlist_scene	tfocus;

#if 0
		/*** allocate texture space */
	imapsize= 0, imapuse= 0;
	for (istep= cts_parse, epmatch= epsurfp; (istep ++); epmatch --)
	{
		if (! epmatch-> tm_count) continue;
		imapsize += epmatch-> tm_count, imapuse ++;
	}
	if (imapuse)
	{
		ttbase= EMEM(imapsize, tsc_texture);
		tnbase= EMEM(imapuse, tsc_map);
		if ((NULL == ttbase) || (NULL == tnbase))
			{ printf(" >> map memory error.\n");  RE }
	}
#endif

	/*** patch scene material into tscene_surface in tr_object */
void		tsurf_patch(tr_object *ptr)
{
	int istep, jstep, ie;
	tscene_surface *tsstep, *worldmatch;

	for (istep= ptr-> ta.surface_c, tsstep= ptr-> ta.slist, ie= 0; (istep --); tsstep ++, ie= 1)
	{
		for (jstep= tfocus.mat.count, worldmatch= tfocus.mat.list;
			(jstep && strcmp(tsstep-> sname, worldmatch-> sname)); jstep --, worldmatch ++) ;
		if (! jstep) { if (ie) printf(" >> unmatched surface %s\n", tsstep-> sname);  continue; }

		* tsstep= * worldmatch;
	}
}

void		tsurf_stampmap(int n, ev_mapdat * step, tscene_surface * src)
{
	long ka= src-> attr, kb= src-> amix;
	extern double val_ambient;

	for (; (n --); step ++)
	{
		if (ka & TRS_HAMBIENT)
		{
			if (kb & TRS_HAMBIENT) { step-> cx_a= src-> ka * val_ambient; }
				else { C_blend(step-> cma, tfocus.setup.ambient, src-> ca) }
		}
		step-> mapfin= kb; /*** BAD !! FIX ***/
		step-> cx_l= src-> kl, C_cpy(step-> cml, src-> cl)
		step-> cx_d= src-> kd, C_cpy(step-> cmd, src-> cd)
		step-> cx_s= src-> ks, C_cpy(step-> cms, src-> cs)
		step-> cx_r= src-> kr, C_cpy(step-> cmr, src-> cr)
		step-> cx_t= src-> kt, C_cpy(step-> cmt, src-> ct)
	}
}

void		tsurf_repatch(tscene_surface *tptr)
{
	int istep, jstep;
	us newfaceset, faceattrhold;
	long lold, lnew;
	tr_object *tlist;
	tscene_surface *tsmatch;
	er_surface *erdest;
	ev_surfbsi *basiscostep;
	er_face *xfacestep;

	for (istep= cobjects, tlist= tolist; (istep --); tlist ++)
	{
		if (! tlist-> ta.surface_c || (NULL == tlist-> ta.sdata.erbasis))
			continue;
		for (jstep= tlist-> ta.surface_c, tsmatch= tlist-> ta.slist, basiscostep= tlist-> ta.sdata.erbasis;
			(jstep && strcmp(tptr-> sname, tsmatch-> sname)); tsmatch ++, basiscostep ++) jstep --;
		if (jstep)
		{
			erdest= basiscostep-> surfp;
			if (! erdest) continue;
			lold= erdest-> attrib;
			tsurf_convertr(erdest, tptr);
			lnew= erdest-> attrib;
			if (NULL != basiscostep-> surfmix)
				* (basiscostep-> surfmix)= * erdest;
			if ((lold ^ lnew) & (ERS_INVIS | ER_TRANSMT))
			{
				newfaceset= 0;
				if (lnew & ERS_INVIS) newfaceset= RACE_INVIS;
					else if (lnew & ER_TRANSMT) newfaceset= RACE_LENS;
				for (jstep= tlist-> ta.poly_c, xfacestep= (er_face *) tlist-> ta.flist; (jstep --); xfacestep ++)
				{
					if (xfacestep-> shading != basiscostep) continue;
					faceattrhold= xfacestep-> bits & ~ (RACE_INVIS | RACE_LENS);
						xfacestep-> bits= faceattrhold | newfaceset;
				}
			}

			if (basiscostep-> maplist)
				tsurf_stampmap(basiscostep-> mapct, basiscostep-> maplist, tptr);

			if (lold & ER_ENVIR) erdest-> attrib |= ER_ENVIR;
			* tsmatch= * tptr;
		}
	}
}

void	tsurf_patchambient(void)
{
	int istep, jstep, kstep;
	color ca;
	tscene_surface *tsstep;
	tr_object *tlist;
	ev_surfbsi *bastep;
	ev_mapdat * mapstep;
	er_surface *erdest;

	extern double val_ambient;

	for (istep= cobjects, tlist= tolist; (istep --); tlist ++)
	{
		if (! tlist-> ta.surface_c || (NULL == tlist-> ta.sdata.erbasis))
			continue ;
		for (jstep= tlist-> ta.surface_c, tsstep= tlist-> ta.slist,
			bastep= tlist-> ta.sdata.erbasis; (jstep --); tsstep ++, bastep ++)
		{
			if (! tsstep-> attr & TRS_HAMBIENT) continue;
			erdest= bastep-> surfp;
				if (! erdest) continue;

			if (tsstep-> amix & TRS_HAMBIENT)
					{ C_blend(ca, (float) tfocus.setup.ambient, tsstep-> ka * tsstep-> base) }
				else { C_blend(ca, (float) tfocus.setup.ambient, tsstep-> ca) }
			C_cpy(erdest-> col_a, ca)
			if (bastep-> surfmix)
				{ C_cpy(bastep-> surfmix-> col_a, ca) }

			if (bastep-> maplist)
			{
				for (kstep= bastep-> mapct, mapstep= bastep-> maplist; (kstep --); mapstep ++)
				{
					if (tsstep-> attr & TRS_HAMBIENT)
					{
						if (tsstep-> amix & TRS_HAMBIENT) { mapstep-> cx_a= tsstep-> ka * val_ambient; }
							else { C_blend(mapstep-> cma, tfocus.setup.ambient, tsstep-> ca) }
					}
				}
			}
		}
	}
}
