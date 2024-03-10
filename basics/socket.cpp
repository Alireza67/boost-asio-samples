#pragma once
#include "pch.h"
#include "asiolib/sockets.hpp"

TEST(sockets, create_and_open_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocket<asio::ip::tcp>(ios));
}

TEST(sockets, create_and_open_udp_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocket<asio::ip::udp>(ios));
}

TEST(sockets, create_and_open_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocketInOneAction<asio::ip::tcp>(ios));
}

TEST(sockets, create_and_open_udp_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocketInOneAction<asio::ip::udp>(ios));
}

TEST(sockets, acceptor)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenAcceptor(ios, asio::ip::tcp::v4()));
}
