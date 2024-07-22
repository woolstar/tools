// serz_buildtools.h

#ifndef SER_BUILDTOOLS_DEF
#define SER_BUILDTOOLS_DEF 1

#include <memory>


namespace Wool {
namespace Serialize {

struct	Block
{
  std::shared_ptr<char>  data ;
  size_t sz ;
} ;

template <typename BASEBUILD>
class BuildTool : public BASEBUILD
{
    public:
	template <typename INTEGRAL>
	void	add( INTEGRAL v ) { add( & v, sizeof(v) ) ; }

	void	add( const auto * ptr, size_t sz ) { BASEBUILD::add( (char *) ptr, sz ) ; }

	void	steal( const Block & block ) { add( block.data.get(), block.sz ) ; }
} ;

}
}

#endif
