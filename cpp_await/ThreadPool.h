#pragma once

#include "lib.h"
#include <atomic>
#include <vector>

namespace Acoross
{
	class ThreadPool
	{
	public:
		~ThreadPool()
		{
			is_run.store(false);
			for (auto& t : worker_threads)
			{
				if (t.joinable())
				{
					t.join();
				}
			}
		}

		static void Post(std::function<void()> func)
		{
			inst.init();
			inst.io_service.post(func);
		}

		static void SetThreadCount(int count)
		{
			inst.count = count;
		}

		template <typename T>
		static T AddService()
		{
			inst.init();
			return T(inst.io_service);
		}

	private:
		ThreadPool()
		{

		}

		void init()
		{
			bool exp = false;
			if (is_run.compare_exchange_strong(exp, true))
			{
				for (int i = 0; i < inst.count; ++i)
				{
					auto th = std::thread([this]()
					{
						std::cout << boost::this_thread::get_id() << ": thread pool init\n";

						while (is_run == true)
						{
							io_service.run();
						}
					});

					worker_threads.push_back(std::move(th));
				}
			}
		}

		static ThreadPool inst;
		std::atomic<bool> is_run{ false };
		boost::asio::io_service io_service;
		std::vector<std::thread> worker_threads{ 0 };
		int count{ 4 };
	};

	class ThreadContext
	{
	public:
		ThreadContext()
			: strand(ThreadPool::AddService<boost::asio::strand>())
		{}

		void Post(std::function<void()> func)
		{
			strand.post(func);
		}

	private:
		boost::asio::strand strand;
	};
}