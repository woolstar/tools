#include <general.h>
#include <time.h>

#define Ra      1629
#define Rm      1048576

static unsigned long    ran_seed;

void    aran_initgen(void)
{
	time_t gt;

	time (& gt);
	ran_seed= gt % Rm;
}

int     aran_val(int iran)
{
	unsigned long  bigtmp;
	double scal;

	ran_seed= (long) (Ra * ran_seed) % Rm;
	scal= 1.0 * ran_seed * iran / Rm, bigtmp= (unsigned long) scal;

	return bigtmp;
}

unsigned long	aran_baseval(void)
{
	ran_seed= (long) (Ra * ran_seed) % Rm;

	return ran_seed ;
}

double  aran_vval(void)
{
	double scal;

	ran_seed= (long) (Ra * ran_seed) % Rm;
	scal= 1.0 * ran_seed / Rm;

	return scal;
}
