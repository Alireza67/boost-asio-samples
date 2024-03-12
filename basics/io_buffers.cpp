#pragma once
#include "pch.h"
#include "asiolib/io_buffers.hpp"
#include <fstream>

TEST(buffers, mutable_buffers)
{
	std::string buffer{"Hello Dear World!"};
	auto asioBuffer = GetBuffer(buffer);
	EXPECT_EQ(asioBuffer.size(), buffer.size());
}

TEST(buffers, mutableBuffer_invalidated_by_operation_on_string)
{
	std::string buffer{ "Hello Dear World!" };
	auto asioBuffer = GetBuffer(buffer);
	EXPECT_EQ(asioBuffer.size(), buffer.size());
	buffer.assign("Hello hell!");
	EXPECT_NE(asioBuffer.size(), buffer.size());
}

TEST(buffers, const_buffers)
{
	const std::string buffer{ "Hello Dear World!" };
	auto asioBuffer = GetBuffer(buffer);
	EXPECT_EQ(asioBuffer.size(), buffer.size());
}

TEST(buffers, streambuf)
{
	asio::streambuf buffer;
	std::ostream output(&buffer);
	std::istream input(&buffer);

	output << "This is a simple test!\nTry to understand with more details!\n";

	std::string msg;
	std::getline(input, msg);
	EXPECT_EQ("This is a simple test!", msg);
	std::getline(input, msg);
	EXPECT_EQ("Try to understand with more details!", msg);
	std::getline(input, msg);
	EXPECT_TRUE(msg.empty());
}
