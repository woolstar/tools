#include "basic.h"
#include "tscenelink.h"
#include "tfren.h"

	tscene	tfocus;

	/***/

extern void	parse_sco(),
			rran_initgen(),
			ew_init();

static char	ixpress[SLINE];
static void	cproc_help();

void	cboot_mem()
{
//	ew_init(&Rfocus);
}

void	cexec_stream(FILE *efs)
{
	char
		c, *p, *q,
		reline[SLINE], strchar[StrWORD];
	int 
		i, imh,
		in1, in2, in1s;

	* strchar= '\0';

	while (1)
	{
		if (efs == stdin)
			{ printf("pho%% ");  fflush(stdout); }
		if (!fgets(ixpress, SLINE, efs))  return ;
		p= ixpress;  P_space  P_end
		switch (*q)
		{
			case 'c':
				if Q_is("character")
				{
					if (!*p) break;  P_end
					strcpy(strchar, q);
				}
				break;
			case 'e':
				if Q_is("end") return;
				break;
			case 'f':
				if (Q_is("f") || Q_is("frame"))
				{
					i= atoi(p);
					if (i)
					{
						P_word
						if (*p)
						{
							if (q= strchr(p, '\n')) { *q= '\0'; }
								else { if (q= strchr(p, '\r')) { *q= '\0'; } }
							imh= tfocus.setup.image.icode, tfocus.setup.image.icode= 1;
							strcpy(reline, tfocus.setup.image.imageform);  strcpy(tfocus.setup.image.imageform, p);
						}
						if (cproc_render(i))
							{ if (efs != stdin) return; else continue; }
						if (*p)
						{
							tfocus.setup.image.icode= imh;
							strcpy(tfocus.setup.image.imageform, reline);
						}
					}
					break;
				}
				if (Q_is("fs") || Q_is("frames"))
				{
					in1= atoi(p);  
					if (in1)
					{
						P_word  in2= atoi(p);
						P_word  in1s= atoi(p);  if (!in1s) in1s= 1; else { P_word }
						if (*p)
						{
							if (q= strchr(p, '\n')) { *q= '\0'; }
								else { if (q= strchr(p, '\r')) { *q= '\0'; } }
							imh= tfocus.setup.image.icode, tfocus.setup.image.icode= 1;
							strcpy(reline, tfocus.setup.image.imageform);  strcpy(tfocus.setup.image.imageform, p);
						}
						for (i= in1; (i <= in2); i+= in1s)
							{ if (cproc_render(i)) { if (efs != stdin) return; else continue; } }
						if (*p)
						{
							tfocus.setup.image.icode= imh;
							strcpy(tfocus.setup.image.imageform, reline);
						}
					}
					break;
				}
				break;
			case 'q':
				return;
			case 's':
/*					  /// set option */
				if Q_is("scene")
				{
					if (!*p) break;  P_end
					if (tfocus.set.count)
						{ printf(" >> a scene is already loaded\n"); break; }
					if (cproc_sload(q, strchar)) return;
				}
				break;
			case 'h':
			case '?':
			default:
				cproc_help();
				break;
		}
	}
}

int	cproc_sload(char *s, char * na)
{
	FILE *f;
	char *p;
	tlist_scene  tlo;

	f= fopen(s, "r");
	if (f== NULL)
	{
/*    /// failed to open scene file */
		sprintf(ixpress, "unable to open file %s", s);
		visW_err("loading scene", ixpress);
		return 1;
	}
	fclose(f);

//	ew_init(&Rfocus);

	parse_artlist(s, & tlo);
	if (! tlo.set.count) return 1;

	if (tsimport_test(& (tlo.cam)) && tsimport_load(& (tlo.cam)))
		return 2;

#if 0
	if (tart_metatest(& tlo))
	{
		if (! * na) { printf(" >> no character specified for meta-art file.\n");  return 1; }
		tart_selectchar(& tfocus, & tlo, na);
	}
		else
#endif
		{ memcpy(& tfocus, & tlo, sizeof(tlist_scene)); }

	rran_initgen();
	return 0;
}

int	cproc_render(int FrameNumber)
{
	/**
	/// setup image buffer memory
	/// setup static scene memory
	use lights, use grid list

	/// step through xy rendering space
	/// cast rays
	/// do adaptive testing
	/// subsampling
	/// filtering

	/// close image file
	/// free workspace
	*/

	mlrender_frame(FrameNumber -1);
	return 0;
}

static void	cproc_help()
{
	printf("    cmd-pho commands\n");
	printf("scene Name\t\tload scene file Name\n");
	printf("frame N (ImageName)\trender frame N (to file ImageName)\n");
	printf("frames Start Stop\n");
	printf("(StepRate) (ImageNames)\trender multiple frames, from Start to Stop\n");
	printf("\t\t\t(advancing by StepRate) and/or (to files ImageNames)\n");
	printf("end or quit\t\tfinish and exit\n");
	printf("help or ?\t\tdisplay this help page\n");
}

