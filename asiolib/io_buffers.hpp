#pragma once
#include "pch.h"
#include <boost/asio.hpp>

using namespace boost;

auto GetBuffer(std::string& inputBuffer)
{
	return asio::buffer(inputBuffer);
}

auto GetBuffer(const std::string& inputBuffer)
{
	return asio::buffer(inputBuffer);
}

auto GetBuffer(void* inputBuffer, const size_t size)
{
	return asio::buffer(inputBuffer, size);
}

auto GetBuffer(const void* inputBuffer, const size_t size)
{
	return asio::buffer(inputBuffer, size);
}
