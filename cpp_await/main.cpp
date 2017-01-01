#include <winsock2.h>
#include <windows.h>
#include <boost\asio.hpp>
#include <iostream>
#include <experimental\resumable>

#include "asyncer.h"
#include <boost\thread\future.hpp>

#pragma comment(lib, "Ws2_32.lib")
using namespace boost;

asio::io_service g_ios;

auto test1() {
	return boost::async(launch::async, []() -> int {
		std::cout << boost::this_thread::get_id() << ": in async\n";
		return 190;
	});
}

boost::unique_future<void> test2() {
	std::cout << boost::this_thread::get_id() << ": sleeping¡¦\n";
	auto t = test1();

	await t.then(launch::async, [](boost::unique_future<int> f)
	{
		std::cout << f.get() << "\n";
		std::cout << boost::this_thread::get_id() << ": then\n";
	});

	std::cout << boost::this_thread::get_id() << ": woke up\n";
}

boost::unique_future<void> test3() {
	std::cout << boost::this_thread::get_id() << ": sleeping¡¦\n";
	await test2();
	std::cout << boost::this_thread::get_id() << ": woke up\n";
}

int main() {
	/*boost::thread th[4];
	for (auto& t : th) {
		t = boost::thread([]() {
			while (true)
			{
				g_ios.run();
			}
		});
	}*/
	
	test3().get();

	std::cout << this_thread::get_id() << ": back in main\n";
	std::system("pause");

	/*for (auto& t : th) {
		t.join();
	}*/
}