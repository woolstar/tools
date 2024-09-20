
#include <string_view>

struct Find
{
  static int  longest( std::string_view ) ;
  static int  longest_tune( std::string_view ) ;

  static int  longest_c( const char * ) ;
  static int  longest_c_clever( const char * ) ;
} ;
