
#include <string>
#include <vector>
#include <cstdint>

// Generate some test strings for benchmarking

struct Gen
{
    static unsigned int	  random() ;

    	// construct a dictionary key with two letters and four digits
    static std::string    label() ;
    	// construct a group of labels
    static std::vector<std::string>   group( int n= 10 ) ;
    	// expand a group of labels by duplication into a search set (with shuffling)
    static std::vector<std::string>   searches( const std::vector<std::string> & src, int n ) ;

    	// construct random bytes
    static std::vector<uint8_t>       bytes( int n=16 ) ;

    	// allocate some memory (2k) to chew up pages.
    static std::string	  block() ;
} ;

