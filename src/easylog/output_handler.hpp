#ifndef __YQ_OUTPUT_HANDLER_HPP__
#define __YQ_OUTPUT_HANDLER_HPP__
#include <string_view>
#include <ostream>
#include <concepts>
#include <sstream>
#include <iostream>
#include <fstream>

namespace yq
{

class base_output_handler
{
public:
	virtual ~base_output_handler() = default;
	virtual void write(std::string_view data) = 0;
	virtual void line_break() = 0;
	virtual void flush() = 0;

};


template<typename T>
concept ostream_derived = std::derived_from<T, std::ostream>
	&& requires(T& t)
{
	t << std::string_view{"sv"};
};


template<ostream_derived Ostream>
class stream_output_handler:
	public base_output_handler
{
};


template<>
class stream_output_handler<std::ostream>:
	public base_output_handler
{
public:
	stream_output_handler() = default;
	void write(std::string_view data) override
	{
		std::cout << data;
	}
	void line_break() override
	{
		std::cout << '\n';
	}
	void flush() override
	{
		std::cout << std::flush;
	}
};


template<typename T>
concept default_constructible_ostream = ostream_derived<T>
	&& requires()
{
	T{};
};


template<default_constructible_ostream Os>
class contains_stream_output_handler:
	public stream_output_handler<Os>
{
public:
	// Os may const, need limit
	contains_stream_output_handler (Os&& os):
		m_os { std::move(os) }
	{ }
	void write(std::string_view data) override
	{
		m_os<<data;
	}

	void line_break() override
	{
		m_os << '\n';
	}
	void flush() override
	{
		m_os << std::flush;
	}
protected:
	Os m_os;
};


class fstream_output_handler:
	public contains_stream_output_handler<std::ofstream>
{
public:
	fstream_output_handler(std::string_view sv):
		contains_stream_output_handler { std::ofstream(sv.data()) }
	{
		if (!m_os)
			throw std::runtime_error { "fstream construct error" };
	}
};


class stringstream_output_handler:
	public contains_stream_output_handler<std::ostringstream>
{
public:
	stringstream_output_handler():
		contains_stream_output_handler { std::ostringstream{} }
	{}

	auto get_string() -> std::string
	{
		auto ret = m_os.str();
		m_os.clear();
		return ret;
	};
};

}	// namespace yq

#endif //__YQ_OUTPUT_HANDLER_HPP__

