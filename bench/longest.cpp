#include "longest.h"

#include <unordered_map>
#include <algorithm>
#include <stddef.h>

int Find::longest( std::string_view in )
{
    std::unordered_map<char, int> indexes ;

    int start = 0 ;
    int best = 0 ;

    for ( size_t i = 0, e = in.size() ; ( i < e ) ; i ++ ) {
      auto index= indexes.find( in[ i ] ) ;
      if ( ( indexes.end() != index ) && ( index-> second >= start ) ) {
        int len= i - start ;
        if ( len > best ) { best = len ; }
        start= index-> second +1 ;
      }

      indexes[ in[ i ] ]= i ;
    }

    int len= in.size() - start ;
    if ( len > best ) { len= best ; }

    return best ;
}

int Find::longest_tune( std::string_view in )
{
    std::unordered_map<char, int> indexes ;

    int start = 0 ;
    size_t best = 0 ;
    char c ;

    for ( size_t i = 0, e = in.size() ; ( i < e ) ; i ++ ) {
      c= in[ i ] ;
      if ( int ipos= indexes[ c ] >= start ) {
        best= std::max( best, i - start ) ;
        start= ipos +1 ;
      }

      indexes[ c ]= i ;
    }

    int len= in.size() - start ;
    if ( len > best ) { len= best ; }

    return best ;
}

int Find::longest_c( const char * str )
{
    long longest= 0 ;
    const char * p= str, * last[256] = { 0 }, ** lptr ;

    while ( * p ) {
      lptr= last + * p ;
      if ( * lptr > str ) { str= * lptr + 1 ; }
      longest = std::max( p-str, longest ) ;
      * lptr= p ++ ;
    }

    return longest ;
}

int Find::longest_c_clever( const char * str )
{
    long longest= 0 ;
    const char * p= str, * last[256] = { 0 }, ** lptr ;

    while ( * p ) {
      lptr= last + * p ;
      if ( * lptr > str ) {
        longest = std::max( p-str, longest ) ;
        str= * lptr + 1 ;
      }
      * lptr= p ++ ;
    }

    return std::max( p - str, longest ) ;
}


