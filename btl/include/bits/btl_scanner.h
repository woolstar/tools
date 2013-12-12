// scanner -*- C++ -*-

#ifndef _BTL_SCANNER_H
#define _BTL_SCANNER_H	1

namespace btl
{

	template <class TCHAR = char> class	scanner : public buffer
	{
		public:
			scanner(const buffer & arec)
				: buffer(arec),
					ptr_((const TCHAR *) * arec),
					limit_(ptr_ + arec.size())
				{ }
			scanner(const buffer & arec, unsigned int aoffset)
				: buffer(arec),
					ptr_(((const TCHAR *) * arec) +aoffset),
					limit_(ptr_ + arec.size())
				{ }
//			scanner(const void * aptr, size_t asz)
//				: buffer((void *) aptr, asz),
//					ptr_((const TCHAR *) aptr),
//					limit_(ptr_ + asz)
//				{ }
			scanner(const scanner<TCHAR> & arec)
				: buffer(arec),
					ptr_(arec.ptr_),
					limit_(arec.limit_)
				{ }

			const buffer	sub(size_t asz) const {
							ASSERT(asz <= (limit_ - ptr_)) ;
							return buffer (ptr_, asz) ;
						}
			const buffer	remaining(void) const
						{ return buffer(ptr_, (size_t) (limit_ - ptr_)) ; }

			const TCHAR *	operator*() const { return ptr_ ; }

			scanner<TCHAR>&	operator++() { ++ ptr_ ; check() ; return *this ; }
			scanner<TCHAR>&	operator--() {
							if (ptr_ > rawbuffer_) ptr_ --; 
							return * this ;
						}
			const TCHAR *	operator++(int) {
							if (ptr_ == limit_) return ptr_ ;
								else return ( ptr_ ++) ;
						}
			scanner<TCHAR>&	operator+=(unsigned int adelt)
					{ ptr_ += adelt ;  check() ;  return *this ; }
			scanner<TCHAR>&	operator-=(unsigned int adelt)
					{ ptr_ -= adelt ;  checko() ;  return *this ; }

			scanner<TCHAR>&	set_begin(void) { ptr_= (const TCHAR *) rawbuffer_ ;  return *this ; }
			scanner<TCHAR>&	set_end(void) { ptr_= limit_ ;  return * this ; }
			scanner<TCHAR>&	jump(int aoff) {
					if ( aoff < 0 ) { ptr_= limit_ + aoff ; checko() ; }
						else { ptr_= ((const TCHAR *) rawbuffer_) +aoff; check(); }
					return * this ;
				}
			scanner<TCHAR>&	seek(int aoff) {
					ptr_ += aoff ;  if ( aoff < 0 ) { checko() ; } else { check() ; }
					return * this ;
				}

		private:
			class	IsValid { void operator delete(void *) ; } ;

		public:

				// test active/done
			bool	hasdata(void) const { return ptr_ != limit_ ; }
			bool	active(void) const { return hasdata() ; }
			operator const IsValid *() const { static IsValid _sentinal ;  return hasdata() ? & _sentinal : nullptr ; }
			// bool	operator()(void) const { return hasdata() ; }

			bool	done(void) const { return ptr_ == limit_ ; }

			size_t	offset(void) const { return ptr_ - (const TCHAR *) rawbuffer_ ; }
			size_t	size(void) const { return limit_ - ptr_ ; }

			int	find(TCHAR achar) const {
					const TCHAR * m_sear ;
					for (m_sear= ptr_; (m_sear < limit_); m_sear ++)
						{ if (achar == * m_sear) { return m_sear - ptr_ ; } }
					return -1 ;
				}
			int	find(TCHAR achar, int aoff) const {
					const TCHAR * m_sear ;
					for (m_sear= ptr_ +aoff; (m_sear < limit_); m_sear ++)
						{ if (achar == * m_sear) { return m_sear - ptr_ ; } }
					return -1 ;
				}

		protected:
			void	check(void) { if (ptr_ > limit_) ptr_= limit_ ; }
			void	checko(void) { if ( (void *) ptr_ < rawbuffer_ ) { ptr_= (const TCHAR *) rawbuffer_ ; } }

		private:
			scanner(const buffer & arec, const TCHAR * aptr)
				: buffer(arec),
					ptr_(aptr),
					limit_(aptr + arec.size())
				{ }

		protected:
			const TCHAR *	ptr_ ;
			const TCHAR *	limit_ ;

#ifdef	TEST
		public:
			static void	test(void) ;
#endif
	} ;

} ;

#endif

