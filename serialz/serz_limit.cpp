#include <serz_limit.h>

namespace Wool {
namespace Serialize {

LimitNone::LimitNone() {}
LimitNone::LimitNone( size_t ) {}

LimitNone:: ~ LimitNone() {}

bool LimitTrack::operator()( size_t sz )
{
    if ((sz + current_) <= limit_) { current_ += sz ;  return true ; }
    fail( sz ) ;
    return false ;
}

}
}

