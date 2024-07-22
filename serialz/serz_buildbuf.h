// serz_buildsz.h

#ifndef SERZ_BUILDSZ_DEF
#define SERZ_BUILDSZ_DEF  1

#include <stddef.h>
#include <string.h>
#include <serz_limit.h>

namespace Wool {
namespace Serialize {

template <class LIMIT>
class   BuildBuf
{
    public:
      BuildBuf( char * buf, size_t size )
              : buffer_( buf ), curr_( buf ), far_( buf + size ),
                limit_( size )
              { }

      void    add( const char *, size_t sz ) ;

      const char *  operator*() const { return buffer_ ; }
      size_t        size() const { return curr_ - buffer_ ; }

    protected:
      char * buffer_, * curr_, * far_ ;

      LIMIT  limit_ ;
} ;

template <class LIMIT>
void  BuildBuf<LIMIT>::add( const char * data, size_t sz )
{
    if ( ! limit_( sz ) ) {
      return ;
    }
    char * temp = curr_ + sz ;
    if ( temp > far_ ) {
      return ;
    }

    memcpy( curr_, data, sz ) ;
    curr_ = temp ;
}

}
}

#endif

