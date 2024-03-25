#pragma once
#include <stdexcept>
#include <boost/asio.hpp>

using namespace boost;
using namespace std::literals;

using Acceptor = asio::ip::tcp::acceptor;

inline Acceptor CreateAndOpenAcceptor(
	asio::io_service& ios, 
	asio::ip::tcp::endpoint endPoint)
{
	//implicitly binding
	Acceptor acceptor(ios, endPoint);
	return acceptor;
}

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
	using SenderSocket = typename Protocol::socket;
	try
	{
		SenderSocket socket(ios);
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
	using SenderSocket = typename Protocol::socket;
	try
	{
		SenderSocket socket(ios, protocol);
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
	using SenderSocket = typename Protocol::socket;

	Protocol protocol = Protocol::v4();
	SenderSocket socket(ios);

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
