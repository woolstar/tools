// serz_buildsz.h

#ifndef SERZ_BUILDSZ_DEF
#define SERZ_BUILDSZ_DEF  1

#include <stddef.h>

namespace Wool {
namespace Serialize {

class   BuildSz
{
    public:
      void	add( const char *, size_t sz ) { size_ += sz ; }

      size_t	size() const { return size_ ; }

    protected:
      size_t	size_ = 0 ;
} ;

}
}

#endif

