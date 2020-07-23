#include <lua.hpp>
#include<stdio.h>
#include<iostream>

using namespace std;

struct ColorTable
{
    char* name;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

ColorTable ct[] = {
    {(char*)"WHITE",1,1,1},
    {(char*)"RED",1,0,0},
    {(char*)"NULL",0,0,0},

};

void printParamsNums(lua_State* l)
{
    printf(">> stack params %d \n", lua_gettop(l));
}

//创建一个table 压入 lua环境中去
void setColor(lua_State* l, ColorTable* ct) 
{
    cout << "before create a new table : stack params capicity:" << lua_gettop(l) << endl;
    lua_newtable(l);
    cout << "afetr a table create :" << lua_gettop(l) << endl;

    cout << "push number red" << endl;
    lua_pushnumber(l, ct->red);
    printParamsNums(l);
    cout << "set field" << endl;
    lua_setfield(l, -2, "r"); // 此时新建的table 的索引是-2  set 之后 栈顶数据 ct-red 出栈，传入table里
    printParamsNums(l);

    cout << "push number green" << endl;
    lua_pushnumber(l, ct->green);
    printParamsNums(l);
    cout << "set field" << endl;
    lua_setfield(l, -2, "g");
    printParamsNums(l);

    cout << "push number green" << endl;
    lua_pushnumber(l, ct->blue);
    printParamsNums(l);
    cout << "set field" << endl;
    lua_setfield(l, -2, "b");
    printParamsNums(l);

    cout << "set global table name" << endl;
    lua_setglobal(l, ct->name);//弹出talbe，并修改表的名字  ****务必注意****
    printParamsNums(l);
}




//加载lua文件
void load(lua_State* l, const char* name, int* w, int* h) {
    if (luaL_loadfile(l, name) || lua_pcall(l, 0, 0, 0)) {
        printf("cannot run lua file : %s\n", lua_tostring(l, -1));
    }
    std::cout << "get global width" << std::endl;
    lua_getglobal(l, "width");
    printParamsNums(l);

    if (!lua_isnumber(l, -1)) {
        std::cout << "width in lua file is not a number type" << std::endl;
    }
    std::cout << "get global height" << std::endl;
    lua_getglobal(l, "height");
    printParamsNums(l);

    if (!lua_isnumber(l, -1)) {
        std::cout << "width in lua file is not a number type" << std::endl;
    }

    *w = lua_tonumber(l, -2);
    *h = lua_tonumber(l, -1);

    std::cout << "clear stack  lua_settop" << std::endl;
    lua_settop(l, 0);//清空栈
    printParamsNums(l);
}


// 永远记住 交互栈 是基于栈顶操作的
void main() {
    lua_State* luaEnv = luaL_newstate();
    printParamsNums(luaEnv);
    luaL_openlibs(luaEnv);
    int w = 0;
    int h = 0;
    load(luaEnv, "test.lua", &w, &h);
    ////////////////////////////从Lua文件中获取

    std::cout << "get global table bg" << std::endl;
    lua_getglobal(luaEnv, "bg");//获取 bg {},
    printParamsNums(luaEnv);

    std::cout << "push string r " << std::endl;
    lua_pushstring(luaEnv, "r");//压入 key “r”
    printParamsNums(luaEnv);

    std::cout << "get table  " << std::endl;
    lua_gettable(luaEnv, -2);//获取 bg[r]
    printParamsNums(luaEnv);
    //cout << lua_gettop(luaEnv) << endl;

    std::cout << "get red bg.r  " << std::endl;
    double red = lua_tonumber(luaEnv, -1);
    printParamsNums(luaEnv);
    std::cout << "bg.r :" << red << std::endl;
    printParamsNums(luaEnv);

    cout << "set top 1" << endl;
    lua_settop(luaEnv, 1);
    printParamsNums(luaEnv);


    //获取表的变量 bg[g]  老式操作
    cout << "push string g" << endl;
    lua_pushstring(luaEnv, "g");
    double green = lua_tonumber(luaEnv, -1);
    std::cout << "green:" << green << endl;//此时的 green 是0  ，因为是我们刚刚压入栈的
    printParamsNums(luaEnv);

    cout << "get table " << endl; // 
    lua_gettable(luaEnv, -2);
    green = lua_tonumber(luaEnv, -1);
    printParamsNums(luaEnv);


    //获取表的变量 bg[b]  新式操作
    cout << "get field " << endl;
    lua_getfield(luaEnv, -2, "b");//table 位于-2
    printParamsNums(luaEnv);// 栈增加了1
    double blue = lua_tonumber(luaEnv, -1);

    ////////////////////////////往Lua文件中添加table////////////////

    int count = 0;
    while (ct[count].name != NULL) {
        setColor(luaEnv, &ct[count++]);
    }

    //此时再从Lua环境获取刚刚添加的 Color
    lua_getglobal(luaEnv, "WHITE");
    lua_pushstring(luaEnv, "r");//压入 key “r”
    lua_gettable(luaEnv, -2);//获取 bg[r]
    cout <<"the table we created which named WHITE `s property  r:" <<lua_tonumber(luaEnv, -1) << endl;

    lua_settop(luaEnv, 0);//清空栈
    //////////////////////从C中调用Lua中的函数 f(x,y)
    lua_getglobal(luaEnv, "f");//拿到函数指针
    lua_pushnumber(luaEnv, 1.0);//压入x
    lua_pushnumber(luaEnv, 4);//y
    int ret = lua_pcall(luaEnv, 2, 2, 0);//2 parameters，1 return value
    if (ret != 0) {
        cout << "函数调用出错！" << endl;
    }
    cout << "现在栈的深度：" << lua_gettop(luaEnv) << endl;
    double result1 = lua_tonumber(luaEnv, -2); // get first return value
    double result = lua_tonumber(luaEnv, -1); // get second return value

    cout << "函数f调用结果为：" << result1 << "和" << result << endl;
    system("pause");
}