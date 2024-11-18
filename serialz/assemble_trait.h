
#include <string>
#include <vector>

using namespace std ;

class   AssembleStr
{
    public:
        AssembleStr( string & dest ) : dest_( dest ) {}

        void add( char c ) 
        {
            dest_ += c ;
        }

        string & dest_ ;    
} ;

class   AssembleVec
{
    public:
        AssembleVec( vector<char> & dest ) : dest_( dest ) {}

        void add( char c ) 
        {
            dest_.push_back( c ) ;
        }

        vector<char> & dest_ ;
} ;

template <typename T>
struct AssembleEngine
{ } ;

template <>
struct AssembleEngine<string>
{
    using Engine = AssembleStr ;
} ;
template <>
struct AssembleEngine< vector<char>>
{
    using Engine = AssembleVec ;
} ;

template <typename ASSEMBLE>
class Assemble
{
    public:
        Assemble( ASSEMBLE & x ) : assemble_( x) {}

        void add( char c ) { assemble_.add( c) ; }

        AssembleEngine<ASSEMBLE>::Engine assemble_ ;
} ;

void test()
{
    string a ;
    vector<char> b ;

    Assemble axs( a ) ;
    axs.add( 's ' ) ;

    Assemble axv( b ) ;
    axv.add( 'v' ) ;

}
