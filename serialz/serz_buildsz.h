// serz_buildsz.h

#ifndef SER_BUILDSZ_DEF
#define SER_BUILDSZ_DEF  1

#include <stddef.h>

namespace Wool {
namespace Serialize {

class   BuildSz
{
    public:
	void	add( const char *, size_t sz ) { size += sz ; }

	size_t	size = 0 ;
} ;

}
}

#endif
