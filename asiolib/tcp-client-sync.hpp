#pragma once
#include "pch.h"
#include "endpoints.hpp"
#include "sockets.hpp"

class SyncTCPClient
{
public:
	SyncTCPClient(const std::string& ip, unsigned short port)
		: endpoint_(std::move(CreateEndpoint<asio::ip::tcp>(ip, port))),
		  socket_(CreateAndOpenSocket<asio::ip::tcp>(ioc_, endpoint_.protocol()))
	{
	}

	~SyncTCPClient()
	{
		Close();
	}

	void Connect()
	{
		socket_.connect(endpoint_);
	}

	void Close()
	{
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_.close();
	}

	std::string EmulateLongComputationOp(unsigned int duration_sec)
	{
		std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + '\n';
		SendRequest(request);
		return ReceiveResponse();
	};

private:
	void SendRequest(const std::string& request)
	{
		asio::write(socket_, asio::buffer(request));
	}

	std::string ReceiveResponse()
	{
		asio::streambuf buffer;
		asio::read_until(socket_, buffer, '\n');
		std::istream input(&buffer);
		std::string response;
		std::getline(input, response);
		return response;
	}

private:
	asio::io_context ioc_;
	asio::ip::tcp::endpoint endpoint_;
	asio::ip::tcp::socket socket_;
};
