#pragma once
#include "pch.h"
#include "asiolib/sockets.hpp"

TEST(sockets, create_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateSocket<asio::ip::tcp>(ios));
}

TEST(sockets, create_socket_udp)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateSocket<asio::ip::udp>(ios));
}

TEST(sockets, create_and_open_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenSocket<asio::ip::tcp>(ios));
}

TEST(sockets, create_and_open_udp_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenSocket<asio::ip::udp>(ios));
}

TEST(sockets, create_and_open_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenSocket(ios, asio::ip::tcp::v4()));
}

TEST(sockets, create_and_open_udp_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenSocket(ios, asio::ip::udp::v6()));
}

TEST(sockets, acceptor)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenAcceptor(ios, asio::ip::tcp::v4()));
}
