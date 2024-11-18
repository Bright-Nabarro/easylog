/**
 * @file output_handler.hpp
 * @brief Contains output handler components for managing various output streams.
 */
#ifndef __YQ_OUTPUT_HANDLER_HPP__
#define __YQ_OUTPUT_HANDLER_HPP__
#include <string_view>
#include <ostream>
#include <concepts>
#include <sstream>
#include <iostream>
#include <fstream>
#include <format>
#include <memory>

#include "log_base.hpp"

namespace yq
{

/**
 * @brief Contains an object(reference) for output,
 * 	managed and synchronized by yq::logger_core
 */
class base_output_handler
{
public:
	virtual ~base_output_handler() = default;
	/**
	 * @brief Display the data
	 * @note Invoking this method may not apply the changes immediately unless `flush` is called.
	 */
	virtual void write(std::string_view data) = 0;
	/**
     * @brief Write a line break to the output.
     */
	virtual void line_break() = 0;
	/**
	 * @brief Flush buffer, like std::flush
	 */
	virtual void flush() = 0;
};


/**
 * @brief Concept to check if a type is derived from std::ostream and supports string output.
 * @tparam T Type to check.
 */
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
			exception_handle<std::runtime_error>(std::format("fstream construct error with file_path {} ", sv.data()));
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
		std::string ret = m_os.str();
		m_os.str("");
		return ret;
	};

};

template<typename T>
concept output_handler_driver = std::derived_from<T, base_output_handler>;

/**
 * @brief generate general stream_output_handler uptr
 * @tparam T an original handler type with brived from base_output_handler
 * @param construct arguments for T
 * @return a handle for the logger.chang_output accept
 */
template<output_handler_driver T, typename... Args>
auto make_output_handler(Args... args)
	-> std::unique_ptr<T>
{
	return std::make_unique<T>(args...);
}

/**
 * @brief generate fstream_output_handler uptr
 * @param file_name logger output file name 
 * @return a handle for the logger.chang_output accept
 */
inline auto make_fstream_handler(std::string_view file_name)
	-> std::unique_ptr<fstream_output_handler>
{
	return std::make_unique<fstream_output_handler>(file_name);
}

}	// namespace yq

#endif //__YQ_OUTPUT_HANDLER_HPP__


