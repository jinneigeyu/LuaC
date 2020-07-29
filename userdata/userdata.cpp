// userdata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <lua.hpp>
#include<limits.h>
#include <iostream>
#pragma comment (lib,"lua5.3.0.lib")

//#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))
//#define I_WORD(i) ((unsigned int)(i)/BITS_PER_WORD)
//#define I_BIT(i) (1<<((unsigned int)(i)%BITS_PER_WORD))
/* 
BITS_PER_WORD 是无符号整数中的位数。
I_WORD 计算存储与给定索引对应的位的we，
I_BIT 计算掩码以访问该字内的正确位
*/

typedef struct BitArray
{
    int size;
    double  values[1];
} BitArray;

// lua 中如何使用数值的值
static int newarray(lua_State* L) {
    int i;
    size_t nbtyes;
    BitArray* a;
    int n = (int)luaL_checkinteger(L, 1); /* number of bits */

    luaL_argcheck(L, n >= 1, 1, "invalid size");
    nbtyes = sizeof(BitArray) + (n - 1) * sizeof(double);
    a = (BitArray*)lua_newuserdata(L, nbtyes);
    a->size = n;
    for (i = 0; i <= n - 1; i++)
        a->values[i] = 0; /* initialize array */
    return 1; /* new userdata is already on the stack */
}

static int setarray(lua_State* L)
{
   
    BitArray* a = (BitArray*)lua_touserdata(L, 1);
    int index = luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, a != nullptr, 1, "'arrary' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    double v = luaL_checknumber(L, 3);//lua 里形式 array.set(a, i, i)所以 1 2 3 的位置对应是数组、索引、 值
    a->values[index] = v;
    return 0;
}

static int getarray(lua_State* L) {
    BitArray* a = (BitArray*)lua_touserdata(L, 1);
    int index = (int)luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2,
        "index out of range");    
    lua_pushnumber(L, a->values[index] );
    return 1;
}


static int getSize(lua_State* L) {
    BitArray* a = (BitArray*)lua_touserdata(L, 1);
    luaL_argcheck(L, a != NULL, 1, "`array' expected");
    lua_pushinteger(L, a->size);
    return 1;
}

// lual_reg --> luaL_Reg  注册在lua中的函数名字，对应C的函数
static const struct luaL_Reg arraylib[] = {
{"new", newarray},
{"set", setarray},
{"get", getarray},
{"size", getSize},
{NULL, NULL},
};

// 打开库之后，我们就可以在Lua中使用定义的类型了 注意 5.2以后 移除了luaL_openlib  
//  https://www.it1352.com/1582618.html\
//  https://blog.csdn.net/linuxheik/article/details/52454640
int luaOpen_Array(lua_State* L)
{
    luaL_newlib(L, arraylib);
    return 1;
}


//int main()
//{
//    lua_State* L = luaL_newstate();
//    luaL_openlibs(L);
//    luaL_requiref(L, "array", luaOpen_Array, 1); // 把 array 传进去 //
//    luaL_dofile(L, "userdata.lua");
//    
//    return 0;
//
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
