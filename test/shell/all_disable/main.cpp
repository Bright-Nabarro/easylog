#include "easylog.hpp"

int main()
{
    yq::info("hello {} !", "easyly");
    yq::info(yq::loc(), "yq::loc trick file,line,function location");
    yq::debug("debug message canbe disable by NDEBUG macro");
    yq::warn("warnning msg");
    yq::error("error msg");
}

