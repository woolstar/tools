namespace btl
{

	template <class TCHAR> class	scanner : public buffer
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
			scanner(const void * aptr, size_t asz)
				: buffer((void *) aptr, asz),
					ptr_((const TCHAR *) aptr),
					limit_(ptr_ + asz)
				{ }
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

			const TCHAR *			operator*() const { return * ptr_ ; }

			const scanner<TCHAR>&	operator++() { ++ ptr_ ; check() ; return *this ; }
			const scanner<TCHAR>&	operator--() {
									if (ptr_ > rawbuffer_) ptr_ --; 
									return * this ;
								}
			const TCHAR *			operator++(int) {
									if (ptr_ == limit_) return ptr_ ;
										else return ( ptr_ ++) ;
								}
			const scanner<TCHAR>&	operator+=(unsigned int adelt)
								{ ptr_ += adelt ;  check() ;  return *this ; }
			const scanner<TCHAR>&	operator-=(unsigned int adelt)
								{ ptr_ -= adelt ;  checko() ;  return *this ; }

			void	reset(void) { ptr_= (const TCHAR *) rawbuffer_ ; }
			void	seek(int aoff) {
						if ( aoff < 0 ) { ptr_= limit_ + aoff ; checko() ; }
							else { ptr_= ((const TCHAR *) rawbuffer_) +aoff; check(); }
					}
			void	setend(void) { ptr_= limit_ ; }

			bool	hasdata(void) const { return ptr_ != limit_ ; }
			bool	isdone(void) const { return ptr_ == limit_ ; }

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

	typedef	scanner<unsigned char>	byte_scanner ;

} ;

