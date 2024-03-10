#pragma once
#include "pch.h"
#include "asiolib/binding.hpp"
#include "asiolib/sockets.hpp"
#include "asiolib/endpoints.hpp"
#include "asiolib/dns-resolve.hpp"

TEST(binding, bind)
{
	asio::io_service ios;
	unsigned short port = 6969;
	auto endpoint = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
	auto acceptor = CreateAndOpenAcceptor(ios, endpoint.protocol());
	Bind(acceptor, endpoint);
	SUCCEED();
}

TEST(binding, double_binding)
{
	asio::io_service ios;
	unsigned short port = 7979;
	auto endpoint = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
	auto acceptor = CreateAndOpenAcceptor(ios, endpoint.protocol());
	Bind(acceptor, endpoint);
	auto acceptor2 = CreateAndOpenAcceptor(ios, endpoint.protocol());
	EXPECT_THROW(Bind(acceptor2, endpoint), std::runtime_error);
}

TEST(binding, udp_bind)
{
	asio::io_service ios;
	unsigned short port = 8989;
	auto endpoint = CreateEndpoint<asio::ip::udp, asio::ip::address_v4>(port);
	auto socket = CreateAndOpenSocket(ios, endpoint.protocol());
	Bind(socket, endpoint);
	SUCCEED();
}

TEST(binding, udp_double_bind)
{
	asio::io_service ios;
	unsigned short port = 25250;
	auto endpoint = CreateEndpoint<asio::ip::udp, asio::ip::address_v4>(port);
	auto socket = CreateAndOpenSocket(ios, endpoint.protocol());
	Bind(socket, endpoint);
	auto socket2 = CreateAndOpenSocket(ios, endpoint.protocol());
	EXPECT_THROW(Bind(socket2, endpoint), std::runtime_error);
}

TEST(connecting, connect_without_server)
{
	asio::io_service ios;
	unsigned short port = 54321;
	auto ip = "127.0.0.1"s;
	auto endpoint = CreateEndpoint<asio::ip::tcp>(ip, port);
	auto socket = CreateAndOpenSocket(ios, endpoint.protocol());
	EXPECT_THROW(Connect(socket, endpoint), std::runtime_error);
}

TEST(connecting, connect_by_dns_resolve)
{
	asio::io_service ios;
	auto host = "google.com"s;
	auto port = "80"s;
	auto eps = GetEndpoints<asio::ip::tcp>(ios, host, port);

	for (auto & endpoint: eps)
	{
		auto socket = CreateAndOpenSocket(ios, endpoint.protocol());
		EXPECT_NO_THROW(Connect(socket, endpoint), std::runtime_error);
		break;
	}
}

TEST(connecting, connect_by_iterator_dns_resolve)
{
	asio::io_service ios;
	auto host = "google.com"s;
	auto port = "80"s;
	auto itr = GetEndpointsItr<asio::ip::tcp>(ios, host, port);
	auto socket = CreateAndOpenSocket(ios, asio::ip::tcp::v4());
	EXPECT_NO_THROW(asio::connect(socket, itr));
}
