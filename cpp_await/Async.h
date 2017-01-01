#pragma once

#include "lib.h"

#include <future>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <utility>

#include "ThreadPool.h"

namespace Acoross
{
	template <typename TResult>
	boost::unique_future<TResult> Async(std::function<TResult()> func)
	{
		auto prom = std::make_shared<boost::promise<TResult>>();
		auto fut = prom->get_future();

		ThreadPool::Post([func, prom]() {
			std::cout << boost::this_thread::get_id() << ": posted\n";
			prom->set_value(func());
		});
		
		return fut;
	}

	inline boost::unique_future<void> Async(std::function<void()> func)
	{
		auto prom = std::make_shared<boost::promise<void>>();
		auto fut = prom->get_future();

		ThreadPool::Post([func, prom]() {
			std::cout << boost::this_thread::get_id() << ": posted\n";
			func();
			prom->set_value();
		});

		return fut;
	}

	template <typename T>
	inline boost::unique_future<T> Async(std::function<boost::unique_future<T>()> func)
	{
		auto prom = std::make_shared<boost::promise<void>>();
		auto fut = prom->get_future();

		ThreadPool::Post([func, prom]() {
			std::cout << boost::this_thread::get_id() << ": posted\n";
			prom->set_value(await func());
		});

		return fut;
	}
}