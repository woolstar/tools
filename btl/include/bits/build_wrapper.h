/* */

#ifndef _BUILD_WRAP_H
#define _BUILD_WRAP_H 1

namespace btl
{
	class build_if
	{
		public:
			template <typename T>
				build_if() : worker_( new adapter_t<T>()) { }

				// standard build calls
			build_if &	add(const buffer & abuf) { worker_->add_( abuf) ;  return * this ; }
			build_if &	add(scanner<> & abuf) { worker_->add_( abuf) ;  return * this ; }
			build_if &	add(scanner<> & abuf, size_t sz) { worker_->add_( abuf, sz) ;  return * this ; }

			build_if &	operator<<(const buffer & abuf) { return add( abuf) ; }
			build_if &	operator<<(scanner<> & abuf) { return add( abuf) ; }

				// base calls
			size_t	remaining(void) const { return worker_->remaining_() ; }
			void	reset(void) { worker_->reset_() ; }

		private:
			build_if() { }

			struct concept_if
			{
				virtual ~ concept_if() { }

				virtual void add_(const buffer &) = 0 ;
				virtual void add_(scanner<> &) = 0 ;
				virtual void add_(scanner<> &, size_t) = 0 ;

				virtual size_t remaining_(void) const = 0 ;
				virtual void reset_(void) = 0 ;
			} ;

			template <typename T>
				struct adapter_t : public concept_if
				{
					void add_(const buffer & abuf) { buf_.add( abuf) ; }
					void add_(scanner<> abuf) { buf_.add( abuf) ; }
					void add_(scanner<> abuf, size_t asz) { buf_.add( abuf, asz) ; }

					size_t	remaining_(void) const { return buf_.remaining() ; }
					void	reset_(void) { buf_.reset() ; }

					T buf_ ;
				} ;

			std::unique_ptr<concept_if>	worker_ ;
	} ;

	//
} ;

#endif

