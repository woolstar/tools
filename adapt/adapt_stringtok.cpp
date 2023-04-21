// adapt_stringtok.cpp				-*-C++-*-

#include	"adapt_stringtok.h"

#include        <ctype.h>
#include	<assert.h>
#include        <limits.h>

namespace Wool {
namespace Adapt {

using namespace std ;

void
StringTokenize::load( string_view str )
{
    assert( str.size() <= USHRT_MAX ) ;

    _string= str.data() ;
    _lasti = str.size() ;

    _curi= _nexti= 0 ;

    if (( _active= _lasti )) { step() ; }
}

void
StringTokenize::step()
{
    if ( ! _active || ! _string ) { return ; }

    _curi= _nexti ;
    if ( _nexti < _lasti )
    {
      auto q= _string + _nexti ;
        // skip leading whitespace
      while (( _nexti != _lasti ) && isblank( * q )) { _nexti ++, q ++ ; }
      _curi= _nexti ;

      if ( _nexti < _lasti )
      {
          // parse out a word (pcre: /(/w+)/) or a single non-space character.
        if ( isalpha( * q ) )
            { while (( _nexti != _lasti ) && isalnum( * q ) ) { _nexti ++, q ++ ; } }
          else { _nexti ++ ; }

        return ;
      }
    }

    _active= false ;
}

}
}

