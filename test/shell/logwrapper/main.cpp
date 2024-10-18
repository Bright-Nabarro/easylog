#include "easylog.hpp"

int main()
{
	yq::info("hello world");
	yq::info(yq::loc(), "hello world");
	yq::warn("warnning msg");
	yq::debug("utf-8");
	yq::error("mvmvm");
	yq::fatal("nv");
}

