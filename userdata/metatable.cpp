/*
ʹ�� userdata ���а�ȫ©����
���������ʵ����һ���ܴ�İ�ȫ©��������ʹ����д���������ƵĴ��룺
array.set(io.stdin, 1, 0)�� io.stdin �е�ֵ��һ������ָ����(FILE*)��ָ��� userdatum����
Ϊ����һ�� userdatum������ array.set �������������Ϊ�������������еĽ�����ܵ���
�ڴ� core dump������㹻���˵Ļ�������ܵõ�һ������Խ�磨index-out-of-range����
�󣩡������Ĵ�������κ�һ�� Lua ����˵���ǲ������ܵġ����������ʹ��һ�� C �⣬
����Ӧ���ƻ� C ���ݻ��ߴ� Lua ���� core dump��
*/

#include <lua.hpp>
#include <iostream>
#pragma comment (lib,"lua5.3.0.lib")

typedef struct NumArray
{
    int size;
    double values[1];
} NumArray;

// �������Ƿ���Ч
static NumArray* checkArray(lua_State* L)
{
    void* ud = luaL_checkudata(L, 1, "Meta.array");
    luaL_argcheck(L, ud != nullptr, 1, "array expected");
    return (NumArray*)ud;
}

// ��ȡԪ��
static double* getElement(lua_State* L)
{
    NumArray* a = checkArray(L);
    int index = luaL_checkinteger(L, 2);
    luaL_argcheck(L, 1 <= index && index < a->size, 2,
        "index out of range");
    return &a->values[index - 1];
}

// lua �����ʹ����ֵ��ֵ
static int newArray(lua_State* L)
{
    int n = luaL_checkinteger(L, 1);
    size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);
    NumArray* a = (NumArray*)lua_newuserdata(L, nbytes);
    //-----------difference---------------
    luaL_getmetatable(L, "Meta.array");
    lua_setmetatable(L, -2);
    a->size = n;
    return 1;
}

static int setArray(lua_State* L)
{
    double newvalue = luaL_checknumber(L, 3);
    *getElement(L) = newvalue;
    return 0;
}

static int getArray(lua_State* L)
{
   /* NumArray* a = (NumArray*)lua_touserdata(L, 1);
    int index = luaL_checkinteger(L, 2);
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 1 <= index && index <= a->size, 2,
        "index out of range");*/
    lua_pushnumber(L, *getElement(L));
    return 1;
}


static int getSize(lua_State* L) {
    /*   NumArray* a = (NumArray*)lua_touserdata(L, 1);
       luaL_argcheck(L, a != NULL, 1, "`array' expected");*/
    NumArray* a = checkArray(L);
    lua_pushnumber(L, a->size);
    return 1;
}

// lual_reg --> luaL_Reg  ע����lua�еĺ������֣���ӦC�ĺ���
static const struct luaL_Reg arraylib[] = {
{"new", newArray},
{"set", setArray},
{"get", getArray},
{"size", getSize},
{NULL, NULL},
};



// �򿪿�֮�����ǾͿ�����Lua��ʹ�ö���������� ע�� 5.2�Ժ� �Ƴ���luaL_openlib  
//  https://www.it1352.com/1582618.html
static int luaOpen_Array(lua_State* L)
{
    luaL_newmetatable(L, "Meta.array");
    luaL_setfuncs(L, arraylib, 0);
    lua_setglobal(L, "array");
    return 1;
}

int main()
{
    auto luaEnv = luaL_newstate();
    luaL_openlibs(luaEnv);
    luaOpen_Array(luaEnv);
    int ret = luaL_dofile(luaEnv, "userdata.lua");
    if (ret) return -1;
    return 0;
}

