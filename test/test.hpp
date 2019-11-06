#ifndef TEST_HPP
#define TEST_HPP

#include <cstdlib>
#include <string>
#include <iostream>

namespace test
{
    namespace impl
    {
        static int passing{ true };
    }

    int exit()
    {
        if (impl::passing) {
            std::cout << "PASS" << std::endl;
            return 0;
        }

        std::cout << "FAIL" << std::endl;

        return 1;
    }
}

namespace impl
{
    bool fail()
    {
        test::impl::passing = false;
        return true;
    }

    bool exit()
    {
        std::exit(1);
        return true;
    }

    std::string get_file_name(const std::string& filepath)
    {
        auto pos = filepath.find_last_of("/");
        if (pos == std::string::npos) {
            return filepath;
        }

        return filepath.substr(pos+1);
    }
}

// extended assert ( with message )
#define eassert(expr,...) \
    (!(expr) \
     && printf("%s(%d) [expr: "#expr"]: ", impl::get_file_name(__FILE__).c_str(), __LINE__) \
     && printf(__VA_ARGS__) \
     && printf("\n") \
     && impl::fail())

#endif
