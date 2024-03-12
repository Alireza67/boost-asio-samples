#pragma once
#include "pch.h"
#include "asiolib/io_buffers.hpp"

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
