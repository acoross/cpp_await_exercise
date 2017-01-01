#include "lib.h"

#include <iostream>
#include <experimental\resumable>
#include <boost\thread\future.hpp>
#include <future>

#include "Async.h"
#include "Await.h"

using namespace boost;

auto test1() {
	return Acoross::Async<int>(
	//return std::async(
		[]() -> int {
		std::cout << boost::this_thread::get_id() << ": in async\n";
		return 190;
	});
}

std::future<void> test2() {
	std::cout << boost::this_thread::get_id() << ": sleeping¡¦\n";
	
	auto t = test1();
	int ret = await t;

	std::cout << boost::this_thread::get_id() << ": woke up\n";
}

int main() {
	auto t = test2();
	t.get();

	std::cout << this_thread::get_id() << ": back in main\n";
	std::system("pause");
}