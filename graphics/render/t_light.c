#include	"trun.h"

int			ct_lights= 0;
ev_light	*magic_light = NULL ;

extern int			global_time;
extern tlist_scene		tfocus;

extern void		tgeom_solidrange(void), tgeom_depth(void),
				tsurf_patchambient(void);
extern double	epart_motionvalue(int, tlist_element *, int, ts_track *);
extern void		epart_motionvec(svector *, int, tlist_element *, int, ts_track *);

extern int	rvm_shadowcolumn(ev_light *, int, int),
	rvm_shadowcube(ev_light *, int, int),
	rvm_shadowbuffer(ev_light *, int, int) ;
extern void	rvm_destroy(void) ;


int	tlight_create(void)
{
	unsigned short
		j, k, ctl;
	unsigned int trackoff;
	double dtmp, dtnp;
	svector
		FromPoint,
		LDirCalc;
	color WorkColorVar;
	tlist_element *elsearch;
	ev_light *lstep;

	ct_lights= 0;
	for (ctl= 0, elsearch= tfocus.elem.lights; (elsearch); elsearch= elsearch-> next)
	{
		j= elsearch-> type;

		if ((j!= _E_LIGHT) && (j!= _E_BEAM) && (j!= _E_SPOT)) continue;
		ctl ++;
	}

	if (! ctl)
	{
		ct_lights= 0;
		magic_light= NULL ;
		return 0;
	}

	magic_light= EMEM(ctl, ev_light);
	if (NULL == magic_light) RE
	ct_lights= ctl;

	for (elsearch= tfocus.elem.lights, lstep= magic_light;
		(elsearch && ctl); elsearch= elsearch-> next )
	{
		j= elsearch-> attr, k= elsearch-> type;
		if ((k!= _E_LIGHT) && (k!= _E_BEAM) && (k!= _E_SPOT)) continue;

		trackoff= 0;
			/* if (! tracknum) continue; */

		lstep-> tipe= 0,
		lstep-> bits= 0;
		lstep-> samples= 0,
		lstep-> lshadmap= NULL,
		lstep-> lprojmap= NULL,
		lstep-> cache_1= NULL,
		lstep-> cache_2= NULL;
		lstep-> safedist= 1e-12;

		epart_motionvec((svector *) & WorkColorVar,
			trackoff, elsearch, global_time +1, & (tfocus.tracks));
		trackoff += 3;

		dtmp= elsearch-> dat.light.bright;
		if (dtmp == 0.0) continue;
		C_cpy(lstep-> color, dtmp * WorkColorVar)

		if (j & TSL_SHADOW)
		{
			lstep-> bits |= ERLA_SHAD;
/*
//			if (el_search-> u_light.auxbits & ELAUX_SHSOLID)
//				{ lstep-> bits |= ERLA_SHADSOLID; }
*/
			if (j & TSL_SAFE)
				{ lstep-> safedist= elsearch-> dat.light.safe; }
		}
		if (j & TSL_SOFT)
		{
			k= elsearch-> dat.light.softrad;
			if (k)
			{
				lstep-> bits |= ERLA_SOFT;
				lstep-> samples= k,
				lstep-> zone_weight= 1.0 / (double) k;
				lstep-> zone_d= elsearch-> dat.light.soft;
			}
		}
		if (j & TSL_FALLOFF)
		{
			k= (j & 7), lstep-> bits|= ERLA_FALL +k;
			lstep-> falloff.x= (j & 4) ? elsearch-> dat.light.fall.x : (float) 0.0,
			lstep-> falloff.y= (j & 2) ? elsearch-> dat.light.fall.y : (float) 0.0,
			lstep-> falloff.z= (j & 1) ? elsearch-> dat.light.fall.z : (float) 0.0;
		}

		switch (elsearch-> type)
		{
			case _E_LIGHT:
				lstep-> tipe= ERL_POINT;
				epart_motionvec(& (lstep-> pt), trackoff, elsearch, global_time +1, & (tfocus.tracks));
					trackoff += 3;

				if (j & TSL_BUFFER)
				{
					lstep-> bits |= ERLA_BUFFER;

					if (j & TSL_SAFE)
						{ lstep-> safedist= elsearch-> dat.light.safe; }
					if ( rvm_shadowcube(lstep, elsearch-> dat.light.bufferx, elsearch-> dat.light.buffery))
						{ printf(" >> shadow buffer create error for light %s\n", elsearch-> name); }
				}
				break;

			case _E_SPOT:
				lstep-> tipe= ERL_SPOT;
				
				epart_motionvec(& FromPoint, trackoff, elsearch, global_time +1, & (tfocus.tracks));
					trackoff += 3;
				lstep-> pt= FromPoint;
				epart_motionvec(& LDirCalc, trackoff, elsearch, global_time +1, & (tfocus.tracks));
					trackoff += 3;
				M_minus(LDirCalc, FromPoint)  M_selfnorm(LDirCalc)
				M_cpy(lstep-> direc, LDirCalc)

				dtmp= epart_motionvalue(trackoff, elsearch, global_time +1, & (tfocus.tracks));
					trackoff ++;
				dtmp= cos (dtmp * M_PI / 180.0);
				if (dtmp < 0.0) { dtmp= 0.0; }
				lstep-> cone_edge= dtmp;

				if (j & TSL_BUFFER)
				{
					lstep-> bits |= ERLA_BUFFER;

					if (j & TSL_SAFE)
						{ lstep-> safedist= elsearch-> dat.light.safe; }
					if ( rvm_shadowbuffer(lstep, elsearch-> dat.light.bufferx, elsearch-> dat.light.buffery))
						{ printf(" >> shadow buffer create error for spot %s\n", elsearch-> name); }
				}

				if (j & TSLS_POWER)
				{
					lstep-> bits |= ERLA_POWER;
					lstep-> cone_scale= elsearch-> dat.light.atten;
					lstep-> cone_edge= 0.0;
				}
				else
				{
					if (j & TSLS_CONE)
					{
						dtmp= cos(elsearch-> dat.light.atten * M_PI / 180.0);
						if ((dtmp >= 0.0) && ((dtmp -lstep-> cone_edge) > 1e-10))
						{
							lstep-> bits |= ERLA_CONE;
							dtnp= 1.0 / (dtmp - lstep-> cone_edge);
							lstep-> cone_bound= dtmp,
							lstep-> cone_scale= dtnp;
						}
					}
#if 0
					if (j & ELM_LENS)
					{
						for (j= el_search-> ct_extent, extn_search= el_search-> modif;
							(j & (extn_search-> code != ELME_LENS)) ; extn_search ++) j--;
						if (j)
						{
							k= extn_search-> track;

							BF_TAG(dtmp)
							dtmp= cos(dtmp * M_PI / 180.0);
							dtnp= lstep-> cone_edge;
							if ((dtmp >= 0.0) && ((dtnp - dtmp) > 1e-10))
							{
								lstep-> bits |= ERLA_CONE;
								dtnp= 1.0 / (dtnp -dtmp);
								lstep-> cone_bound= dtmp,
								lstep-> cone_scale= dtnp;
							}
						}
					}
#endif
				}
				break;

			case _E_BEAM:
				lstep-> tipe= ERL_BEAM;
				epart_motionvec(& LDirCalc, trackoff, elsearch, global_time +1, & (tfocus.tracks));

				M_selfnorm(LDirCalc)
				lstep-> direc= LDirCalc;

				if (j & TSL_BUFFER)
				{
					lstep-> bits |= ERLA_BUFFER;

					if (j & TSL_SAFE)
						{ lstep-> safedist= elsearch-> dat.light.safe; }
					if ( rvm_shadowcolumn(lstep, elsearch-> dat.light.bufferx, elsearch-> dat.light.buffery))
						{ printf(" >> shadow buffer create error for beam %s\n", elsearch-> name); }
				}
				break;
		}
		ctl --, lstep ++;
	}

	if (ctl)
		{ ct_lights -= ctl ; }
	return 0;
}

void	tlight_updateambient(void)
{
	tsurf_patchambient();
}

int	tlight_updatecolor(void)
{
	unsigned short
		k, ctl, ctv;
	unsigned int trackoff;
	double dtmp;
	color WorkColorVar;

	tlist_element *elsearch;
	ev_light *lstep;

	for (ctv= 0, elsearch= tfocus.elem.lights; (elsearch); elsearch= elsearch-> next )
	{
		k= elsearch-> type;
		if ((k!= _E_LIGHT) && (k!= _E_BEAM) && (k!= _E_SPOT)) continue;

		trackoff= 0;
		epart_motionvec((svector *) & WorkColorVar,
			trackoff, elsearch, global_time +1, & (tfocus.tracks));
		dtmp= elsearch-> dat.light.bright ;

		if ((dtmp > 1e-8) && (C_nonzero(WorkColorVar)))
			ctv ++ ;
	}

	if (! ct_lights)
		{ return tlight_create(); }

	if (ct_lights != ctv)
	{
		rvm_destroy();
		return tlight_create();
	}
	
	ctl= ct_lights;
	for (elsearch= tfocus.elem.lights, lstep= magic_light;
		(elsearch && ctl); elsearch= elsearch-> next )
	{
		k= elsearch-> type;
		if ((k!= _E_LIGHT) && (k!= _E_BEAM) && (k!= _E_SPOT)) continue;

		trackoff= 0;
		epart_motionvec((svector *) & WorkColorVar,
			trackoff, elsearch, global_time +1, & (tfocus.tracks));
		dtmp= elsearch-> dat.light.bright;
		if (dtmp == 0.0)
		{
			C_zero(lstep-> color)
			continue;
		}
		C_cpy(lstep-> color, dtmp * WorkColorVar)
		lstep ++, ctl --;
	}

	return 0;
}

void	tlight_destroy( void )
{
	if ( magic_light )
	{
		EFREE( magic_light ) ;
		magic_light = NULL ;
	}
}
