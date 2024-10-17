#include "gtest/gtest.h"
#include "easylog.hpp"

#include <iostream>
#include <sstream>

// testing template<>
// class stream_output_handler<std::ostream>:
// 		public base_output_handler

class rdbuf_cout_to_ssm: public testing::Test
{
protected:
	void SetUp() override
	{
		origin = std::cout.rdbuf(ossm.rdbuf());
	}

	void TearDown() override
	{
		std::cout.rdbuf(origin);
		origin = nullptr;
	}

	yq::stream_output_handler<std::ostream> out;
	std::ostringstream ossm;
	std::streambuf* origin = nullptr;
};

TEST_F(rdbuf_cout_to_ssm, stream_output_handler_write)
{
	out.write("hello world");
	EXPECT_EQ(ossm.str(), "hello world");
}

TEST_F(rdbuf_cout_to_ssm, stream_output_handler_other)
{
	out.write("output handler");
	out.line_break();
	EXPECT_NE(ossm.str().find("\n"), std::string::npos);
}

TEST_F(rdbuf_cout_to_ssm, stream_output_handler_polymorphism)
{
	yq::base_output_handler* p = &out;
	p->write("polymorphism");
	EXPECT_EQ(ossm.str(), "polymorphism");
}

//testing fstream_output_handler 

TEST(fstream_output_handler, throw_except)
{
	EXPECT_THROW({
		yq::fstream_output_handler handler("");
    }, std::runtime_error);
}

//testing stringstream_output_handler

TEST(stringstream_output_handler, base)
{
	yq::stringstream_output_handler handler;
	handler.write("hello");
	handler.line_break();
	EXPECT_EQ(handler.get_string(), "hello\n");
}

TEST(stringstream_output_handler, polymorphism)
{
	yq::stringstream_output_handler handler;
	yq::base_output_handler& b = handler;
	b.write("hello");
	b.line_break();
	EXPECT_EQ(handler.get_string(), "hello\n");
}

