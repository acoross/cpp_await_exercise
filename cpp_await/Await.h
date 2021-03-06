#pragma once

#include "lib.h"
#include <experimental\resumable>
#include "ThreadPool.h"

namespace std {
	namespace experimental {
		
		using namespace boost;

		// 이건 return 값을 future 로 만들어 주는 기능이다.
		template<class _Ty, class... _ArgTypes>
		struct coroutine_traits<unique_future<_Ty>, _ArgTypes...>
		{	// defines resumable traits for functions returning future<_Ty>
			struct promise_type
			{
				boost::promise<_Ty> _MyPromise;

				unique_future<_Ty> get_return_object()
				{
					return (_MyPromise.get_future());
				}

				bool initial_suspend() const
				{
					return (false);
				}

				bool final_suspend() const
				{
					return (false);
				}

				template<class _Ut>
				void return_value(_Ut&& _Value)
				{
					_MyPromise.set_value(_STD forward<_Ut>(_Value));
				}

				void set_exception(exception_ptr _Exc)
				{
					_MyPromise.set_exception(_STD move(_Exc));
				}
			};
		};

		template<class... _ArgTypes>
		struct coroutine_traits<unique_future<void>, _ArgTypes...>
		{	// defines resumable traits for functions returning future<void>
			struct promise_type
			{
				boost::promise<void> _MyPromise;

				unique_future<void> get_return_object()
				{
					return (_MyPromise.get_future());
				}

				bool initial_suspend() const
				{
					return (false);
				}

				bool final_suspend() const
				{
					return (false);
				}

				void return_void()
				{
					_MyPromise.set_value();
				}

				void set_exception(exception_ptr _Exc)
				{
					_MyPromise.set_exception(_STD move(_Exc));
				}
			};
		};
	}
}

auto operator await(boost::unique_future<void>& fut)
{
	class awaiter
	{
	public:
		explicit awaiter(boost::unique_future<void>& fut) : fut(fut) { }
		bool await_ready() const {
			return fut.is_ready();
		}
		void await_suspend(std::experimental::coroutine_handle<> resume_cb) {
			fut.then(boost::launch::sync, [this, resume_cb](boost::unique_future<void>& f) {
				std::cout << boost::this_thread::get_id() << ": await_suspend then\n";
				resume_cb();
			});
		}
		auto await_resume()
		{
			return;
		}
		boost::unique_future<void>& fut;
	};

	return awaiter(fut);
}

template <typename T>
auto operator await(boost::unique_future<T>& fut)
{
	class awaiter
	{
	public:
		explicit awaiter(boost::unique_future<T>& fut) : fut(fut) { }
		bool await_ready() const {
			return fut.is_ready();
		}
		void await_suspend(std::experimental::coroutine_handle<> resume_cb) {
			fut.then(boost::launch::sync, [this, resume_cb](boost::unique_future<T>& f) {
				std::cout << boost::this_thread::get_id() << ": await_suspend then\n";
				ret = f.get();
				resume_cb();
			});
		}
		auto await_resume()
		{
			return ret;
		}
		boost::unique_future<T>& fut;
		T ret;
	};

	return awaiter(fut);
}