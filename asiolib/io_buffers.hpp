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

template<typename SenderSocket, typename Buffer>
typename std::enable_if<!is_shared_ptr<SenderSocket>::value>::type
WriteToSocket(SenderSocket& socket, Buffer& buffer)
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

template<typename SenderSocket, typename Buffer>
typename std::enable_if<is_shared_ptr<SenderSocket>::value>::type
WriteToSocket(SenderSocket& socket, Buffer& buffer)
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
	typename SenderSocket, 
	typename Buffer, 
	typename std::enable_if<!is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::size_t WriteToSocketInSingleCall(SenderSocket& socket, Buffer& buffer)
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
	typename SenderSocket, 
	typename Buffer, 
	typename std::enable_if<is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::size_t WriteToSocketInSingleCall(SenderSocket& socket, Buffer& buffer)
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
	typename SenderSocket,
	std::size_t BufferSize,
	typename std::enable_if<!is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::string ReadFromSocket(SenderSocket& socket)
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
	typename SenderSocket,
	std::size_t BufferSize,
	typename std::enable_if<is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::string ReadFromSocket(SenderSocket& socket)
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
	typename SenderSocket,
	std::size_t BufferSize,
	typename std::enable_if<!is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::string ReadFromSocketInSingleCall(SenderSocket& socket)
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
	typename SenderSocket,
	std::size_t BufferSize,
	typename std::enable_if<is_shared_ptr<SenderSocket>::value, void>::type* = nullptr>
std::string ReadFromSocketInSingleCall(SenderSocket& socket)
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
	typename SenderSocket,
	char Delimiter,
	typename = std::enable_if_t<is_shared_ptr<SenderSocket>::value ||
	!is_shared_ptr<SenderSocket>::value>>
std::string ReadFromSocketByDelimiter(SenderSocket& socket)
{
	asio::streambuf buffer;
	try
	{
		if constexpr (!is_shared_ptr<SenderSocket>::value)
		{
			asio::read_until(socket, buffer, Delimiter);
		}
		else if constexpr (is_shared_ptr<SenderSocket>::value)
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

struct Session {
	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::string buf;
	std::size_t total_bytes_written;
};

inline void WriteAsync(
	std::shared_ptr<asio::ip::tcp::socket> senderSocket,
	std::shared_ptr<asio::ip::tcp::socket> receiverSocket,
	std::string& buffer,
	std::function<void(const boost::system::error_code&,
		std::size_t,
		std::shared_ptr<asio::ip::tcp::socket>)> Callback)
{
	asio::async_write(
		*senderSocket,
		asio::buffer(buffer),
		std::bind(Callback,
			std::placeholders::_1,
			std::placeholders::_2,
			receiverSocket));
}

template<
	typename Socket,
	typename Buffer,
	typename = std::enable_if<is_shared_ptr<Socket>::value>>
	std::string SendRequestAndGetReplay(Socket& sock, Buffer& buffer)
{
	asio::write(*sock, asio::buffer(buffer));
	sock->shutdown(asio::socket_base::shutdown_send);

	system::error_code ec;
	asio::streambuf response;
	asio::read(*sock, response, ec);
	if (ec == asio::error::eof)
	{
		std::istream input(&response);
		std::string output;
		std::getline(input, output);
		return output;
	}
	throw system::system_error(ec);
}

template<
	typename Socket,
	typename Buffer,
	typename = std::enable_if<is_shared_ptr<Socket>::value>>
	std::string GetRequestAndSendReplay(Socket& sock, Buffer& buffer)
{
	system::error_code ec;
	asio::streambuf request;

	asio::read(*sock, request, ec);
	if (ec != asio::error::eof)
	{
		throw system::system_error(ec);
	}

	asio::write(*sock, asio::buffer(buffer));
	sock->shutdown(asio::socket_base::shutdown_send);

	std::istream input(&request);
	std::string output;
	std::getline(input, output);
	return output;
}
