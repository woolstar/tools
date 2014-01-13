
	static int	_passed = 0 ;

template <typename T, typename U>
	static void	check(T va, U vb)
{
	if ( va != vb ) { 
		fprintf(stderr, "test failed: %d != %d\n", (int) va, (int) vb) ;
		exit( 1) ;
	}

	++ _passed ;
}

