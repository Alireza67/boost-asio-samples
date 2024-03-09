#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;
using Acceptor = asio::ip::tcp::acceptor;

template<typename Protocol, typename IpVersion>
typename Protocol::endpoint CreateEndpoint(unsigned short port)
{
	using Endpoint = typename Protocol::endpoint;
	boost::system::error_code ec;
	asio::ip::address ip_address = IpVersion::any();
	return Endpoint(ip_address, port);
}

template<typename Protocol>
typename Protocol::endpoint CreateEndpoint(std::string& ip, unsigned short port)
{
	using Endpoint = typename Protocol::endpoint;
	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::address::from_string(ip, ec);
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return Endpoint(ip_address, port);
}

Acceptor CreateAndOpenAcceptor(asio::io_service& ios, asio::ip::tcp protocol)
{
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

template<typename Protocol>
typename Protocol::socket CreateAndOpenSocket(asio::io_service& ios, Protocol protocol)
{
	using Socket = typename Protocol::socket;
	try
	{
		Socket socket(ios, protocol);
		return socket;
	}
	catch (boost::system::system_error& e) {
		std::stringstream msg;
		msg << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		throw std::runtime_error(msg.str());
	}
}

template<typename Acceptor, typename Endpoint>
void Bind(Acceptor& acceptor, Endpoint& endpoint)
{
	boost::system::error_code ec;
	acceptor.bind(endpoint, ec);
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to bind the acceptor socket."
			<< "Error code = " << ec.value() << ". Message: "
			<< ec.message();
		throw std::runtime_error(msg.str());
	}
}

template<typename Socket, typename Endpoint>
void Connect(Socket& socket, Endpoint& endpoint)
{
	try
	{
		socket.connect(endpoint);
	}
	catch (boost::system::system_error& e)
	{
		std::stringstream msg;
		msg << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		throw std::runtime_error(msg.str());
	}
}

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
