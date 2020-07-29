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
BITS_PER_WORD ���޷��������е�λ����
I_WORD ����洢�����������Ӧ��λ��we��
I_BIT ���������Է��ʸ����ڵ���ȷλ
*/

typedef struct BitArray
{
    int size;
    double  values[1];
} BitArray;

// lua �����ʹ����ֵ��ֵ
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
    double v = luaL_checknumber(L, 3);//lua ����ʽ array.set(a, i, i)���� 1 2 3 ��λ�ö�Ӧ�����顢������ ֵ
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

// lual_reg --> luaL_Reg  ע����lua�еĺ������֣���ӦC�ĺ���
static const struct luaL_Reg arraylib[] = {
{"new", newarray},
{"set", setarray},
{"get", getarray},
{"size", getSize},
{NULL, NULL},
};

// �򿪿�֮�����ǾͿ�����Lua��ʹ�ö���������� ע�� 5.2�Ժ� �Ƴ���luaL_openlib  
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
//    luaL_requiref(L, "array", luaOpen_Array, 1); // �� array ����ȥ //
//    luaL_dofile(L, "userdata.lua");
//    
//    return 0;
//
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
