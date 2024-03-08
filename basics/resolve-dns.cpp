#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

template<typename Protocol>
std::vector<typename Protocol::endpoint> GetEndpoints(asio::io_service& ios, const std::string& host, const std::string& port)
{
	using Resolver = asio::ip::basic_resolver<Protocol>;
	using iterator = typename Resolver::iterator;
	using Endpoint = typename Protocol::endpoint;
	using Query = typename Resolver::query;

	Query resolver_query(host, port, Resolver::query::numeric_service);
	Resolver resolver(ios);

	boost::system::error_code ec;
	iterator it = resolver.resolve(resolver_query, ec);

	if (ec.value() != 0) {
		std::stringstream msg;
		msg << "Failed to resolve a DNS name."
			<< "Error code = " << ec.value()
			<< ". Message = " << ec.message();
		throw std::runtime_error(msg.str());
	}

	iterator end{};
	std::vector<Endpoint> output;

	for (; it != end; it++) {
		output.emplace_back(it->endpoint());
	}
	return output;
}

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
