#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;

using TcpActiveSocket = asio::ip::tcp::socket;
using UdpActiveSocket = asio::ip::udp::socket;
using Acceptor = asio::ip::tcp::acceptor;

TcpActiveSocket CreateAndOpenActiveSocket(asio::io_service& ios)
{
	asio::ip::tcp protocol = asio::ip::tcp::v4();
	TcpActiveSocket socket(ios);
	boost::system::error_code ec;

	socket.open(protocol, ec);

	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to open the socket! Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return std::move(socket);
}

TEST(sockets, create_and_open_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocket(ios));
}

TcpActiveSocket CreateAndOpenActiveSocketInOneAction(asio::io_service& ios)
{
	try
	{
		asio::ip::tcp protocol = asio::ip::tcp::v4();
		TcpActiveSocket socket(ios, protocol);
		return socket;
	}
	catch (boost::system::system_error& e) {
		std::stringstream msg;
		msg << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		throw std::runtime_error(msg.str());
	}
}

TEST(sockets, create_and_open_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenActiveSocketInOneAction(ios));
}

UdpActiveSocket CreateAndOpenUdpActiveSocket(asio::io_service& ios)
{
	asio::ip::udp protocol = asio::ip::udp::v4();
	UdpActiveSocket socket(ios);
	boost::system::error_code ec;

	socket.open(protocol, ec);

	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to open the socket! Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return std::move(socket);
}

UdpActiveSocket CreateAndOpenUdpActiveSocketInOneAction(asio::io_service& ios)
{
	try
	{
		asio::ip::udp protocol = asio::ip::udp::v4();
		UdpActiveSocket socket(ios, protocol);
		return socket;
	}
	catch (boost::system::system_error& e) {
		std::stringstream msg;
		msg << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		throw std::runtime_error(msg.str());
	}
}

TEST(sockets, create_and_open_udp_active_socket)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenUdpActiveSocket(ios));
}

TEST(sockets, create_and_open_udp_active_socket_one_method)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenUdpActiveSocketInOneAction(ios));
}

Acceptor CreateAndOpenAcceptor(asio::io_service& ios)
{
	asio::ip::tcp protocol = asio::ip::tcp::v6();
	Acceptor acceptor(ios);

	boost::system::error_code ec;
	acceptor.open(protocol, ec);
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to open the acceptor socket!"
			<< "Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}
	return acceptor;
}

TEST(sockets, acceptor)
{
	asio::io_service ios;
	EXPECT_NO_THROW(CreateAndOpenAcceptor(ios));
}
