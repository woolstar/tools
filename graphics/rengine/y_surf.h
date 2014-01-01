extern void	applybase(er_surface * ptr, scolor & abase) ;
extern void	applysurf(er_surface * ptr, aart::surface * asrc, ev_surfbsi *) ;
extern bool	applytexture(ev_mapping * destmap, ev_mapdat * destcore, er_surface * asrc, const aart::texture * atexture) ;
extern void	applytexturing(ev_surfbsi * adest, tbagstepper<aart::texture *> * astep, ev_mapdat * & aproj, ev_mapping * & amap) ;
