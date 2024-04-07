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

	inline const boost::system::error_category& get_http_errors_category()
	{
		static http_errors_category category;
		return category;
	}

	inline boost::system::error_code make_error_code(http_error_codes error)
	{
		return boost::system::error_code(
			static_cast<int>(error), get_http_errors_category());
	}
}

namespace boost
{
	namespace system
	{
		template<>
		struct is_error_code_enum
			<http_errors::http_error_codes>
		{
			BOOST_STATIC_CONSTANT(bool, value = true);
		};
	}
}
