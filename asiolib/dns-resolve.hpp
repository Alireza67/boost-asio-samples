#pragma once
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

template<typename Protocol>
inline std::vector<typename Protocol::endpoint> GetEndpoints(asio::io_service& ios, const std::string& host, const std::string& port)
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
