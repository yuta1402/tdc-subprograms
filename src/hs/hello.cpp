#include <iostream>
#include "hs/hello.hpp"

namespace hs
{
    void Hello::hello()
    {
        std::cout << "Hello, world" << std::endl;
    }

    std::string Hello::getHello() const
    {
        return "Hello, world";
    }
}
