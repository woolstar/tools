#include "basic.h"
#include "tscenelink.h"

extern	tlist_scene	tfocus;

extern	void	cboot_mem(),
				cexec_stream();
extern	int	cproc_render();

	/***/

static	shurt	af_start= 0, af_end= 0, af_step= 1;

static	char	na_scen[StrWORD], na_char[StrWORD],
				na_batch[StrWORD],
				na_imname[StrNAME],
				ixpress[SLINE];
static	FILE	*f;

	/***/

void	main(int N, char **S)
{
	int i;
	char c, *p;

	cboot_mem();
	*na_scen= '\0', * na_char= '\0',
	*na_batch= '\0', *na_imname= '\0';
	S++, N--;

	if (N)
	while (N)
		switch(**S)
		{
			case '-':
				switch (S[0][1])
				{
					case 'f':
						if (N> 1)
						{
							S++, N--, p= *S;
							i= atoi(p);  if (!i) break;  af_start= i;
							while ((c= *p) && (c>= '0') && (c<='9')) p++;
							while ((c== *p) && ((c < '0') || (c > '9'))) p++;
							if (c && (i= atoi(p)) && (i > af_start)) af_end= i;
							while ((c= *p) && (c>= '0') && (c<='9')) p++;
							while ((c== *p) && ((c < '0') || (c > '9'))) p++;
							if (c && (i= atoi(p)) && (i > 0)) af_step= i;
							}
						break;
					case 'o':
						if (N> 1) { S++, N--;  strcpy(na_imname, *S); }
						break;
					case 'c':
						if (N> 1) { S++, N--;  strcpy(na_char, *S); }
						break;
					case 'r':
						if (N> 1) { S++, N--;  strcpy(na_batch, *S); }
						break;
					default:
						printf("no flag -%c\n\n", S[0][1]);
					case '-':
					case 'h':
						printf("azor 4.0 rel 1  Copyright (c) 1997 Twin Forces\n");
						printf("cmd-render usage:  azor [options] [SceneFile]\n");
						printf("  options,\n");
						printf("   -f n or -f n1-n2\tRender frame n, or range n1 through n2\n");
						printf("   -o ImageName\t\tRender to specified image file(s)\n");
						printf("   -r BatchFile\t\tRead commands from batch file\n");
						printf("\n");
						DIE
					}
				S++, N--;
				break;
			default:
				strcpy(na_scen, *S);
				S++, N--;
				break;
		}

	printf(" cmd-render (azor) 4.0 rel 1  Copyright (c) 1997 Twin Forces\n");

	if (*na_scen && cproc_sload(na_scen, na_char)) exit(1);
	if (*na_batch)
	{
		f= fopen(na_batch, "r");
		if (f== NULL)
		{
/*			/// failed to open batch file */
			sprintf(ixpress, "opening script file %s", na_batch);
			printf(" >> Error %s: file open was unsuccessful\n", ixpress);
			exit(1);
			}
		cexec_stream(f);
		exit(0);
	}

	if (*na_imname)
	{
		tfocus.setup.image.icode= 1;
		strcpy(tfocus.setup.image.imageform, na_imname);
	}
	if (af_start && tfocus.set.count)
	{
/*    /// render a single or sequence */
		if (!af_end) { if (cproc_render((int) af_start)) exit(1); }
			else { for (i= af_start; (i <= af_end); i+= af_step) { if (cproc_render(i)) exit(1); } }
		exit(0);
	}

	cexec_stream(stdin);
	exit(0);
}

