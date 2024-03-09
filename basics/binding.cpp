#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;
using Acceptor = asio::ip::tcp::acceptor;

template<typename Protocol, typename IpVersion>
typename Protocol::endpoint CreateEndpoint(unsigned short port)
{
	using Endpoint = typename Protocol::endpoint;
	boost::system::error_code ec;
	asio::ip::address ip_address = IpVersion::any();
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

TEST(binding, bind)
{
	asio::io_service ios;
	unsigned short port = 6969;
	auto endpoint = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
	auto acceptor = CreateAndOpenAcceptor(ios, endpoint.protocol());
	Bind(acceptor, endpoint);
	SUCCEED();
}
