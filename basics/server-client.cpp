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

TEST(server_client, test_read_write)
{
	auto ip = "127.0.0.1"s;
	unsigned short port = 19871;
	auto backlogSize = 10;
	asio::io_context ioc;

	std::promise<asio::ip::tcp::socket> promise{};
	auto t = std::jthread(GetSingleClientSocket, std::ref(ioc), backlogSize, port, std::ref(promise));
	auto future = promise.get_future();

	std::shared_ptr<asio::ip::tcp::socket> clientSocket{};
	while (future.wait_for(1ms) == std::future_status::timeout)
	{
		try
		{
			if(!clientSocket)
				clientSocket = std::make_shared<asio::ip::tcp::socket>(GetConnectedSocket<asio::ip::tcp>(ioc, ip, port));

		}
		catch (const std::exception&)
		{
			continue;
		}
	}

	auto serverSocket = future.get();
	
	constexpr size_t size{1024};
	std::string msg(size, 'a');
	std::string msg2(size, 'a');
	EXPECT_NO_THROW(WriteToSocket(serverSocket, msg));
	EXPECT_NO_THROW(WriteToSocket(clientSocket, msg));
	auto res = ReadFromSocket<decltype(serverSocket), size>(serverSocket);
	EXPECT_EQ(msg, res);
	res = ReadFromSocket<decltype(clientSocket), size>(clientSocket);
	EXPECT_EQ(msg, res);
	EXPECT_EQ(1024, WriteToSocketInSingleCall(serverSocket, msg2));
	EXPECT_EQ(1024, WriteToSocketInSingleCall(clientSocket, msg2));
	res = ReadFromSocketInSingleCall<decltype(serverSocket), size>(serverSocket);
	EXPECT_EQ(msg2, res);
	res = ReadFromSocketInSingleCall<decltype(clientSocket), size>(clientSocket);
	EXPECT_EQ(msg2, res);
}
