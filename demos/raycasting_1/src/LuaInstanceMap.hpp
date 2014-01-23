#ifndef LUA_INSTANCE_MAP_HPP
#define LUA_INSTANCE_MAP_HPP

#include <map>

struct lua_State;

template<class T>
class LuaInstanceMap
{
public:
    void Add(lua_State* const L, T& inst)
    {
        instances.insert(std::make_pair(L, std::ref(inst)));
    }

    void Remove(lua_State* const key)
    {
        instances.erase(key);
    }

    void Remove(T& val)
    {
        for (auto it = instances.begin(); it != instances.end(); it++)
        {
            if (&it->second == &val) {
                instances.erase(it);
                break;
            }
        }
    }

    T& Get(lua_State* const L)
    {
        const auto iter = instances.find(L);
        if (iter == instances.end()) {
            throw "Trying to access non-existing reference.";
        }
        return iter->second;
    }

private:
    std::map<lua_State*, T&> instances;
};

#endif // LUA_INSTANCE_MAP_HPP
