#ifndef __YQ_OUTPUT_HANDLER_HPP__
#define __YQ_OUTPUT_HANDLER_HPP__
#include <string_view>
#include <ostream>
#include <memory>
#include <concepts>
#include <type_traits>
#include <iostream>
#include <fstream>

namespace yq
{

template<typename Target>
class base_output_handler
{
public:
	virtual ~base_output_handler() = default;
	virtual void write(std::string_view data) = 0;
};

template<typename T>
concept ostream_concept = std::derived_from<T, std::ostream>
	&& requires(T& t)
{
	t << std::string_view{"sv"};
};

template<ostream_concept Ostream>
class stream_output_handler:
	public base_output_handler<Ostream>
{
};

template<>
class stream_output_handler<std::ostream>:
	public base_output_handler<std::ostream>
{
public:
	void write(std::string_view data) override
	{
		std::cout << data;
	}
};

template<>
class stream_output_handler<std::ofstream>:
	public base_output_handler<std::ofstream>
{
public:
	stream_output_handler(std::string_view sv):
		m_os { sv.data() }
	{
		
	}

	void write(std::string_view data) override
	{
		m_os<<data;
	}
private:
	std::ofstream m_os;
};

}	// namespace yq

#endif //__YQ_OUTPUT_HANDLER_HPP__
