#pragma once

#include "lib.h"
#include <functional>
#include <memory>
#include <list>
#include <cassert>
#include <mutex>
#include "ThreadPool.h"

namespace Acoross
{
	/*
	TODO: 
		mutex 추가
		wait, get 의 경우 spinlock 을 통해 값이 설정될 때 까지 대기
		내부 상태들을 thread safe 하게 수정: atomic 으로? -> std, boost 참고하여 만들기
	*/
	class FutureState
	{
	public:
		void set_value(int val)
		{
			if (ready)
			{
				throw std::exception("value is already set");
			}

			ready = true;

			this->val = val;
			if (next)
			{
				next();
			}
		}

		int get_value()
		{
			if (value_retrieved)
			{
				throw std::exception("value is already retrieved");
			}

			if (!ready)
			{
				// TODO: spinlock
			}

			value_retrieved = true;
			return val;
		}

		void then(std::function<void()> next)
		{
			if (ready)
			{
				next();
			}
			else
			{
				this->next = next;
			}
		}

		bool future_retrieved{ false };
		bool ready{ false };
		bool value_retrieved{ false };

	private:
		int val{ 0 };
		std::function<void()> next;
	};

	// 임시로 int 형으로.
	class Future
	{
	public:
		int get()
		{
			assert(state != nullptr);
			if (state == nullptr)
			{
				throw std::exception("future state is null!!");
			}

			return state->get_value();
		}

		Future Then(std::function<int(Future)> func)
		{
			auto then = std::make_shared<Promise>();
			auto fut = then->get_future();

			assert(state != nullptr);
			if (state == nullptr)
			{
				throw std::exception("future state is null!!");
			}

			state->then([then, func, this]() {
				then->set_value(func(
					Future(std::move(state))
				));
			});

			return fut;
		}

	private:
		explicit Future(std::shared_ptr<FutureState> state)
			: state(state)
		{
			state->future_retrieved = true;
		}

		std::shared_ptr<FutureState> state;
		friend class Promise;
	};

	// 임시로 int 형으로.
	class Promise
	{
	public:
		Promise()
			: state(std::make_shared<FutureState>())
		{}
		Promise& operator=(Promise&& rhs)
		{
			state = std::move(rhs.state);
		}

		Promise(Promise&) = delete;
		Promise& operator=(Promise&) = delete;

		Future get_future()
		{
			assert(state->future_retrieved == false);
			if (state->future_retrieved)
			{
				throw std::exception("Future is already retrieved");
			}
			return Future(state);
		}

		void set_value(int val)
		{
			state->set_value(val);
		}

		std::shared_ptr<FutureState> state;
	};

	inline Future _Async(std::function<int()> func)
	{
		auto prom = std::make_shared<Promise>();
		auto fut = prom->get_future();

		ThreadPool::Post([func, prom]() {
			std::cout << boost::this_thread::get_id() << ": posted\n";
			prom->set_value(func());
		});

		return fut;
	}
}