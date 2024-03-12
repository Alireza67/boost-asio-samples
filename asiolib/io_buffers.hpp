#pragma once
#include "pch.h"
#include <boost/asio.hpp>
#include <type_traits>
#include "sockets.hpp"

using namespace boost;

inline auto GetBuffer(std::string& inputBuffer)
{
	return asio::buffer(inputBuffer);
}

inline auto GetBuffer(const std::string& inputBuffer)
{
	return asio::buffer(inputBuffer);
}

inline auto GetBuffer(void* inputBuffer, const size_t size)
{
	return asio::buffer(inputBuffer, size);
}

inline auto GetBuffer(const void* inputBuffer, const size_t size)
{
	return asio::buffer(inputBuffer, size);
}

template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename Socket, typename Buffer>
typename std::enable_if<!is_shared_ptr<Socket>::value>::type
WriteToSocket(Socket& socket, Buffer& buffer)
{
	std::size_t totalByteWritten{};
	try
	{
		while (totalByteWritten != buffer.size())
		{
			totalByteWritten += socket.write_some(asio::buffer(buffer.data() + totalByteWritten,
				buffer.size() - totalByteWritten));
		}
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to write to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<typename Socket, typename Buffer>
typename std::enable_if<is_shared_ptr<Socket>::value>::type
WriteToSocket(Socket& socket, Buffer& buffer)
{
	std::size_t totalByteWritten{};
	try
	{
		while (totalByteWritten != buffer.size())
		{
			totalByteWritten += socket->write_some(asio::buffer(buffer.data() + totalByteWritten,
				buffer.size() - totalByteWritten));
		}
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to write to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}