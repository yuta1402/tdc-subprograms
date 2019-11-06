#ifndef HELLO_HPP
#define HELLO_HPP

#include <string>

/*!
@namespace hs
@brief Hello subprograms
*/
namespace hs
{
    class Hello
    {
    public:
        Hello() = default;
        ~Hello() = default;

        void hello();
        std::string getHello() const;
    };
}

#endif
