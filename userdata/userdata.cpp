// userdata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <lua.hpp>
#include <iostream>
#pragma comment (lib,"lua5.3.0.lib")

typedef struct NumArray
{
    int size;
    double values[1];
} NumArray;

// lua 中如何使用数值的值
static int newArray(lua_State* L)
{
    int n = luaL_checkinteger(L, 1);
    size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);
    NumArray* a = (NumArray*)lua_newuserdata(L, nbytes);
    a->size = n;
    return 1; //此时 新的 userdata 已经在栈上了
}

static int setArray(lua_State* L)
{
    NumArray* a = (NumArray*)lua_touserdata(L, 1);
    int index = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);
    luaL_argcheck(L, a != nullptr, 1, "'arrary' expected");
    luaL_argcheck(L, 1 <= index && index <= a->size, 2, "index out of range");
    a->values[index - 1] = value;
    return 0;
}

static int getArray(lua_State* L)
{
    NumArray* a = (NumArray*)lua_touserdata(L, 1);
    int index = luaL_checkinteger(L, 2);
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 1 <= index && index <= a->size, 2,
        "index out of range");
    lua_pushnumber(L, a->values[index - 1]);
    return 1;
}


static int getSize(lua_State* L) {
    NumArray* a = (NumArray*)lua_touserdata(L, 1);
    luaL_argcheck(L, a != NULL, 1, "`array' expected");
    lua_pushnumber(L, a->size);
    return 1;
}

// lual_reg --> luaL_Reg  注册在lua中的函数名字，对应C的函数
static const struct luaL_Reg arraylib[] = {
{"new", newArray},
{"set", setArray},
{"get", getArray},
{"size", getSize},
{NULL, NULL},
};

// 打开库之后，我们就可以在Lua中使用定义的类型了 注意 5.2以后 移除了luaL_openlib  
//  https://www.it1352.com/1582618.html
int luaOpen_Array(lua_State* L)
{
    lua_newtable(L);
    luaL_setfuncs(L, arraylib, 0); 
    lua_setglobal(L, "array");
    return 1;
}
//
//int main()
//{
//    auto luaEnv = luaL_newstate();
//    luaL_openlibs(luaEnv);
//    luaOpen_Array(luaEnv);
//    int ret = luaL_dofile(luaEnv, "userdata.lua");
//    if (ret) return -1;
//    return 0;
//}

/*
我们上面的实现有一个很大的安全漏洞。假如使用者写了如下类似的代码：
array.set(io.stdin, 1, 0)。 io.stdin 中的值是一个带有指向流(FILE*)的指针的 userdatum。因
为它是一个 userdatum，所以 array.set 很乐意接受它作为参数，程序运行的结果可能导致
内存 core dump（如果你够幸运的话，你可能得到一个访问越界（index-out-of-range）错
误）。这样的错误对于任何一个 Lua 库来说都是不能忍受的。不论你如何使用一个 C 库，
都不应该破坏 C 数据或者从 Lua 产生 core dump。
*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
