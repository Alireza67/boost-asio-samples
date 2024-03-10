#include "pch.h"
#include "asiolib/endpoints.hpp"
#include <iostream>
#include <typeinfo>

TEST(endpoint, creating_client_endpoint) 
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoint<asio::ip::tcp>(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::tcp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_client_endpoint_ipv6) 
{
	auto ip = "fe36::404:c3fa:ef1e:3829"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoint<asio::ip::tcp>(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::tcp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_client_endpoint_bad_address)
{
	auto ip = "127.0.0.300"s;
	unsigned short port = 6969;
	EXPECT_THROW(CreateEndpoint<asio::ip::tcp>(ip, port), std::runtime_error);
}

TEST(endpoint, creating_serverside_endpoint)
{
	auto ip = "0.0.0.0"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
}

TEST(endpoint, creating_udp_client_endpoint)
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoint<asio::ip::udp>(ip, port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
	EXPECT_EQ(typeid(asio::ip::udp).name(), typeid(ep.protocol()).name());
}

TEST(endpoint, creating_udp_serverside_endpoint)
{
	auto ip = "::"s;
	unsigned short port = 6969;
	auto ep = CreateEndpoint<asio::ip::udp, asio::ip::address_v6>(port);

	EXPECT_EQ(port, ep.port());
	EXPECT_EQ(ip, ep.address().to_string());
}
