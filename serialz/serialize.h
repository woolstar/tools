// serialize.h

#ifndef SERIALIZE_DEF
#define SERIALIZE_DEF  1

#include <stddef.h>
#include <type_traits>
#include <utility>


namespace Wool {
namespace Serialize {

template <class BUILD>
class Serialize
{
    public:
      Serialize() = default ;

      template <typename ...ARGS>
      Serialize(ARGS&& ...args) : build_( std::forward<ARGS>( args )... ) {}

      size_t size() const { return build_.size() ; }
      auto operator*() { return * build_ ; }

      template <typename T, std::enable_if_t< std::is_integral_v< T >, bool > = true >
      Serialize & operator<<( const T & val ) {
          build_.add( val ) ;
	  return * this ;
      }

      template <typename T,
                std::enable_if_t< std::is_member_function_pointer_v<
                     decltype(& T::template serialize<BUILD>) >, bool> = true >
      Serialize & operator<<( const T & val ) {
	  val.serialize( build_ ) ;
	  return * this ;
      }

    protected:
      BUILD   build_ ;
} ;

}
}

#endif
