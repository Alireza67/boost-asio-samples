#pragma once
#include "pch.h"
#include "endpoints.hpp"
#include "sockets.hpp"
#include "binding.hpp"

class AsyncService
{
public:
	virtual void HandleRequest() = 0;
};

class ServiceFakeAsync : public AsyncService
{
public:
	explicit ServiceFakeAsync(
		std::string msg,
		std::shared_ptr<asio::ip::tcp::socket> socket)
		:task_(std::move(msg)),
		socket_(std::move(socket)) {}

	void HandleRequest() override
	{
		asio::async_read_until(*socket_, buffer_, '\n',
			[this](const boost::system::error_code& ec,
				std::size_t bytesTransferred) 
			{
				RecieveRequest(ec, bytesTransferred);
			});
	}

private:
	std::string task_{};
	std::string response{};
	asio::streambuf buffer_;
	std::shared_ptr<asio::ip::tcp::socket> socket_{};

	void Clean()
	{
		delete this;
	}

	std::string ProcessRequest()
	{
		//Simulate time consuming calculateion
		auto counter{ 0 };
		while (counter < 1'000'000)
		{
			++counter;
		}

		//simulate I/O operation
		std::this_thread::sleep_for(1s);

		std::string output;
		std::istream input(&buffer_);
		std::getline(input, output);

		return "request is processed!: "s + output + '\n';
	}

	void RecieveRequest(
		const boost::system::error_code& ec,
		std::size_t bytesTransferred)
	{
		if (ec.value())
		{
			Clean();
			return;
		}

		response = ProcessRequest();

		asio::async_write(*socket_, asio::buffer(response),
			[this](const boost::system::error_code& ec,
				std::size_t bytesTransferred)
			{
				SendAnswer(ec, bytesTransferred);
			});
	}

	void SendAnswer(
		const boost::system::error_code& ec,
		std::size_t bytesTransferred)
	{
		Clean();
	}
};
