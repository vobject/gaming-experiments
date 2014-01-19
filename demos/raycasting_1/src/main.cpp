#include "LuaInterpreter.hpp"
#include "RcDemo.hpp"

#include <iostream>

int main()
{
    try
    {
        RcDemo& app = RcDemo::Instance();

        LuaInterpreter lua(app);
        lua.RunScript("raycasting.lua");
        app.Start();
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
