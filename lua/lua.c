#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int main(int argc, char *argv[])
{
    lua_State *L = luaL_newstate();

    lua_pushstring(L, "I am so cool~");
    lua_pushnumber(L,20);

    if (lua_isstring(L,1)) {
        printf("str = %s\n", lua_tostring(L,1));
    }
    if (lua_isnumber(L,2)) {
        printf("int = %f\n", lua_tonumber(L,2));
    }

    luaL_openlibs(L);

    // we can use this to load and run or load then pcall separatlly
    //int ret = luaL_dofile(L, "/Users/jxue/test/lua/test.lua");
    int ret = luaL_loadfile(L, "/Users/jxue/test/lua/test.lua");
    if (ret) {
        fprintf(stderr, "load file fail! ret=%d\n", ret);
        return ret;
    }
    ret = lua_pcall(L, 0, 0, 0);
    if (ret) {
        fprintf(stderr, "run file fail! ret = %d\n", ret);
        return ret;
    }
#if 0
    ret = lua_pcall(L, 0, 0, 0);
    if (ret) {
        fprintf(stderr, "run file fail! ret = %d\n", ret);
        return ret;
    }
    if (lua_isstring(L,lua_gettop(L))) {
        printf("str = %s\n", lua_tostring(L,1));
    }
#endif
    lua_getglobal(L, "x");
    printf("str = %s\n", lua_tostring(L,-1));

    // load function
    lua_getglobal(L, "func");
    // give the parameter
    lua_pushnumber(L, 10);
    // call the function
    ret = lua_pcall(L, 1, 1, 0);
    if (ret) {
        fprintf(stderr, "run func fail! ret=%d\n", ret);
        return ret;
    }

    if(lua_isnumber(L, -1))
    {
        printf("int = %f\n", lua_tonumber(L,-1));
    }

    lua_close(L);
}
