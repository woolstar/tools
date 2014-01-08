#include <stdio.h>
#include <ctype.h>
#include <buffer>
#include <algorithm>
#include <random>

	// using namespace btl::literals ;

	using btl::buffer ;

int main(int N, char **S )
{
	int ipos ;

	buffer	bufferstring("simple static thing") ;
	btl::build_static<1024> bhold ;
	btl::scanner<btl::sized_storage> scan( bufferstring) ;
	btl::build_managed	bgrow(256) ;
	btl::build_if buildte { btl::build_static<256, btl::expand_strict>() } ;

	bgrow.reset() << "Initial space usage ---- " ;	// what I wanted was bgrow = "..." ;

	printf("BUFFER: %s\n", * bufferstring) ;
	printf("SCAN: <%s>\n", * scan) ;
	++ scan ;
	printf("SCAN +1: <%s>\n", * scan) ;
	scan.seek( 1 + scan.find(' ')) ;
	printf("SCAN-skipword: <%s>\n", * scan) ;

	bhold << "Hellow World + " << btl::format( "%d", 10) << "." ;
	bhold.terminate() ;

	printf("BUILD: %s\n", * bhold) ;
	printf("BUILD begin: %s\n", begin(bhold)) ;
	printf("BGROW: %s\n", *( bgrow.terminate()) ) ;

	buildte << " and now we've hidden away all the details. " ;
	buildte << "A little music, " << u8"\U0001D11E" << " mystro." ;
	printf("Bif (u8): %s\n", * buffer( buildte)) ;

		// it can be used as a standard container

	std::stable_partition(begin(bhold), end(bhold), [](unsigned char c){ return isupper(c) ; } ) ;
	bhold.terminate() ;
	printf("partition-buf: %s\n", * bhold) ;
	std::stable_partition(begin(bhold), end(bhold), [](unsigned char c){ return islower(c) ; } ) ;
	printf("partition2-buf: %s\n", * bhold) ;

	std::sort(begin(bhold), end(bhold)) ;
	printf("sort-buf: %s\n", * bhold) ;

	bhold << "; and it goes on." << "  Lets add a " << scan << ", here." ;
	printf("BUILD+scan: %s\n", * ( bhold.terminate()) ) ;

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle( begin(bhold), end(bhold), g ) ;
	std::stable_partition(begin(bhold), end(bhold), [](unsigned char c){ return ! isspace(c) ; } ) ;
	printf("shuffle-buf: %s\n", * bhold) ;

	for ( const auto & i : { 1, 2, 3, 4, 5, 6, 7, 8 } )
		{ bgrow << bhold ;  std::shuffle( begin(bhold), end(bhold), g ) ; }

	std::stable_partition(begin(bgrow), end(bgrow), [](unsigned char c){ return ! isspace(c) ; } ) ;
	printf("shuffle-grow*8: %s\n", * ( bgrow.terminate() )) ;

		// doing dynamic reallocation
  
	return 0 ;
}

