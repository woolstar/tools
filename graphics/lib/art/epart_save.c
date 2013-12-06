#include "basic.h"
#include "tscene.h"
#include "eart.h"
#include "eartlab.h"

extern int	mofield[];
extern int	mospan[];

static	FILE	*fart;

char *worldLabels[]= {"lt", "bm", "sp", "ob"};
char *elementLabels[]= {"light", "beam", "spot", "object"};
char *filterLabels[]= {"", "box", "gauss", "lanczos", "tri"};
char *motionLabels[]= {
	"", "position+target+up+fov", "color+position", 
	"color+position+target+fov", "color+target",
	"position+scale+q"
};

#define LB(X)	getLabel(X)

char *getLabel( int idx )
{
	int i;
	LabelRef *hlr;

	for (i=LabelCount, hlr=Labels; (i--); hlr++)
		if (idx == hlr->idx)
			return(hlr->label);
	return "";
}


char *mixLabel( tsc_texture *tex )
{	return (tex->lapply & TRS_TMIX) ? LB(_SURF_MIX) : "";	}

char *projCodeLabel(tsc_map *map)
{
	switch (map-> projcode)
	{
		case 1:	return "planar";
		case 2:	return "cubic";
		case 3:	return "cylnder";
		case 5:	return "sphere";
		case 4:	return "squarecyl";
	}
	return "";
}

char *mapCodeLabel(tsc_texture *tex)
{
	switch (tex->lapply & ~TRS_TMIX)
	{
		case TRS_TCOLOR :	return LB(_COLOR);

		case TRS_TAMBIENT : return  (tex->lset == 3) ? LB(_AMBIENT) : "ka";
		case TRS_TLUM :		return  (tex->lset == 3) ? LB(_SURF_LUMINANT) : "kl";
		case TRS_TDIFF :	return  (tex->lset == 3) ? LB(_SURF_DIFFUSE) : "kd";
		case TRS_TSPEC :	return  (tex->lset == 3) ? LB(_SURF_SPECULAR) : "ks";
		case TRS_TREFLECT : return  (tex->lset == 3) ? LB(_SURF_REFLECT) : "kr";
		case TRS_TGLASS :	return  (tex->lset == 3) ? LB(_SURF_GLASS) : "kt";

		case TRS_TALPHA :	return LB(_SURF_ALPHA);
		case TRS_TGLOS :	return LB(_SURF_GLOSS);
		case TRS_TINDX :	return LB(_SURF_INDEX);
		case TRS_TBUMP :	return LB(_SURF_BUMP);
		case TRS_TENVIR :	return LB(_SURF_RENVIR);
	}
	return "";
}

#define PRINT_COLOR(F, L, C) fprintf(F, "\t\t%s %g %g %g\n", L, C.r, C.g, C.b);

static void print_sequence( FILE *f, char *label, char *form, int format, int offset )
{
	int i;
	static char buf[128];

	buf[0]= '\0';
	tart_undo_seqname(buf, offset, form);
	fprintf(f, "\t%s %s", label, buf);
	if (format)
	{
		switch ((i= format) & 0x0f00)
		{
			case EV_VID: fprintf(f, " vid");  break;
			case EV_RGB: fprintf(f, " rgb");  break;
			case EV_TGA:
				fprintf(f, " tga");
				switch (i & 0x07)
				{
					case EVT_BT16: fprintf(f, "16");  break;
					case EVT_BT24: fprintf(f, "24");  break;
					case EVT_BT32: fprintf(f, "32");  break;
				}
				if (EVT_TP10 == (i & 0xf0)) fprintf(f, "-10");
				break;
			case EV_VAD: fprintf(f, " vad");  break;
			case EV_PICT: fprintf(f, " pict");  break; 
		}
		if (EV_FLIP & i) fprintf(f, " %s", LB(_SCENE_FLIP));
	}
	fprintf(f, "\n");
}


int element2motion(int elementType)
{
	switch (elementType) {
		case _E_LIGHT : return MO_LIGHT;
		case _E_SPOT : return MO_SPOT;
		case _E_BEAM : return MO_BEAM;
		case _E_OBJECT : return MO_OBJECT;
	}
	return 0;
}


void print_motion( FILE *f, int type, int track, ts_track *ref)
{
	int i, j, pos;
	long lset, lnex;
//return; // xxx

	if (type < MO_CAMERA || type > MO_OBJECT) 
		return;

	fprintf(f, "\t%s %s\n", LB(_MOTION), motionLabels[type]);

	if (track < 0)
	{
		fprintf(f, "\t%s\n", LB(_MATRIX));
		for (i=1; i<=*(ref->depth); i++)
		{
			fprintf(f, "\t\t& ");
			for (pos=1; pos<=12; pos++)
				fprintf(f, " %g", epart_motionvalue(track, pos, ref));
			fprintf(f, "\n");
		}
	}
	else
	{
		us *depthcheck;
		int maxdepth;
		for (i= mospan[type], depthcheck= ref-> depth + track -1, maxdepth= 0; (i --); depthcheck ++)
		{ if (*depthcheck > maxdepth) maxdepth= *depthcheck; }

		for (i=1; i<=maxdepth; i++)
		{
			fprintf(f, "\t\t& ");
			pos= track;
			for (lset= mofield[type]; (lset); )
			{
				lnex= lset & (~(lset -1));
				switch (lnex)
				{
					// Vectors:
					case MOTION_POS:
					case MOTION_TAR:
					case MOTION_COLOR:
					case MOTION_SCALE: 
					case MOTION_UP:
					case MOTION_ROT:
						for (j=0; j<3; j++)
							fprintf(f, " %g", epart_motionvalue(pos++, i, ref));
						break;
					// Values:
					case MOTION_FOV:
						fprintf(f, " %g", epart_motionvalue(pos++, i, ref));
						break;
					// Quaternion:
					case MOTION_Q:
						for (j=0; j<4; j++)
							fprintf(f, " %g", epart_motionvalue(pos++, i, ref));
						break;
				}
				lset -= lnex;
			}
			fprintf(f, "\n");
		}
	}
}


int		save_art(char *finame, tscene *sc)
{
	int i,j,k;
	char * p;

	tscene_camera	*tcam;
	ts_setup		*tset;
	tscene_element	*tele, **htele;
	te_light		*tlig;
	te_object		*tobj;
	tscene_surface	*tsur;
	tsc_map			*tmap;
	tsc_texture		*ttex;
	long ca, cb;

	if ((fart= fopen(finame, "w"))==NULL) RE


	// _SCENE_VIEW
	//		_SIZE,_SCENE_ASPECT,_SCENE_WINDOW,_SCENE_WIDEVIEW,_MOTION 
	for(i=0, tcam= sc-> cam.list; i<sc-> cam.count; i++, tcam++)
	{
		fprintf(fart, "%s\t%s\n", LB(_SCENE_VIEW), tcam-> name);
		fprintf(fart, "\t%s %d %d\n", LB(_SIZE), tcam-> x, tcam-> y );
		if (tcam-> attr & TSC_ASPECT)
				fprintf(fart, "\t%s %g 1.0\n", LB(_SCENE_ASPECT), tcam-> gaspect );
		if (tcam-> attr & TSC_WINDOW)
			fprintf(fart, "\t%s %d %d %d %d\n", LB(_SCENE_WINDOW), tcam-> wxa, tcam-> wxb, tcam-> wya, tcam-> wyb );
		if (tcam-> attr & TSC_WIDEVIEW)
			fprintf(fart, "\t%s\n", LB(_SCENE_WIDEVIEW) );

		print_motion(fart, MO_CAMERA, tcam-> motion, &(sc-> tracks));

		fprintf(fart, "\n");
		
	}


		/** _SCENE_SETUP  */
	tset= &(sc-> setup);
	fprintf(fart, "setup\n");

	if (tset-> adapt)
		{ fprintf(fart, "\tadapt %d\n", tset-> adapt); }
	if (tset-> attr & TSS_GAMMA) 
		{ fprintf(fart, "\tgamma %g\n", tset-> gamma); }
	fprintf(fart, "\tdepth %d %d\n", tset-> rdepth, tset-> gdepth);
	if (tset-> curcam)
		{ fprintf(fart, "\tactive %s\n", tset-> curcam-> name); }

	print_sequence(fart, "image", tset-> image.imageform, tset-> image.iformat, tset-> image.ioffset);
	switch (tset-> bg.attr)
	{
		case 1: fprintf(fart, "\tbackground color %g %g %g\n", C_expandl(tset-> bg.cbase)); break;
		case 2: fprintf(fart, "\tbackground image %s\n", tset-> bg.imagename); break;
	}

	fprintf(fart, "\tambient %g %g %g\n", C_expandl(tset-> ambient));
	if ((tset-> filtertype >= _F_BOX) && (tset-> filtertype <= _F_LANCZOS))
		fprintf(fart, "\tfilter %s %d\n", filterLabels[tset-> filtertype], tset-> filterwidth);

	fprintf(fart, "\n");


	// (Elements is a list of lights, spots, beams, and objects)
	// _SCENE_LIGHT
	//		_SCENE_BRIGHT,_SAFE,_SCENE_SHADOW,_SCENE_FALLOFF*,
	//		_SCENE_PARENT,_MOTION
	// _SCENE_SPOT
	//		_SCENE_BRIGHT,_SCENE_CONE,_SCENE_SHADOW,_SCENE_BUFFER,
	//		_SCENE_BUFTOL,_SCENE_SOFT,_SAFE,_SCENE_FALLOFF*,_SCENE_PARENT,
	//		_MOTION
	// _SCENE_BEAM
	//		_SCENE_BRIGHT,_SCENE_SHADOW,_SCENE_BUFFER,_SCENE_BUFTOL
	//		_SCENE_SOFT,_SAFE,_SCENE_PARENT,_MOTION
	// _SCENE_OBJECT
	//		_SCENE_USE,_SCENE_PARENT,_MOTION
	for(i=0, tele= sc-> elem.list; i<sc-> elem.count; i++, tele++)
	{
		fprintf(fart, "%s %s\n", elementLabels[tele-> type], tele-> name);
		switch (tele-> type) {
			case _E_LIGHT :
			case _E_SPOT :
			case _E_BEAM :
				tlig= &(tele-> dat.light);
				fprintf(fart, "\t%s %g\n", LB(_SCENE_BRIGHT), tlig-> bright);
				if ( tele-> attr & TSL_SAFE ) 
					fprintf(fart, "\t%s %g\n", LB(_SAFE), tlig-> safe);
				if ( tele-> attr & TSL_SHADOW ) 
					fprintf(fart, "\t%s\n", LB(_SCENE_SHADOW));
				if ( tele-> attr & TSLS_CONE ) 
					fprintf(fart, "\t%s %g\n", LB(_SCENE_CONE), tlig-> atten);
				if ( tele-> attr & TSL_SOFT ) 
					fprintf(fart, "\t%s %d %g\n", LB(_SCENE_SOFT), tlig-> softrad, tlig-> soft);
				if (tele-> attr & TSL_BUFFER)
				{
					fprintf(fart, "\t%s %d %d\n", LB(_SCENE_BUFFER), tlig-> bufferx, tlig-> buffery);
					fprintf(fart, "\t%s %g\n", LB(_SCENE_BUFTOL), tlig-> buffertol);
				}
				break;
			case _E_OBJECT :
				tobj= &(tele->dat.object);
				print_sequence(fart, LB(_SCENE_USE), tobj-> srcnameform, 0, tobj-> ioffset);
				if ((tele-> attr & TSO_CMD) && tele-> cmd)
				{
					p= strrchr(tele-> cmd-> cmdparams, ' ');
					if (! p) { p= "-"; }
						else *(p ++)= '\0';
					fprintf(fart, "\tserv %s %d\n\t\t'%s'\n\t\t%s\n",
						tele-> cmd-> cmdstring,
						tele-> cmd-> cmdport,
						tele-> cmd-> cmdparams, p);
				}
				break;
		}

		// motion
		print_motion(fart, element2motion(tele-> type), tele-> motion, &(sc-> tracks));

		// parent
		if (tele-> parent)
			/* fprintf(fart, "\tparent %s\n", ((tscene_element *)(tele-> parent))-> name); */
			fprintf(fart, "\tparent %s\n", (tele-> parent)-> name);

		fprintf(fart, "\n");
	}


	// _SCENE_MATERIAL (material is a list of surfaces)
	//		_COLOR,_AMBIENT,_SURF_LUMINANT,_SURF_DIFFUSE,_SURF_SPECULAR
	//		_SURF_REFLECT,_SURF_GLASS,_SURF_MIXGLOSS*,
	//		_SURF_KA,_SURF_KL,_SURF_KD,_SURF_KS,_SURF_KR,_SURF_KT,
	//		_SURF_ALPHA,_SURF_ALPHALILTE,_SURF_GLOSS,_SURF_INDEX,_SURF_INVIS,
	//		_SURF_TEXTURE*,_SOLID*
	fprintf(fart, "%s\n", LB(_SCENE_MATERIAL) );
	for(i=0, tsur= sc-> mat.list; i<sc-> mat.count; i++, tsur++)
	{
		ca= tsur-> attr,
		cb= tsur-> amix;

		fprintf(fart, "\tsurface %s\n", tsur-> sname);

		if (! ca) continue;
		ca &= ~ cb;

		if (cb)
			{ PRINT_COLOR(fart, "color", tsur-> base); }

		// attribute colors
		if (ca & TRS_HAMBIENT)
			PRINT_COLOR(fart, LB(_AMBIENT), tsur-> ca); 
		if (ca & TRS_HLUMEN)
			PRINT_COLOR(fart, LB(_SURF_LUMINANT), tsur-> cl); 
		if (ca & TRS_HDIFF)
			PRINT_COLOR(fart, LB(_SURF_DIFFUSE),  tsur-> cd); 
		if (ca & TRS_HSPEC)
			PRINT_COLOR(fart, LB(_SURF_SPECULAR),  tsur-> cs); 
		if (ca & TRS_HREFLECT)
			PRINT_COLOR(fart, LB(_SURF_REFLECT),  tsur-> cr); 
		if (ca & TRS_HGLASS)
			PRINT_COLOR(fart, LB(_SURF_GLASS),  tsur-> ct);

		// attribute values			
		if (cb & TRS_HAMBIENT)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KA), tsur-> ka); 
		if (cb & TRS_HLUMEN)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KL), tsur-> kl); 
		if (cb & TRS_HDIFF)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KD), tsur-> kd); 
		if (cb & TRS_HSPEC)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KS), tsur-> ks); 
		if (cb & TRS_HREFLECT)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KR), tsur-> kr); 
		if (cb & TRS_HGLASS)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_KT), tsur-> kt); 

		// misc.
		if (tsur-> attr & TRS_ALPHA)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_ALPHA), tsur-> alpha); 
		if (tsur-> attr & TRS_ALPHALITE)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_ALPHALITE), tsur-> alpha); 
		if (tsur-> attr & TRS_HSPEC)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_GLOSS), tsur-> spec); 
		if (tsur-> attr & TRS_HGLASS)
			fprintf(fart, "\t\t%s %g\n", LB(_SURF_INDEX), tsur-> indx); 
		if (tsur-> attr & TRS_INVIS)
			fprintf(fart, "\t\t%s\n", LB(_INVIS) ); 

		// Textures
		// _SURF_MAP,_SURF_PROJECT,_SURF_WRAP
		for(j=0, tmap= tsur-> maplist; j<tsur-> mapcount; j++, tmap++)
		{
			fprintf(fart, "\t\t%s\n", LB(_SURF_TEXTURE));
			if (tmap->mapcount)
			{
				for(k=0, ttex= tmap-> imagelist; k<tmap-> mapcount; k++, ttex++)
				{
					fprintf(fart, "\t\t\t%s %s %s %s\n",
						LB(_SURF_MAP), mixLabel(ttex), mapCodeLabel(ttex), ttex->name); 
				}
			}
			if (tmap->projcode)
			{
				fprintf(fart, "\t\t\t%s %s ", LB(_SURF_PROJECT), projCodeLabel(tmap) );
				fprintf(fart, "%g %g %g %g %g %g ", M_expand(tmap->scale), M_expand(tmap->offset));
				fprintf(fart, "%g %g ", tmap->postsx, tmap->postsy);
				fprintf(fart, "%g %g\n", tmap->postx, tmap->posty);
			}
		}


		fprintf(fart, "\n");
	}

	// _SCENE_WORLD, _SCENE_OBJECT (world is a list of object names) 
	fprintf(fart, "%s\n", LB(_SCENE_WORLD) ); 	
	for(i=0, htele= sc-> set.list; i<sc-> set.count; i++, htele++)
	{
		tele= *htele;
		if (tele-> type == _E_OBJECT)
		{
			fprintf(fart, "\t%s %s\n", LB(_SCENE_OB), tele-> name); 
		}
	}
	fprintf(fart, "\n");

	fclose(fart);

	return 0;

}  // save_art()
