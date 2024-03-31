#pragma once
#include "pch.h"
#include "asiolib/tcp-server-async.hpp"
#include "asiolib/tcp-client-async.hpp"

std::promise<bool> promiseTest{};

void Testhandler(
	unsigned int request_id,
	const std::string& response,
	const system::error_code& ec)
{
	if (ec.value() == 0)
	{
		EXPECT_EQ("request is processed!: EMULATE_LONG_CALC_OP 12"s,
			response);
		promiseTest.set_value(true);
	}
	else
	{
		FAIL()<< "TEST is failed!";
		promiseTest.set_exception(
			std::make_exception_ptr(std::runtime_error("TEST is failed!")));
	}
}

TEST(TCP_ASYNC, tcp_client_server_async)
{
	auto port{ 6969 };
	AsyncServer server;
	server.Start(port);

	AsyncTCPClient client;
	client.EmulateLongComputationOp(12, "127.0.0.1", port, Testhandler, 1);
	auto future = promiseTest.get_future();
	future.get();
}