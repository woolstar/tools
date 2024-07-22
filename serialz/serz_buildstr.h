// serz_buildstr.h

#ifndef SERZ_BUILDSTR_DEF
#define SERZ_BUILDSTR_DEF  1

#include <stddef.h>
#include <string>

namespace Wool {
namespace Serialize {

class   BuildString
{
    public:
      BuildString() = default ;
      BuildString( size_t sz ) { reserve( sz ) ; }

      void    add( const char *, size_t ) ;

      void          reserve( size_t sz ) { buffer_.reserve( sz ) ; }
      const char *  operator*() const { return buffer_.data() ; }
      size_t        size() const { return buffer_.length() ; }

    protected:
      std::string   buffer_ ;
} ;

}
}

#endif

