#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"
#include "eart.h"

static int	pl_light(tlist_element * ref, ts_track * tkptr, FILE * f)
{
	long t = ref-> type;
	te_light * te = & (ref-> dat.light);

	switch (t)
	{
		case _E_LIGHT:
			fprintf(f, "light %s\n", ref-> name);
			break;
		case _E_BEAM:
			fprintf(f, "beam %s\n", ref-> name);
			break;
		case _E_SPOT:
			fprintf(f, "spot %s\n", ref-> name);
			break;
		default:
			return 1;
	}

	fprintf(f, "\tbright %g\n", te-> bright);
	if (ref-> parent) { fprintf(f, "\tparent %s\n", ref-> parent-> name); }
	if (ref-> attr & TSL_SHADOW) { fprintf(f, "\tshadow\n"); }
		else if (ref-> attr & TSL_BUFFER)
	{
		fprintf(f, "\tbuffer %d %d\n", te-> bufferx, te-> buffery);
		fprintf(f, "\tbuffertol %g\n", te-> buffertol);
		if (ref-> attr & TSL_SOFT) fprintf(f, "\tsoft %d %g\n", te-> softrad, te-> soft);
		if (ref-> attr & TSL_SAFE) fprintf(f, "\tsafe %g\n", te-> safe);
	}
	if (ref-> attr & TSL_FALLOFF)
	{
		switch (ref-> attr & 0x0f)
		{
			case 1:
				fprintf(f, "\tfalloff flat %g\n", te-> fall.x);
				break;
			case 2:
				fprintf(f, "\tfalloff linear %g\n", te-> fall.y);
				break;
			case 4:
				fprintf(f, "\tfalloff real %g\n", te-> fall.z);
				break;
			case 7:
				fprintf(f, "\tfalloff complex %g %g %g\n", M_expand(te-> fall));
				break;
		}
	}
	if (ref-> attr & TSLS_CONE) { fprintf(f, "\tcone %g\n", te-> atten); }
		else if (ref-> attr & TSLS_POWER) { fprintf(f, "\tpower %g\n", te-> atten); }

	epartmo_print(f, t, 0, ref, tkptr);
	fprintf(f, "\n");
	return 0;
}

static int	pl_object(tlist_element * ref, ts_track * tkptr, FILE * f)
{
	long t = ref-> type;
	char str[256], * p;

	if (_E_OBJECT != t)
		return 1;

	fprintf(f, "object %s\n", ref-> name);
	tart_undo_seqname(str, ref-> dat.object.ioffset, ref-> dat.object.srcnameform);
	fprintf(f, "\tuse %s\n", str);
	if (ref-> parent) { fprintf(f, "\tparent %s\n", ref-> parent-> name); }
	if ((ref-> attr & TSO_CMD) && ref-> cmd)
	{
		p= strrchr(ref-> cmd-> cmdparams, ' ');
		if (! p) { p= "-"; }
			else *(p ++)= '\0';
		fprintf(f, "\tserv %s %d\n\t\t'%s'\n\t\t%s\n",
			ref-> cmd-> cmdstring,
			ref-> cmd-> cmdport,
			ref-> cmd-> cmdparams, p);
	}
	if (ref-> attr & TSO_INVIS)
	{
		fprintf(f, "\tinvis\n");
	}

	epartmo_print(f, t, 0, ref, tkptr);
	fprintf(f, "\n");
	return 0;
}

int	print_art(char *name, tscene *ptr)
{
	int istep, iret;
	tscene_camera	*tcam;
	tscene_surface * tsurf;
	FILE * fdest;

	fdest= fopen(name, "w");
	if (NULL == fdest)
			return 1;

	for (istep= ptr-> cam.count, tcam= ptr-> cam.list; (istep --); tcam ++)
		{ if (iret= epart_printcam(tcam, & (ptr-> tracks), fdest)) { fclose(fdest);  return 1; }}

	iret= epart_printsetup(& (ptr-> setup), fdest);
		if (iret) { fclose(fdest);  return 1; }

	if (ptr-> mat.count)
	{
		fprintf(fdest, "material\n");
		for (istep= ptr-> mat.count, tsurf= ptr-> mat.list; (istep --); tsurf ++)
		{
			iret= epart_printsurf(tsurf, fdest);
			if (iret) { fclose(fdest);  return 1; }
		}
	}

	iret= epart_printcontain(& (ptr-> set), NULL, fdest);
		if (iret) { fclose(fdest);  return 1; }

	fclose(fdest);

	return 0;
}

int  print_artlist(char * name, tlist_scene * ptr)
{
	int istep, iret;
	tscene_camera	*tcam;
	tlist_element * telem;
	tscene_surface * tsurf;
	FILE * fdest;

	fdest= fopen(name, "w");
	if (NULL == fdest)
			return 1;

	for (istep= ptr-> cam.count, tcam= ptr-> cam.list; (istep --); tcam ++)
		{ if (iret= epart_printcam(tcam, & (ptr-> tracks), fdest)) { fclose(fdest);  return 1; }}

	iret= epart_printsetup(& (ptr-> setup), fdest);
		if (iret) { fclose(fdest);  return 1; }

	for (telem= ptr-> elem.lights; (telem); telem= telem-> next)
		{ if (iret=pl_light(telem, & (ptr-> tracks), fdest)) { fclose(fdest);  return 1; } }
	for (telem= ptr-> elem.objects; (telem); telem= telem-> next)
		{ if (iret=pl_object(telem, & (ptr-> tracks), fdest)) { fclose(fdest);  return 1; } }

	if (ptr-> mat.count)
	{
		fprintf(fdest, "material\n");
		for (istep= ptr-> mat.count, tsurf= ptr-> mat.list; (istep --); tsurf ++)
		{
			iret= epart_printsurf(tsurf, fdest);
			if (iret) { fclose(fdest);  return 1; }
		}
	}

	iret= epart_printcontain(& (ptr-> set), ptr-> elem.objects, fdest);
		if (iret) { fclose(fdest);  return 1; }

	fclose(fdest);
	return 0;
}

