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

const std::string metaName = "Meta.array";

typedef struct BitArray
{
    int size;
    double values[1];
} BitArray;

// �������Ƿ���Ч
static BitArray* checkArray(lua_State* L)
{
    void* ud = luaL_checkudata(L, 1, metaName.c_str());

    luaL_argcheck(L, ud != nullptr, 1, "array expected");
    return (BitArray*)ud;
}

// ��ȡԪ��
static double* getElement(lua_State* L)
{
    BitArray* a = checkArray(L);
    int index = luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, 0 <= index && index < a->size, 2,
        "index out of range");

    return &(a->values[index]);
}

// lua �����ʹ����ֵ��ֵ
static int newArray(lua_State* L)
{
    int n = luaL_checkinteger(L, 1); //  ��ȡ new��n��  ��n 
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

// lual_reg --> luaL_Reg  ע����lua�еĺ������֣���ӦC�ĺ���
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



// �򿪿�֮�����ǾͿ�����Lua��ʹ�ö���������� ע�� 5.2�Ժ� �Ƴ���luaL_openlib  
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