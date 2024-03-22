#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS
#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
	#define BOOST_ASIO_DISABLE_IOCP
	#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <map>
#include <iostream>
using namespace boost;

typedef void(*Callback) (
	unsigned int request_id,
	const std::string& response,
	const system::error_code& ec);

struct Session
{
	Session(asio::io_context& ioc, const std::string& ip, unsigned short port,
			const std::string& request, unsigned int id, Callback callback)
		:m_sock(ioc), m_ep(asio::ip::address::from_string(ip), port),
		 m_request(request), m_id(id), m_callback(callback), m_was_cancelled(false)
	{
	}

	asio::ip::tcp::socket m_sock;
	asio::ip::tcp::endpoint m_ep;

	std::string m_request; 
	std::string m_response;
	asio::streambuf m_response_buf;
	
	system::error_code m_ec;
	unsigned int m_id; 
	Callback m_callback;
	bool m_was_cancelled;
	std::mutex m_cancel_guard;
};

class AsyncTCPClient : public boost::noncopyable
{
public:
	AsyncTCPClient()
	{
		m_work = std::move(std::make_unique<boost::asio::io_service::work>(m_ioc));
		m_thread = move(std::make_unique<std::thread>([this]()
			{
				m_ioc.run();
			}));
	}

	AsyncTCPClient(uint8_t numberOfThread)
	{
		if (!numberOfThread)
		{
			numberOfThread = static_cast<uint8_t>(std::thread::hardware_concurrency());
		}
		m_work = std::move(std::make_unique<boost::asio::io_service::work>(m_ioc));
		for (auto i{ 0 }; i < numberOfThread; ++i)
		{
			m_threads.emplace_back(std::make_unique<std::thread>([this]()
				{
					m_ioc.run();
				}));
		}
	}

	void EmulateLongComputationOp(
		unsigned int duration_sec,
		const std::string& ip,
		unsigned short port,
		Callback callback,
		unsigned int request_id)
	{
		std::string request = 
			"EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + '\n';

		auto session = std::make_shared<Session>(m_ioc, ip, port, request, 
												 request_id, callback);

		session->m_sock.open(session->m_ep.protocol());

		{
			std::lock_guard<std::mutex> lock(m_active_sessions_guard);
			m_active_sessions[request_id] = session;
		}

		session->m_sock.async_connect(
			session->m_ep,
			[this, session](const system::error_code& ec)
			{
				if (ec.value() != 0)
				{
					session->m_ec = ec;
					OnRequestComplete(session);
					return;
				}
				
				{
					std::lock_guard<std::mutex> cancel_lock(session->m_cancel_guard);
					if (session->m_was_cancelled)
					{
						OnRequestComplete(session);
						return;
					}
				}

				asio::async_write(
					session->m_sock, 
					asio::buffer(session->m_request),
					[this, session](const boost::system::error_code& ec,
						std::size_t bytes_transferred)
						{
							if (ec.value() != 0)
							{
								session->m_ec = ec;
								OnRequestComplete(session);
								return;
							}

							{
								std::unique_lock<std::mutex>
								cancel_lock(session->m_cancel_guard);
								if (session->m_was_cancelled)
								{
									OnRequestComplete(session);
									return;
								}
							}

							asio::async_read_until(
								session->m_sock,
								session->m_response_buf,
								'\n',
								[this, session](const boost::system::error_code& ec,
												std::size_t bytes_transferred)
								{
									if (ec.value() != 0) 
									{
										session->m_ec = ec;
									}
									else 
									{
										std::istream strm(&session->m_response_buf);
										std::getline(strm, session->m_response);
									}
									OnRequestComplete(session);
								});
					});
			});
	};

	void CancelRequest(unsigned int request_id)
	{
		std::lock_guard<std::mutex> lock(m_active_sessions_guard);
		auto it = m_active_sessions.find(request_id);
		if (it != m_active_sessions.end())
		{
			std::lock_guard<std::mutex> cancel_lock(it->second->m_cancel_guard);
			it->second->m_was_cancelled = true;
			it->second->m_sock.cancel();
		}
	}

	void Close() 
	{
		m_work.reset(nullptr);
		if (m_thread)
		{
			m_thread->join();
		}
		else
		{
			for (auto& item : m_threads)
			{
				item->join();
			}
		}
	}

private:
	void OnRequestComplete(std::shared_ptr<Session> session)
	{
		boost::system::error_code ignored_ec;
		session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
		
		{
			std::lock_guard<std::mutex> lock(m_active_sessions_guard);
			auto it = m_active_sessions.find(session->m_id);
			if (it != m_active_sessions.end())
			{
				m_active_sessions.erase(it);
			}
		}

		boost::system::error_code ec;
		if (session->m_ec.value() == 0 && session->m_was_cancelled)
		{
			ec = asio::error::operation_aborted;
		}
		else
		{
			ec = session->m_ec;
		}
		session->m_callback(session->m_id, session->m_response, ec);
	}

	private:
		asio::io_context m_ioc;
		std::mutex m_active_sessions_guard;
		std::unique_ptr<std::thread> m_thread{};
		std::vector<std::unique_ptr<std::thread>> m_threads{};
		std::unique_ptr<boost::asio::io_context::work> m_work{};
		std::map<int, std::shared_ptr<Session>> m_active_sessions;
};

void handler(
	unsigned int request_id,
	const std::string& response,
	const system::error_code& ec)
{
	if (ec.value() == 0)
	{
		std::cout << "Request #" << request_id
			<< " has completed. Response: "
			<< response << std::endl;
	}
	else if (ec == asio::error::operation_aborted)
	{
		std::cout << "Request #" << request_id
			<< " has been cancelled by the user."
			<< std::endl;
	}
	else
	{
		std::cout << "Request #" << request_id
			<< " failed! Error code = " << ec.value()
			<< ". Error message = " << ec.message()
			<< std::endl;
	}
}
