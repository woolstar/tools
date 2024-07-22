#include <serz_buildstr.h>

namespace Wool {
namespace Serialize {

void	BuildString::add( const char * ptr, size_t sz )
{
    buffer_.append( ptr, sz ) ;
}

}
}

