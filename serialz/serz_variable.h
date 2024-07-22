// serz_variable.h

#ifndef SERZ_VARIABLE_DEF
#define SERZ_VARIABLE_DEF 1

#include <stddef.h>
#include <string_view>
#include <type_traits>


namespace Wool {
namespace Serialize {

template <typename HEADER = uint32_t >
class	Variable
{
    public:
	Variable( const char * ptr, size_t sz ) : data_( ptr ), sz_( sz ) { }

        Variable( std::string_view str ) : Variable( str.data(), str.size() ) {}

	template <typename BLOCK>
	void	serialize( BLOCK & block ) const {
                  block.add( sz_ ) ;
                  block.add( data_, sz_ ) ;
		}

    protected:
        const char * data_ ;
        HEADER sz_ ;
} ;

template <typename HEADER = uint32_t >
class   VariableStruct : public Variable<HEADER>
{
    public:
      template <typename STRUCT>
      VariableStruct( const STRUCT & data ) : Variable<HEADER>( (char *) & data, sizeof( STRUCT )) { }
} ;

}
}

#endif
