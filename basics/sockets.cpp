#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;

using TcpActiveSocket = asio::ip::tcp::socket;

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
		asio::ip::tcp::socket socket(ios, protocol);
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
