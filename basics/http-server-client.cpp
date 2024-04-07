#pragma once
#include "pch.h"
#include "asiolib/tcp-server-async.hpp"
#include "asiolib/http-client-async.hpp"

std::promise<bool> promiseTest2{};

void Testhandler2(
	const HTTPRequest& request,
	const HTTPResponse& response,
	const system::error_code& ec)
{
	if (ec.value() == 0)
	{
		promiseTest2.set_value(true);
	}
	else
	{
		FAIL() << "TEST is failed!";
		promiseTest2.set_exception(
			std::make_exception_ptr(std::runtime_error("TEST is failed!")));
	}
}

TEST(Http, simpleScenario)
{
	auto port{ 6969 };
	AsyncServer<HttpService> server;
	server.Start(port);

	HTTPClient client;
	auto request = client.create_request(1);
	request->set_host("localhost");
	request->set_uri("/index.html");
	request->set_port(6969);
	request->set_callback(Testhandler2);
	request->execute();


	auto future = promiseTest2.get_future();
	future.get();
}
