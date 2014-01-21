#include "LuaInterpreter.hpp"
#include "MainLoop.hpp"

#include <iostream>

int main()
{
    try
    {
        MainLoop().Run();
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
