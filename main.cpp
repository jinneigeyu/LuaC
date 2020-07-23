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

//����һ��table ѹ�� lua������ȥ
void setColor(lua_State* l, ColorTable* ct) 
{
    cout << "before create a new table : stack params capicity:" << lua_gettop(l) << endl;
    lua_newtable(l);
    cout << "afetr a table create :" << lua_gettop(l) << endl;

    cout << "push number red" << endl;
    lua_pushnumber(l, ct->red);
    printParamsNums(l);
    cout << "set field" << endl;
    lua_setfield(l, -2, "r"); // ��ʱ�½���table ��������-2  set ֮�� ջ������ ct-red ��ջ������table��
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
    lua_setglobal(l, ct->name);//����talbe�����޸ı������  ****���ע��****
    printParamsNums(l);
}




//����lua�ļ�
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
    lua_settop(l, 0);//���ջ
    printParamsNums(l);
}


// ��Զ��ס ����ջ �ǻ���ջ��������
void main() {
    lua_State* luaEnv = luaL_newstate();
    printParamsNums(luaEnv);
    luaL_openlibs(luaEnv);
    int w = 0;
    int h = 0;
    load(luaEnv, "test.lua", &w, &h);
    ////////////////////////////��Lua�ļ��л�ȡ

    std::cout << "get global table bg" << std::endl;
    lua_getglobal(luaEnv, "bg");//��ȡ bg {},
    printParamsNums(luaEnv);

    std::cout << "push string r " << std::endl;
    lua_pushstring(luaEnv, "r");//ѹ�� key ��r��
    printParamsNums(luaEnv);

    std::cout << "get table  " << std::endl;
    lua_gettable(luaEnv, -2);//��ȡ bg[r]
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


    //��ȡ��ı��� bg[g]  ��ʽ����
    cout << "push string g" << endl;
    lua_pushstring(luaEnv, "g");
    double green = lua_tonumber(luaEnv, -1);
    std::cout << "green:" << green << endl;//��ʱ�� green ��0  ����Ϊ�����Ǹո�ѹ��ջ��
    printParamsNums(luaEnv);

    cout << "get table " << endl; // 
    lua_gettable(luaEnv, -2);
    green = lua_tonumber(luaEnv, -1);
    printParamsNums(luaEnv);


    //��ȡ��ı��� bg[b]  ��ʽ����
    cout << "get field " << endl;
    lua_getfield(luaEnv, -2, "b");//table λ��-2
    printParamsNums(luaEnv);// ջ������1
    double blue = lua_tonumber(luaEnv, -1);

    ////////////////////////////��Lua�ļ������table////////////////

    int count = 0;
    while (ct[count].name != NULL) {
        setColor(luaEnv, &ct[count++]);
    }

    //��ʱ�ٴ�Lua������ȡ�ո���ӵ� Color
    lua_getglobal(luaEnv, "WHITE");
    lua_pushstring(luaEnv, "r");//ѹ�� key ��r��
    lua_gettable(luaEnv, -2);//��ȡ bg[r]
    cout <<"the table we created which named WHITE `s property  r:" <<lua_tonumber(luaEnv, -1) << endl;

    lua_settop(luaEnv, 0);//���ջ
    //////////////////////��C�е���Lua�еĺ��� f(x,y)
    lua_getglobal(luaEnv, "f");//�õ�����ָ��
    lua_pushnumber(luaEnv, 1.0);//ѹ��x
    lua_pushnumber(luaEnv, 4);//y
    int ret = lua_pcall(luaEnv, 2, 2, 0);//2 parameters��1 return value
    if (ret != 0) {
        cout << "�������ó���" << endl;
    }
    cout << "����ջ����ȣ�" << lua_gettop(luaEnv) << endl;
    double result1 = lua_tonumber(luaEnv, -2); // get first return value
    double result = lua_tonumber(luaEnv, -1); // get second return value

    cout << "����f���ý��Ϊ��" << result1 << "��" << result << endl;
    system("pause");
}