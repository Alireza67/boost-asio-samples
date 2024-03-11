#pragma once
#include "pch.h"
#include "binding.hpp"
#include "sockets.hpp"
#include "endpoints.hpp"

inline void RunClient(const std::string& ip, const unsigned short portNumber)
{
	asio::io_context ioc;
	auto endPoint = CreateEndpoint<asio::ip::tcp>(ip, portNumber);
	try
	{
		auto socket = CreateSocket<asio::ip::tcp>(ioc);
		socket.connect(endPoint);
	}
	catch (std::exception& e)
	{
		std::cout << std::string(e.what()) << '\n';
	}
}
