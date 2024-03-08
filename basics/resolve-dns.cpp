#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

asio::ip::tcp::endpoint GetEndpoint(asio::io_service& ios, const std::string& host, const std::string& port)
{
	asio::ip::tcp::resolver::query resolver_query(host, port, asio::ip::tcp::resolver::query::numeric_service);
	asio::ip::tcp::resolver resolver(ios);

	boost::system::error_code ec;
	asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query, ec);
	
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to resolve a DNS name."
			<< "Error code = " << ec.value()
			<< ". Message = " << ec.message();
		throw std::runtime_error(msg.str());
	}
	return it->endpoint();
}

TEST(dnsResolve, resolver)
{
	asio::io_service ios;
	auto host = "google.com"s;
	auto port = "80"s;

	auto ep = GetEndpoint(ios, host, port);
	auto address = ep.address().to_string();
	auto count = std::count(address.begin(), address.end(), '.');
	EXPECT_EQ(3, count);
	EXPECT_EQ(port, std::to_string(ep.port()));
}

TEST(dnsResolve, resolver_failed)
{
	asio::io_service ios;
	auto host = "fake_address_that_never_be_resolved.com"s;
	auto port = "80"s;

	EXPECT_THROW(GetEndpoint(ios, host, port), std::runtime_error);
}
