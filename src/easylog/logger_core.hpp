#ifndef __YQ_LOGGER_CORE_HPP__
#define __YQ_LOGGER_CORE_HPP__

#include <cassert>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

#include "log_msg.hpp"


namespace yq
{

class base_logger_core
{
public:
	base_logger_core() = default;
	virtual ~base_logger_core() = default;
	virtual void push_msg(std::unique_ptr<base_log_msg> msg) = 0;
	virtual void check_fatal(log_level level) = 0;
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
		std::lock_guard lock { m_mtx };
		m_msg_que.push(std::move(msg));
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
		m_running.store(false, std::memory_order_release);
		m_cond.notify_one();
	}

	void thd_call_back()
	{
		while(true)
		{
			std::unique_lock lock { m_mtx };
			m_cond.wait(lock, [this] {
				return !m_msg_que.empty() || !m_running.load(std::memory_order_acquire);
			});
			
			if (!m_running.load(std::memory_order_acquire)
			 && m_msg_que.empty())
				break;
			
			auto msg = std::move(m_msg_que.front());
			m_msg_que.pop();
			lock.unlock();

			std::ostream* os = nullptr;
			if (msg->get_os().has_value())
				os = &msg->get_os()->get();
			else if(msg->get_level() == log_level::error
				|| msg->get_level() == log_level::fatal)
				os = &std::cerr;
			else
				os = &std::cout;

			(*os)<<msg->log_string();
			if (msg->enable_flush())
				(*os)<<std::endl;
			else
				(*os)<<'\n';
		}
	}
	
private:
	std::queue<std::unique_ptr<base_log_msg>> m_msg_que;
	//donot use m_running.store(false) singlely
	std::atomic<bool> m_running;
	std::mutex m_mtx;
	std::condition_variable m_cond;
	std::thread m_thd;
};


}	//namespace yq

#endif //__YQ_LOGGER_CORE_HPP__
