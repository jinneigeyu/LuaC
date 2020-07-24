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

// lua �����ʹ����ֵ��ֵ
static int newArray(lua_State* L)
{
    int n = luaL_checkinteger(L, 1);
    size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);
    NumArray* a = (NumArray*)lua_newuserdata(L, nbytes);
    a->size = n;
    return 1; //��ʱ �µ� userdata �Ѿ���ջ����
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
���������ʵ����һ���ܴ�İ�ȫ©��������ʹ����д���������ƵĴ��룺
array.set(io.stdin, 1, 0)�� io.stdin �е�ֵ��һ������ָ����(FILE*)��ָ��� userdatum����
Ϊ����һ�� userdatum������ array.set �������������Ϊ�������������еĽ�����ܵ���
�ڴ� core dump������㹻���˵Ļ�������ܵõ�һ������Խ�磨index-out-of-range����
�󣩡������Ĵ�������κ�һ�� Lua ����˵���ǲ������ܵġ����������ʹ��һ�� C �⣬
����Ӧ���ƻ� C ���ݻ��ߴ� Lua ���� core dump��
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
