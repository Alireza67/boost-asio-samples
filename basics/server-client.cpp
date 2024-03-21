#pragma once
#include "pch.h"
#include "asiolib/servers.hpp"
#include "asiolib/clients.hpp"
#include "asiolib/io_buffers.hpp"

TEST(server_client, runServer_runClient)
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 19871;
	auto backlogSize = 10;

	std::promise<bool> promise{};
	auto t = std::jthread(RunServer, backlogSize, port, std::ref(promise));
	auto future = promise.get_future();
	future.get();
	auto t2 = std::jthread(RunClient, std::ref(ip), port);
	t2.join();
	SUCCEED();
}

inline void Callback2(
	const boost::system::error_code& ec,
	std::size_t bytes_transferred)
{
	if (ec.value() != 0)
	{
		if (ec == asio::error::operation_aborted)
		{
			EXPECT_EQ("995: The I/O operation has been aborted because of \
either a thread exit or an application request"s, 
				std::to_string(ec.value()) + ": "s + ec.message());
			return;
		}

		std::stringstream msg;
		msg << "Error occured! Error code = "
			<< ec.value()
			<< ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}
	EXPECT_EQ(1024, bytes_transferred);
}


inline void Callback(
	const boost::system::error_code& ec,
	std::size_t bytes_transferred,
	std::shared_ptr<asio::ip::tcp::socket> socket)
{
	if (ec.value() != 0)
	{
		std::stringstream msg;
		msg << "Error occured! Error code = "
			<< ec.value()
			<< ". Message: " << ec.message();
		throw std::runtime_error(msg.str());
	}

	boost::asio::streambuf readBuffer;
	boost::asio::streambuf::mutable_buffers_type bufs = readBuffer.prepare(1024);
	asio::async_read(
		*socket,
		bufs,
		std::bind(Callback2,
			std::placeholders::_1,
			std::placeholders::_2));
}

class ClientServerScenario : public ::testing::Test
{
protected:
	void SetUp();
	void TearDown() override {};

public:
	asio::io_context ioc;
	std::shared_ptr<asio::ip::tcp::socket> clientSocket{};
	std::shared_ptr<asio::ip::tcp::socket> serverSocket{};
};

void ClientServerScenario::SetUp()
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 19871;
	auto backlogSize = 10;

	std::promise<asio::ip::tcp::socket> promise{};
	auto t = std::jthread(GetSingleClientSocket, 
		std::ref(ioc), backlogSize, port, std::ref(promise));
	auto future = promise.get_future();

	while (future.wait_for(1ms) == std::future_status::timeout)
	{
		try
		{
			if (!clientSocket)
				clientSocket = std::make_shared<asio::ip::tcp::socket>(
					GetConnectedSocket<asio::ip::tcp>(ioc, ip, port));
		}
		catch (const std::exception&)
		{
			continue;
		}
	}

	auto tmp = future.get();
	serverSocket = std::make_shared<asio::ip::tcp::socket>(std::move(tmp));
}

TEST_F(ClientServerScenario, write_read_some)
{
	constexpr size_t size{1024};
	std::string msg(size, 'a');

	EXPECT_NO_THROW(WriteToSocket(serverSocket, msg));
	EXPECT_NO_THROW(WriteToSocket(clientSocket, msg));

	auto res = ReadFromSocket<decltype(serverSocket), size>(serverSocket);
	EXPECT_EQ(msg, res);
	res = ReadFromSocket<decltype(clientSocket), size>(clientSocket);
	EXPECT_EQ(msg, res);
}

TEST_F(ClientServerScenario, asio_read_write)
{
	constexpr size_t size{ 1024 };
	std::string msg(size, 'b');

	EXPECT_EQ(1024, WriteToSocketInSingleCall(serverSocket, msg));
	EXPECT_EQ(1024, WriteToSocketInSingleCall(clientSocket, msg));

	auto res = ReadFromSocketInSingleCall<decltype(serverSocket), size>(serverSocket);
	EXPECT_EQ(msg, res);
	res = ReadFromSocketInSingleCall<decltype(clientSocket), size>(clientSocket);
	EXPECT_EQ(msg, res);
}

TEST_F(ClientServerScenario, asio_read_until)
{
	constexpr size_t size{ 1024 };
	const std::string msg{ "abcdef@ghijk" };
	std::string msg2(size, 'c');
	std::string target{ "abcdef" };

	EXPECT_EQ(msg.size(), WriteToSocketInSingleCall(serverSocket, msg));
	EXPECT_EQ(msg.size(), WriteToSocketInSingleCall(clientSocket, msg));

	auto res = ReadFromSocketByDelimiter<decltype(serverSocket), '@'>(serverSocket);
	EXPECT_EQ(target, res);
	res = ReadFromSocketByDelimiter<decltype(clientSocket), '@'>(clientSocket);
	EXPECT_EQ(target, res);

	WriteAsync(serverSocket, clientSocket, msg2, Callback);
	ioc.run();
	ioc.restart();
	WriteAsync(serverSocket, serverSocket, msg2, Callback);
	auto tt = std::jthread([&]() {
		ioc.run();
		});
	std::this_thread::sleep_for(1s);
	serverSocket->cancel();
}

TEST_F(ClientServerScenario, read_write_async)
{
	constexpr size_t size{ 1024 };
	std::string msg(size, 'c');

	WriteAsync(serverSocket, clientSocket, msg, Callback);
	ioc.run();
}

TEST_F(ClientServerScenario, cancel_async)
{
	constexpr size_t size{ 1024 };
	std::string msg(size, 'c');

	WriteAsync(serverSocket, clientSocket, msg, Callback);
	ioc.run();
	ioc.restart();
	WriteAsync(serverSocket, serverSocket, msg, Callback);
	auto tt = std::jthread([&]() {
		ioc.run();
		});
	std::this_thread::sleep_for(1s);
	serverSocket->cancel();
}

TEST_F(ClientServerScenario, shutdown_send)
{
	std::string request{ "Hello Bob!" };
	std::string replay{ "Hi dear Alice!" };

	auto clientFuture = std::async(
		std::launch::async, 
		SendRequestAndGetReplay<std::shared_ptr<asio::ip::tcp::socket>, std::string>, 
		std::ref(clientSocket), std::ref(request));

	auto serverFuture = std::async(
		std::launch::async, 
		GetRequestAndSendReplay<std::shared_ptr<asio::ip::tcp::socket>, std::string>,
		std::ref(serverSocket), std::ref(replay));

	auto serverReplay = clientFuture.get();
	auto clientRequest = serverFuture.get();

	EXPECT_EQ(replay, serverReplay);
	EXPECT_EQ(request, clientRequest);
}
