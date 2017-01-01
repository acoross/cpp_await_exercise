#pragma once

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <windows.h>

#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <experimental\resumable>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\thread\future.hpp>
#include <future>
