#pragma once
#include "pch.h"
#include "asiolib/tcp-server-sync.hpp"
#include "asiolib/tcp-client-sync.hpp"

TEST(TCP_SYNC, tcp_serve_client_async)
{
	auto port = 6969;
	auto ip = "127.0.0.1"s;
	ServiceFake fake("Task is done: "s);
	Server server(fake, port);
	server.Start();

	{
		SyncTCPClient client(ip, port);
		client.Connect();

		EXPECT_EQ("Task is done: EMULATE_LONG_COMP_OP 10"s,
			client.EmulateLongComputationOp(10));
	}

	std::this_thread::sleep_for(100ms);
	auto f1 = std::async(std::launch::async, [&]() {
		server.Stop();
		});

	{
		SyncTCPClient client(ip, port);
		client.Connect();

		EXPECT_EQ("Task is done: EMULATE_LONG_COMP_OP 20"s,
			client.EmulateLongComputationOp(20));
	}
	f1.get();
}
