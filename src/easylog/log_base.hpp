#ifndef __YQ_LOGGER_BASE_HPP__
#define __YQ_LOGGER_BASE_HPP__
#include <stdexcept>
#include <variant>
#include <memory>

namespace yq
{

/**
 * !!! Forbidden to give a fiexed value (except first value)
 */
enum class log_level
{
    info = 0,
    warn,
    debug,
    error,
    fatal,
	log_size
};

[[nodiscard]] 
constexpr auto log_level_to_string(log_level level) -> const char*
{
	switch(level)
	{
	case log_level::info:
		return "Info";
	case log_level::warn:
		return "Warning";
	case log_level::debug:
		return "Debug";
	case log_level::error:
		return "Error";
	case log_level::fatal:
		return "Fatal";
	default:
		return "Unkown";
	}
}

/**
 * If passing log_level::fatal as log_level,
 * the logger::log function will throw this exception
 */
class log_fatal_error: public std::exception { };

class log_construct_error: public std::runtime_error
{
public:
	log_construct_error(std::string_view sv):
		runtime_error { sv.data() }
	{}
};

//Base
template<template<log_level> typename, typename>
struct log_level_variant_generator;

template<template<log_level> typename Wrapper, std::size_t... Idices>
struct log_level_variant_generator<Wrapper, std::index_sequence<Idices...>>
{
	using type = std::variant<Wrapper<static_cast<log_level>(Idices)>...>;
};

template<template<log_level> typename Wrapper>
using log_level_variant_generator_t = typename log_level_variant_generator<Wrapper,
	  std::make_index_sequence<static_cast<std::size_t>(log_level::log_size)>>::type;

//reference_wrapper
//template<template<log_level> typename, typename>
//struct log_level_variant_ref_generator;
//
//template<template<log_level> typename Wrapper, std::size_t... Idices>
//struct log_level_variant_ref_generator<Wrapper, std::index_sequence<Idices...>>
//{
//	using type = std::variant<std::reference_wrapper<Wrapper<static_cast<log_level>(Idices)>>...>;
//};
//
//template<template<log_level> typename Wrapper>
//using log_level_variant_ref_generator_t = typename log_level_variant_generator<Wrapper,
//	  std::make_index_sequence<static_cast<std::size_t>(log_level::log_size)>>::type;


//unique_ptr
template<template<log_level> typename, typename>
struct log_level_variant_uptr_generator;

template<template<log_level> typename Wrapper, std::size_t... Idices>
struct log_level_variant_uptr_generator<Wrapper, std::index_sequence<Idices...>>
{
	using type = std::variant<std::unique_ptr<Wrapper<static_cast<log_level>(Idices)>>...>;
};

template<template<log_level> typename Wrapper>
using log_level_variant_uptr_generator_t = typename log_level_variant_uptr_generator<Wrapper,
	  std::make_index_sequence<static_cast<std::size_t>(log_level::log_size)>>::type;

template<log_level level>
struct level_trait
{
	static constexpr const char* level_msg = log_level_to_string(level);
};


}	//namespace yq

#endif	//__YQ_LOGGER_BASE_HPP__

