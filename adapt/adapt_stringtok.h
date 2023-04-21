// adapt string tokenize			-*-C++-*-
#ifndef INCLUDED_ADAPT_STRINGTOK
#define INCLUDED_ADAPT_STRINGTOK 1

#include <string_view>

namespace Wool {
namespace Adapt {

// String Tokenize
//
// Machinery for iterating tokens (words and single character punctuation) from a string
// optimized for use within c++11 foreach
//
// usage:  for ( auto s : StringTokenize( src_string ) ) { ... }
//

class	StringTokenize
{
    public:
      StringTokenize() : _string( nullptr ), _active( 0 ) { }
      StringTokenize( std::string_view str )              { load( str ) ; }

    protected:
      const char    * _string ;
      unsigned short  _curi, _nexti, _lasti, _active ;

        // internal mechanism for advancing through the string
      void            step() ;

    public:
      explicit operator bool() const { return _active ; }
      unsigned  remaining() const { return _lasti - _nexti ; }

      StringTokenize &  operator=(std::string_view str) { load( str ) ;  return * this ; }
      void              load( std::string_view ) ;

      std::string_view  operator*() const
                        { return _active ? std::string_view( _string + _curi, ( _nexti - _curi ) ) : "" ; }

      StringTokenize    begin() const { return * this ; }
      StringTokenize    end()   const { return StringTokenize() ; }
      void              operator++()    { step() ; }
      void              operator++(int) { step() ; }

      friend bool   operator==(const StringTokenize & a, const StringTokenize & b )
                    {
                      return ( a._active == b._active ) &&
                             ( ! a._active || (( a._string == b._string ) && ( a._nexti == b._nexti ))) ;
                    }
      friend bool   operator!=(const StringTokenize & a, const StringTokenize & b ) { return ! ( a == b ) ; }
} ;

}
}

#endif
