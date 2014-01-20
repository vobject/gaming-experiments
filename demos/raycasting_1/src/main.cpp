#include "LuaInterpreter.hpp"
#include "RcDemo.hpp"

#include <iostream>

int main()
{
    try
    {
        RcDemo::Instance().Start();
    }
    catch (const char* e)
    {
        std::cout << "ERROR: " << e << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception thrown!" << std::endl;
    }
}
