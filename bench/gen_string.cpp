#include "gen_string.h"

#include <random>
#include <algorithm>

namespace
{

const static int  kBlockSize = 2048 ;

}

using namespace std ;

unsigned int
Gen::random()
{
    std::random_device r ;
    return r() ;
}

string
Gen::label()
{
    static mt19937  rgen( random() ) ;
    string result( "      " ) ;
    
    uniform_int_distribution<int> u_digit(0, 9), u_letter('A', 'Z' );

    result[0]= u_letter( rgen ) ;
    result[1]= u_letter( rgen ) ;
    result[2]= u_digit( rgen ) + '0' ;
    result[3]= u_digit( rgen ) + '0' ;
    result[4]= u_digit( rgen ) + '0' ;
    result[5]= u_digit( rgen ) + '0' ;
    return result ;
}

vector<string>
Gen::group( int n )
{
    vector<string>  result ;

    result.reserve( n ) ;
    for ( int i= n ; ( i-- ) ; )
    {
      result.emplace_back( label() ) ;
    }
    return result ;
}

string
Gen::block()
{
    static minstd_rand mgen( random() ) ;
    static uniform_int_distribution<int> u_byte('!', '~' ) ;

    return string( kBlockSize, u_byte( mgen ) ) ;
}

vector<string>
Gen::searches( const vector<string> & src, int sz )
{
    static minstd_rand mgen( random() ) ;
    vector<string>  result ;

    result.reserve( sz ) ;
    int mult= sz / src.size() ;
    if ( sz % src.size()) { mult ++ ; }

    for ( int i= mult ; ( i-- ) ; )
      { result.insert( result.end(), src.begin(), src.end() ) ; }

    shuffle( result.begin(), result.end(), mgen ) ;

    return result ;
}

