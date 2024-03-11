#pragma once
#include <stdexcept>
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

template<typename Protocol>
inline typename Protocol::endpoint CreateEndpoint(const std::string& ip, const unsigned short port)
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

template<typename Protocol, typename IpVersion>
inline typename Protocol::endpoint CreateEndpoint(unsigned short port)
{
	using Endpoint = typename Protocol::endpoint;
	boost::system::error_code ec;
	asio::ip::address ip_address = IpVersion::any();
	return Endpoint(ip_address, port);
}
