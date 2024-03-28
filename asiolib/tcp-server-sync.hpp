#pragma once
#include "pch.h"
#include "endpoints.hpp"
#include "sockets.hpp"
#include "binding.hpp"

class Service
{
public:
	virtual void HandleRequest(asio::ip::tcp::socket& socket) = 0;
};

class ServiceFake : public Service
{
public:
	explicit ServiceFake(std::string msg)
		:task_(std::move(msg)){}

	void HandleRequest(asio::ip::tcp::socket& socket) override
	{
		asio::streambuf buffer;
		asio::read_until(socket, buffer, '\n');

		//Simulate time consuming calculateion
		auto counter{ 0 };
		while (counter < 1'000'000)
		{
			++counter;
		}

		//simulate I/O operation
		std::this_thread::sleep_for(1s);

		std::string output;
		std::istream input(&buffer);
		std::getline(input, output);
		asio::write(socket, asio::buffer(task_ + output + "\n"));
	}

	void DetachHandle(std::shared_ptr<asio::ip::tcp::socket> socket)
	{
		auto t = std::thread([this, socket]() {
			HandleRequest(socket);
			});

		t.detach();
	}

	void HandleRequest(std::shared_ptr<asio::ip::tcp::socket> socket)
	{
		asio::streambuf buffer;
		asio::read_until(*socket, buffer, '\n');

		//Simulate time consuming calculateion
		auto counter{ 0 };
		while (counter < 1'000'000)
		{
			++counter;
		}

		//simulate I/O operation
		std::this_thread::sleep_for(1s);

		std::string output;
		std::istream input(&buffer);
		std::getline(input, output);
		asio::write(*socket, asio::buffer(task_ + output + "\n"));

		delete this;
	}

private:
	std::string task_{};
};

class AcceptorCl
{
public:
	AcceptorCl(
		Service& service, asio::io_context& ioc, 
		asio::ip::tcp::endpoint& endPoint)
		:service_(service), ioc_(ioc)
	{
		acceptor_ = std::move(std::make_unique<asio::ip::tcp::acceptor>(
			CreateAndOpenAcceptor(ioc, endPoint)));

		acceptor_->listen(backlogSize_);
	}

	void Accept()
	{
		auto socket = CreateSocket<asio::ip::tcp>(ioc_);
		acceptor_->accept(socket);
		service_.HandleRequest(socket);
	}

private:
	Service& service_;
	asio::io_context& ioc_;
	uint8_t backlogSize_{10};
	std::unique_ptr<asio::ip::tcp::acceptor> acceptor_{};
};

class ParallelAcceptor
{
public:
	ParallelAcceptor(
		asio::io_context& ioc,
		asio::ip::tcp::endpoint& endPoint)
		:ioc_(ioc)
	{
		acceptor_ = std::move(std::make_unique<asio::ip::tcp::acceptor>(
			CreateAndOpenAcceptor(ioc, endPoint)));

		acceptor_->listen(backlogSize_);
	}

	void Accept()
	{
		auto socket = std::make_shared<asio::ip::tcp::socket>(
			CreateSocket<asio::ip::tcp>(ioc_));
		acceptor_->accept(*socket);
		(new ServiceFake("Task will be done: "))->DetachHandle(socket);
	}

private:
	asio::io_context& ioc_;
	uint8_t backlogSize_{ 10 };
	std::unique_ptr<asio::ip::tcp::acceptor> acceptor_{};
};

template<typename T>
class Server
{
public:
	explicit Server(Service& service, unsigned short port)
	{
		endPoint_ = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
		acceptor_ = std::move(std::make_unique<T>(service, ioc_, endPoint_));
	}

	explicit Server(unsigned short port)
	{
		endPoint_ = CreateEndpoint<asio::ip::tcp, asio::ip::address_v4>(port);
		acceptor_ = std::move(std::make_unique<T>(ioc_, endPoint_));
	}

	virtual ~Server()
	{
		Stop();
	}

	void Start()
	{
		if (!thread_.joinable())
		{
			thread_ = std::thread(&Server::Run, this);
		}
	}

	void Stop()
	{
		liveFlag = false;
		if(thread_.joinable())
		{
			thread_.join();
		}
	}

private:
	std::thread thread_;
	asio::io_context ioc_;
	asio::ip::tcp::endpoint endPoint_;
	std::atomic<bool> liveFlag{ true };
	std::unique_ptr<T> acceptor_{};

	void Run()
	{
		while (liveFlag)
		{
			acceptor_->Accept();
		}
	}
};
