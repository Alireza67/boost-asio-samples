#pragma once
#include <stdexcept>
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

using Acceptor = asio::ip::tcp::acceptor;

inline Acceptor CreateAndOpenAcceptor(asio::io_service& ios, asio::ip::tcp protocol)
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
inline typename Protocol::socket CreateSocket(asio::io_service& ios)
{
	using Socket = typename Protocol::socket;
	try
	{
		Socket socket(ios);
		return socket;
	}
	catch (boost::system::system_error& e) {
		std::stringstream msg;
		msg << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		throw std::runtime_error(msg.str());
	}
}

template<typename Protocol>
inline typename Protocol::socket CreateAndOpenSocket(asio::io_service& ios, Protocol protocol)
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

template<typename Protocol>
inline typename Protocol::socket CreateAndOpenSocket(asio::io_service& ios)
{
	using Socket = typename Protocol::socket;

	Protocol protocol = Protocol::v4();
	Socket socket(ios);

	boost::system::error_code ec;
	socket.open(protocol, ec);

	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Failed to open the socket! Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	return std::move(socket);
}
