#pragma once
#include <boost/system.hpp>

namespace http_errors
{
	enum class http_error_codes: int
	{
		invalid_response = 1
	};

	class http_errors_category : public boost::system::error_category
	{
	public:
		const char* name() const BOOST_SYSTEM_NOEXCEPT
		{
			return "http_errors";
		}

		std::string message(int error) const
		{
			switch (error)
			{
			case static_cast<int>(http_error_codes::invalid_response):
				return "Server response cannot be parsed!";
			default:
				return "Unknown error!";
			}
		}
	};
}
