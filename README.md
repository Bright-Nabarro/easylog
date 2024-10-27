# EasyLog
A simple header-only library written in C++20

Document: [chick this](https://bright-nabarro.github.io/easylog/index.html)

# QuickStart
#### Use logger wrapper function
```cpp
#include "easylog.hpp"

int main()
{
    yq::info("hello {} !", "easylog");
    yq::info(qy::loc, "yq::loc trick file,line,function location");
    yq::debug("debug message canbe disable by NDEBUG macro");
    yq::warn("warnning msg");
    yq::error("error msg");
    yq::fatal("fatal throw a exception, and donot catch it");
}
```

#### Use logger class ( Customize output stream )
```cpp
#include "easylog.hpp"

yq::logger logger;

void set_logger()
{
    logger.set_enable_time(true);
    logger.set_enable_flush(true);
    logger.chang_output(yq::make_fstream_handler("app.log"));
}

int main()
{
    using namespace yq;
    set_logger();   
    logger<log_level::info>("hello");
}
```

# Install / Build

### for unix
```bash
git clone https://github.com/Bright-Nabarro/easylog.git
cd easylog
cmake -B build
cmake --build build
cmake --install build
```

# TODO
- [x] Add `CTAD` in template class which use format_string.

- [x] Separate output in single module.

- [ ] Throw exception in fatal error in static way

- [ ] Separate queue management in single module.

- [ ] Move no template and explicit instantiation implementation to cpp file

- [ ] Finish shell test

- [ ] Coverage test include shell test

- [ ] Add memroy pool

