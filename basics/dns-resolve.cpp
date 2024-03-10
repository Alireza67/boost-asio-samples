#pragma once
#include "pch.h"
#include "asiolib/dns-resolve.hpp"

TEST(dnsResolve, resolver)
{
	asio::io_service ios;
	auto host = "google.com"s;
	auto port = "80"s;

	auto eps = GetEndpoints<asio::ip::tcp>(ios, host, port);
	if (eps.size())
	{
		auto address = eps[0].address().to_string();
		auto count = std::count(address.begin(), address.end(), '.');
		EXPECT_EQ(3, count);
		EXPECT_EQ(port, std::to_string(eps[0].port()));
	}
}

TEST(dnsResolve, resolver_failed)
{
	asio::io_service ios;
	auto host = "fake_address_that_never_be_resolved.com"s;
	auto port = "80"s;

	EXPECT_THROW(GetEndpoints<asio::ip::tcp>(ios, host, port), std::runtime_error);
}

TEST(dnsResolve, udp_resolver)
{
	asio::io_service ios;
	auto host = "google.com"s;
	auto port = "80"s;

	auto eps = GetEndpoints<asio::ip::udp>(ios, host, port);
	if (eps.size())
	{
		auto address = eps[0].address().to_string();
		auto count = std::count(address.begin(), address.end(), '.');
		EXPECT_EQ(3, count);
		EXPECT_EQ(port, std::to_string(eps[0].port()));
	}
}

TEST(dnsResolve, udp_resolver_failed)
{
	asio::io_service ios;
	auto host = "fake_address_that_never_be_resolved.com"s;
	auto port = "80"s;

	EXPECT_THROW(GetEndpoints<asio::ip::udp>(ios, host, port), std::runtime_error);
}
