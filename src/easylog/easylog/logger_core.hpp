#ifndef __YQ_LOGGER_CORE_HPP__
#define __YQ_LOGGER_CORE_HPP__

#include <cassert>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>
#include "output_handler.hpp"

#include "log_msg.hpp"

namespace yq
{

class base_logger_core
{
public:
	base_logger_core():
		// default std::cout
		m_output 	{
			std::make_unique<stream_output_handler<std::ostream>>()
		}
	{ }
	virtual ~base_logger_core() = default;
	virtual void push_msg(std::unique_ptr<base_log_msg> msg) = 0;
	virtual void check_fatal(log_level level)
	{
		if (level == log_level::fatal)
			throw log_fatal_error{};
	}
	virtual void change_output(std::unique_ptr<base_output_handler> uptr_os
		= std::make_unique<stream_output_handler<std::ostream>>())
	{
		m_output = std::move(uptr_os);
	}

protected:
	std::unique_ptr<base_output_handler> m_output;
};


class logger_core: public base_logger_core
{
public:
	static auto instance() -> logger_core&
	{
		static logger_core instance;

		return instance;
	}

	void push_msg(std::unique_ptr<base_log_msg> msg) override
	{
		{
			std::lock_guard lock { m_mtx };
			m_msg_que.push(std::move(msg));
		}
		m_cond.notify_one();
	}

	void check_fatal(log_level level) override
	{
		if (level == log_level::fatal)
		{
			end_running();
			m_thd.join();
			throw log_fatal_error{};
		}
	}

    void change_output(
        std::unique_ptr<base_output_handler> uptr_os =
            std::make_unique<stream_output_handler<std::ostream>>()) override
    {
        std::lock_guard output_lock{m_output_mtx};
        base_logger_core::change_output(std::move(uptr_os));
    }

#ifdef YQ_EASYLOG_UNIT_TEST
	auto get_output_mtx() -> std::mutex&
	{
		return m_output_mtx;
	}
#endif

protected:
	logger_core():
		m_msg_que 	{},
		m_running 	{ true },
		m_mtx 		{},
		m_cond 		{},
		m_thd 		{ &logger_core::thd_call_back, this }
	{ }

	virtual ~logger_core()
	{
		end_running();
		if (m_thd.joinable())
			m_thd.join();
		assert(m_msg_que.empty());
	}

private:
	void end_running()
	{
		m_running.store(false);
		m_cond.notify_all();
	}

	void thd_call_back()
	{
		auto write_to_output = [this](auto& msg)
		{
			std::lock_guard output_lock{ m_output_mtx };
			m_output->write(msg->log_string());
			m_output->line_break();
			if (msg->enable_flush())
				m_output->flush();
		};

		while(true)
		{
			std::unique_lock lock { m_mtx };
			m_cond.wait(lock, [this] {
				return !m_msg_que.empty() || !m_running.load();
			});
			
			if (!m_running.load()
			 && m_msg_que.empty())
				break;
			
			auto msg = std::move(m_msg_que.front());
			m_msg_que.pop();
			lock.unlock();
			
			write_to_output(msg);
		}
	}
	
private:
	std::queue<std::unique_ptr<base_log_msg>> m_msg_que;
	//donot use m_running.store(false) singlely
	std::atomic<bool> m_running;
	std::mutex m_mtx;
	std::condition_variable m_cond;
	std::mutex m_output_mtx;
	std::thread m_thd;
};


}	//namespace yq

#endif //__YQ_LOGGER_CORE_HPP__

