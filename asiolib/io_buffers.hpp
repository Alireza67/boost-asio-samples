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

template<
	typename Socket, 
	typename Buffer, 
	typename std::enable_if<!is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::size_t WriteToSocketInSingleCall(Socket& socket, Buffer& buffer)
{
	try
	{
		return asio::write(socket, asio::buffer(buffer));
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to write to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<
	typename Socket, 
	typename Buffer, 
	typename std::enable_if<is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::size_t WriteToSocketInSingleCall(Socket& socket, Buffer& buffer)
{
	try
	{
		return asio::write(*socket, asio::buffer(buffer));
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to write to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}


template<
	typename Socket,
	std::size_t BufferSize,
	typename std::enable_if<!is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::string ReadFromSocket(Socket& socket)
{
	std::string buffer(BufferSize, ' ');
	std::size_t totalBytesRead{};

	try
	{
		while (totalBytesRead != BufferSize)
		{
			totalBytesRead += socket.read_some(asio::buffer(buffer.data() + totalBytesRead,
															BufferSize - totalBytesRead));
		}
		return buffer;
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to read to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<
	typename Socket,
	std::size_t BufferSize,
	typename std::enable_if<is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::string ReadFromSocket(Socket& socket)
{
	std::string buffer(BufferSize, ' ');
	std::size_t totalBytesRead{};

	try
	{
		while (totalBytesRead != BufferSize)
		{
			totalBytesRead += socket->read_some(asio::buffer(buffer.data() + totalBytesRead,
															 BufferSize - totalBytesRead));
		}
		return buffer;
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to read to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<
	typename Socket,
	std::size_t BufferSize,
	typename std::enable_if<!is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::string ReadFromSocketInSingleCall(Socket& socket)
{
	std::string buffer(BufferSize, ' ');
	try
	{
		asio::read(socket, asio::buffer(buffer.data(), BufferSize));
		return buffer;
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to read to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<
	typename Socket,
	std::size_t BufferSize,
	typename std::enable_if<is_shared_ptr<Socket>::value, void>::type* = nullptr>
std::string ReadFromSocketInSingleCall(Socket& socket)
{
	std::string buffer(BufferSize, ' ');
	try
	{
		asio::read(*socket, asio::buffer(buffer.data(), BufferSize));
		return buffer;
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to read to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}

template<
	typename Socket,
	char Delimiter,
	typename = std::enable_if_t<is_shared_ptr<Socket>::value ||
	!is_shared_ptr<Socket>::value>>
std::string ReadFromSocketByDelimiter(Socket& socket)
{
	asio::streambuf buffer;
	try
	{
		if constexpr (!is_shared_ptr<Socket>::value)
		{
			asio::read_until(socket, buffer, Delimiter);
		}
		else if constexpr (is_shared_ptr<Socket>::value)
		{
			asio::read_until(*socket, buffer, Delimiter);
		}

		std::string output;
		std::istream input(&buffer);
		std::getline(input, output, Delimiter);
		return output;
	}
	catch (const system::system_error& e)
	{
		std::stringstream msg;
		msg << "Failed to read to socket. Error code = "
			<< ". Message: " << std::string(e.what());
		throw std::runtime_error(msg.str());
	}
}
