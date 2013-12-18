#define	FILTER_BOX	1
#define FILTER_TRI		2
#define FILTER_GAUSS	3
#define FILTER_SINC	4

#define FILT_TOLERANCE  1e-10
#define FILT_SINC_CONST  1.3

#define	filter_point	sampS

typedef struct { unsigned short csr, csg, csb, csa; } sampI;
typedef struct { float csr, csg, csb, csa; } sampS;

typedef struct
{
	long
		boolap,
		hash_a, hash_b;
	sampS v;
} er_adapt;

typedef struct
{
	unsigned char
		r, g, b, a;
} er_pixel;

#define	Pixel_Rec	er_pixel

