#pragma once
#include "pch.h"
#include "binding.hpp"
#include "sockets.hpp"
#include "endpoints.hpp"

inline void RunServer(const int backlogSize, const unsigned short portNumber, std::promise<bool>& promise)
{
	asio::io_context ioc;
	auto endPoint = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(portNumber);
	try
	{
		auto acceptor = CreateAndOpenAcceptor(ioc, endPoint.protocol());
		Bind(acceptor, endPoint);
		acceptor.listen(backlogSize);
		auto socket = CreateSocket<asio::ip::tcp>(ioc);
		promise.set_value(true);
		acceptor.accept(socket);
	}
	catch(std::exception& e)
	{
		promise.set_exception(std::make_exception_ptr(std::runtime_error(std::string(e.what()))));
	}
}
