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
			clientSocket = std::make_shared<asio::ip::tcp::socket>(GetConnectedSocket<asio::ip::tcp>(ioc, ip, port));

		}
		catch (const std::exception&)
		{
			continue;
		}
	}

	auto serverSocket = future.get();
	
	std::string msg(1024, 'a');
	EXPECT_NO_THROW(WriteToSocket(serverSocket, msg));
	EXPECT_NO_THROW(WriteToSocket(clientSocket, msg));
}
