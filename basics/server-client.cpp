#pragma once
#include "pch.h"
#include "asiolib/binding.hpp"
#include "asiolib/servers.hpp"
#include "asiolib/clients.hpp"

TEST(server_client, runServer)
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
