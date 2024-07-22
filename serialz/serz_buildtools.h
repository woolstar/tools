// serz_buildtools.h

#ifndef SERZ_BUILDTOOLS_DEF
#define SERZ_BUILDTOOLS_DEF 1

#include <memory>
#include <boost/endian/conversion.hpp>


namespace Wool {
namespace Serialize {

struct	Block
{
  std::shared_ptr<char>  data ;
  size_t sz ;
} ;

template <typename BASEBUILD>
class BuildTools : public BASEBUILD
{
    public:
      using BASEBUILD::BASEBUILD ;

      template <typename INTEGRAL>
      void	add( INTEGRAL v ) { add( (const char *) & v, sizeof(v) ) ; }

      void	add( const auto * ptr, size_t sz ) { BASEBUILD::add( (const char *) ptr, sz ) ; }

      void	steal( const Block & block ) { add( block.data.get(), block.sz ) ; }
} ;

template <typename BASEBUILD>
class BuildEndian : public BASEBUILD
{
    public:
      template <typename ...ARGS>
      BuildEndian(ARGS&& ...args) : BASEBUILD( std::forward<ARGS>( args )... ) {}

      template <typename INTEGRAL>
      void      add( INTEGRAL v ) {
                  boost::endian::native_to_big_inplace( v ) ;
                  BASEBUILD::add( (const char * ) & v, sizeof( v ) ) ;
                }
} ;

}
}

#endif
