#pragma once
#include "pch.h"
#include "http-service-async.hpp"
#include "endpoints.hpp"
#include "sockets.hpp"
#include "binding.hpp"

class ServiceFakeAsync : public AsyncService
{
public:
	explicit ServiceFakeAsync(
		std::shared_ptr<asio::ip::tcp::socket> socket)
		:socket_(std::move(socket)) {}

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

template<typename ServiceType>
class AsyncAcceptor
{
public:
	AsyncAcceptor(asio::io_context& ioc, asio::ip::tcp::endpoint& endPoint)
		:ioc_(ioc),
		acceptor_(asio::ip::tcp::acceptor(ioc, endPoint))
	{
	}

	void Start()
	{
		acceptor_.listen(backlogSize_);
		Init();
	}

	void Stop()
	{
		stopFlag_ = true;
	}

private:

	void Init()
	{
		auto socket = std::make_shared<asio::ip::tcp::socket>(ioc_);
		acceptor_.async_accept(*socket, [this, socket]
		(const boost::system::error_code& error)
			{
				Accept(error, socket);
			});
	}

	void Accept(
		const boost::system::error_code& ec,
		std::shared_ptr<asio::ip::tcp::socket> socket)
	{
		if (ec.value() == 0)
		{
			(new ServiceType(socket))->HandleRequest();
		}

		if (!stopFlag_)[[likely]]
		{
			Init();
		}
		else
		{
			acceptor_.close();
		}
	}

private:
	asio::io_context& ioc_;
	uint8_t backlogSize_{ 10 };
	std::atomic<bool> stopFlag_{};
	asio::ip::tcp::acceptor acceptor_;
};

template<typename ServiceType>
class AsyncServer
{
public:
	AsyncServer()
	{
		work_ = std::move(std::make_unique<asio::io_context::work>(ioc_));
	}

	~AsyncServer()
	{
		Stop();
	}

	void Start(uint16_t port,
		uint16_t threadPoolSize=
		static_cast<uint16_t>(std::thread::hardware_concurrency() * 2))
	{
		threadPoolSize = threadPoolSize ? threadPoolSize : 2;
		auto endpoint = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
		acceptor_ = std::move(
			std::make_unique<AsyncAcceptor<ServiceType>>(ioc_, endpoint));
		acceptor_->Start();

		for (auto i{ 0 }; i < threadPoolSize; ++i)
		{
			threadPool_.emplace_back(std::make_unique<std::thread>([this]()
				{
					ioc_.run();
				}));
		}
	}

	void Stop()
	{
		acceptor_->Stop();
		ioc_.stop();

		for (auto& thread : threadPool_)
		{
			if (thread->joinable())
			{
				thread->join();
			}
		}
	}

private:
	asio::io_context ioc_;
	std::unique_ptr<asio::io_context::work> work_{};
	std::vector<std::unique_ptr<std::thread>> threadPool_{};
	std::unique_ptr<AsyncAcceptor<ServiceType>> acceptor_{};
};
