#include "easylog.hpp"

int main()
{
	try
	{
		yq::fatal("hello");
		return 2;
	}
	catch(yq::log_fatal_error& e)
	{
		return 0;
	}
	return 2;
}
