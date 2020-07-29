/*
使用 userdata 是有安全漏洞的
我们上面的实现有一个很大的安全漏洞。假如使用者写了如下类似的代码：
array.set(io.stdin, 1, 0)。 io.stdin 中的值是一个带有指向流(FILE*)的指针的 userdatum。因
为它是一个 userdatum，所以 array.set 很乐意接受它作为参数，程序运行的结果可能导致
内存 core dump（如果你够幸运的话，你可能得到一个访问越界（index-out-of-range）错
误）。这样的错误对于任何一个 Lua 库来说都是不能忍受的。不论你如何使用一个 C 库，
都不应该破坏 C 数据或者从 Lua 产生 core dump。
*/

#include <lua.hpp>
#include <iostream>
#pragma comment (lib,"lua5.3.0.lib")

const std::string metaName = "Meta.array";

typedef struct BitArray
{
    int size;
    double values[1];
} BitArray;

// 检查参数是否有效
static BitArray* checkArray(lua_State* L)
{
    void* ud = luaL_checkudata(L, 1, metaName.c_str());

    luaL_argcheck(L, ud != nullptr, 1, "array expected");
    return (BitArray*)ud;
}

// 获取元素
static double* getElement(lua_State* L)
{
    BitArray* a = checkArray(L);
    int index = luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, 0 <= index && index < a->size, 2,
        "index out of range");

    return &(a->values[index]);
}

// lua 中如何使用数值的值
static int newArray(lua_State* L)
{
    int n = luaL_checkinteger(L, 1); //  获取 new（n）  的n 
    int s = lua_gettop(L);
    size_t nbytes = sizeof(BitArray) + (n - 1) * sizeof(double);
    BitArray* a = (BitArray*)lua_newuserdata(L, nbytes);
    s = lua_gettop(L);
    for (int i = 0; i <= (n - 1); i++) {
        a->values[i] = 0;
    }
    a->size = n;
    //-----------difference---------------
    luaL_getmetatable(L, metaName.c_str());
    lua_setmetatable(L, -2);
    return 1;  /* new userdata is already on the stack */
}

int setArray(lua_State* L)
{
    double newvalue = luaL_checknumber(L, 3);
    *getElement(L) = newvalue;
    return 0;
}

int getArray(lua_State* L)
{
    auto v = getElement(L);
    lua_pushnumber(L, *v);
    return 1;
}


int getSize(lua_State* L) {
    /*   NumArray* a = (NumArray*)lua_touserdata(L, 1);
       luaL_argcheck(L, a != NULL, 1, "`array' expected");*/
    BitArray* a = checkArray(L);
    lua_pushnumber(L, a->size);
    return 1;
}
int array2String(lua_State* L)
{
    BitArray* a = checkArray(L);
    lua_pushfstring(L, "array(%d)", a->size);
    return 1;
}

// lual_reg --> luaL_Reg  注册在lua中的函数名字，对应C的函数
static const struct luaL_Reg arraylib_m[] = {
{"__tostring", array2String},
{"__newindex", setArray},
{"__index", getArray},
{"__len", getSize},
{NULL, NULL},
};

static const struct luaL_Reg arraylib_f[] = {
{"new", newArray},
{NULL, NULL},
};



// 打开库之后，我们就可以在Lua中使用定义的类型了 注意 5.2以后 移除了luaL_openlib  
//  https://www.it1352.com/1582618.html
static int luaOpen_Array(lua_State* L)
{

    luaL_newmetatable(L, metaName.c_str()); /* create metatable */
    lua_pushvalue(L, -1); /* duplicate the metatable */
    lua_setfield(L, -2, "__index"); /* mt.__index = mt */
    luaL_setfuncs(L, arraylib_m, 0); /* register metamethods */
    luaL_newlib(L, arraylib_f); /* create lib table */
    return 1;
}

int main()
{
    auto luaEnv = luaL_newstate();
    luaL_openlibs(luaEnv);
    //luaOpen_Array(luaEnv);
    luaL_requiref(luaEnv, "array", luaOpen_Array, 1);
    int ret = luaL_dofile(luaEnv, "accessArray.lua");

    if (ret) return -1;
    return 0;
}