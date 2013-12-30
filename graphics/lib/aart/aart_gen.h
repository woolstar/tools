#ifndef	AARTGEN_DEF
#	define	AARTGEN_DEF	1

#include "aart_base.h"

class	aart_gen
{
	public:
		aart_gen() ;
		static aart_component *		create(const char *) ;

		static void		init(void) ;
		static void		close(void) ;

	protected:
		virtual ~ aart_gen() ;

	protected:
		virtual bool	test(const char *) = 0 ;
		virtual aart_component *	generate(const char *) = 0 ;

		bool	matchword(const char *, const char *) ;

	private:
		aart_gen	* next ;

		static aart_gen * list ;
} ;

template<class T>
class aart_tgen : public aart_gen
{
	protected:
		aart_component * generate(const char * astr) { return new T(astr); }
} ;

#define	TEST_NAME(aaa) \
	bool	test(const char * astring) { return matchword(aaa, astring); }

#endif	// AARTGEN_DEF