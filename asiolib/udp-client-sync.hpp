#pragma once
#include "pch.h"
#include "endpoints.hpp"
#include "sockets.hpp"

class SyncUDPClient
{
public:
	SyncUDPClient() :
		socket_(CreateAndOpenSocket<asio::ip::udp>(ioc_))
	{
	}

	std::string EmulateLongComputationOp(
		unsigned int duration_sec,
		const std::string& ip,
		unsigned short port)
	{
		std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + '\n';
		auto endPoint = CreateEndpoint<asio::ip::udp>(ip, port);
		SendRequest(endPoint, request);
		return ReceiveResponse(endPoint);
	};

private:
	void SendRequest(const asio::ip::udp::endpoint& ep, const std::string& request)
	{
		socket_.send_to(asio::buffer(request), ep);
	}

	std::string ReceiveResponse(asio::ip::udp::endpoint& ep)
	{
		char response[6];
		std::size_t bytes_recieved = socket_.receive_from(asio::buffer(response), ep);
		socket_.shutdown(asio::ip::udp::socket::shutdown_both);
		return std::string(response, bytes_recieved);
	}

private:
	asio::io_context ioc_;
	asio::ip::udp::socket socket_;
};
