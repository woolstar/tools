
#include <string>
#include <vector>

struct Gen
{
    static unsigned int	  random() ;
    static std::string    label() ;
    static std::vector<std::string>   group( int n= 10 ) ;
    static std::vector<std::string>   searches( const std::vector<std::string> & src, int n ) ;

    static std::string	  block() ;
} ;

