#pragma once
#include <stdexcept>
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

template<typename Acceptor, typename Endpoint>
inline void Bind(Acceptor& acceptor, Endpoint& endpoint)
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
inline void Connect(Socket& socket, Endpoint& endpoint)
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
