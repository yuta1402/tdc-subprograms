#include "test.hpp"
#include "hs/hello.hpp"

void get_hello_test()
{
    hs::Hello h;
    auto s = h.getHello();
    eassert(s == "Hello, world", "s == %s", s.c_str());
}

void hello_test()
{
    get_hello_test();
}
