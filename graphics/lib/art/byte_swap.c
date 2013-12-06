
void	x8(int sz, char *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 8, ba= ptr; (istep --); ba += 3)
	{
		bb= ba +7, v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, *bb= v;
	}
}
void	x4(int sz, char *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 4, ba= ptr; (istep --); ba += 3)
	{
		bb= ba +3, v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, *bb= v;
	}
}
void x2(int sz, char *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 2, ba= ptr; (istep --); ba += 2)
		{ bb= ba +1, v= *ba, *ba= *bb, *bb= v; }
}

void	x8e(int n, char *ptr, int r)
{
	int istep;
	char *ba, *bb, *bx, v;
	for (istep= n, ba= bx= ptr; (istep --); ba= bx)
	{
		bx= ba +r;
		bb= ba +7, v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, *bb= v;
	}
}
void	x4e(int n, char *ptr, int r)
{
	int istep;
	char *ba, *bb, *bx, v;
	for (istep= n, ba= bx= ptr; (istep --); ba= bx)
	{
		bx= ba +r;
		bb= ba +3, v= *ba, *ba= *bb, ba ++, *bb= v, bb --;
		v= *ba, *ba= *bb, *bb= v;
	}
}
void	x2e(int n, char *ptr, int r)
{
	int istep;
	char *ba, *bb, *bx, v;
	for (istep= n, ba= bx= ptr; (istep --); ba= bx)
		{ bx= ba +r; bb= ba +1, v= *ba, *ba= *bb, *bb= v; }
}

