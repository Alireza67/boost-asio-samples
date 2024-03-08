#include "pch.h"

#include <iostream>
#include <typeinfo>
#include <stdexcept>

#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

using TcpEndPoint = asio::ip::tcp::endpoint;
using UdpEndPoint = asio::ip::udp::endpoint;

TcpEndPoint CreateEndpoing(std::string& ip, unsigned short port)
{
	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::address::from_string(ip, ec);
	if (ec.value() != 0) 
	{
		std::stringstream msg;
		msg << "Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return TcpEndPoint(ip_address, port);
}

TcpEndPoint CreateEndpoing(unsigned short port)
{
	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::address_v4::any();
	return TcpEndPoint(ip_address, port);
}

TEST(endpoint, creating_client_endpoint) 
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoing(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::tcp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_client_endpoint_ipv6) 
{
	auto ip = "fe36::404:c3fa:ef1e:3829"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoing(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::tcp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_client_endpoint_bad_address)
{
	auto ip = "127.0.0.300"s;
	unsigned short port = 6969;
	EXPECT_THROW(CreateEndpoing(ip, port), std::runtime_error);
}

TEST(endpoint, creating_serverside_endpoint)
{
	auto ip = "0.0.0.0"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoing(port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
}

UdpEndPoint CreateEndpoingUdp(std::string& ip, unsigned short port)
{
	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::address::from_string(ip, ec);
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return UdpEndPoint(ip_address, port);
}

UdpEndPoint CreateEndpoingUdp(unsigned short port)
{
	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::address_v6::any();
	return UdpEndPoint(ip_address, port);
}

TEST(endpoint, creating_udp_client_endpoint)
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoingUdp(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::udp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_udp_serverside_endpoint)
{
	auto ip = "::"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoingUdp(port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
}
