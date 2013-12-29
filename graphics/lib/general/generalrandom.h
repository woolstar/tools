#ifndef	GENERALRANDOM_DEF
#	define	GENERALRANDOM_DEF	1

#ifdef __cplusplus
	extern "C" {
#endif

void    aran_initgen(void) ;
int     aran_val(int) ;
unsigned long aran_baseval(void) ;
double  aran_vval(void) ;

#ifdef __cplusplus
	}
#endif
#endif	// GENERALRANDOM_DEF