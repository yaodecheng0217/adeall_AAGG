/*
 * @Author: Yaodecheng
 * @Date: 2020-04-09 14:29:59
 * @LastEditTime: 2020-04-09 16:08:44
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
#include "readjson.h"
Readconfig::Readconfig(const char *dir)
{
    std::ifstream infile;
    infile.open(dir,std::ios::in); //将文件流对象与文件连接起来
    if (infile.is_open())
    {

        char c;
        infile >> std::noskipws;
        std::stringstream ss;
        while (!infile.eof())
        {
            infile >> c;
            ss << c;
        }

        config = neb::CJsonObject(ss.str());
    }
    infile.close(); //关闭文件输入流
    if (config.IsEmpty())
    {
        JSON_INFO("Can not loding Init file :  %s", dir);
        getchar();
        exit(0);
    }
    else
    {
        JSON_INFO("Init is ok!");
    }
}
Readconfig::~Readconfig()
{
}
void Readconfig::printfinitinfo()
{
    printf("init======>\n%s\n=============\n", config.ToFormattedString().c_str());
}
void Readconfig::setitem(std::string item)
{
    itemname = item;
}
void Readconfig::GetValue(const char *name, double &value)
{
    if (config[itemname].Get(name, value))
        JSON_INFO("from %s set: %s ==> %f", itemname.c_str(), name, value);
}
void Readconfig::GetValue(const char *name, bool &value)
{
    if (config[itemname].Get(name, value))
        JSON_INFO("from %s set: %s ==> %d", itemname.c_str(), name, value);
}
void Readconfig::GetValue(const char *name, int &value)
{
    if (config[itemname].Get(name, value))
        JSON_INFO("from %s set: %s ==> %d", itemname.c_str(), name, value);
}
void Readconfig::GetValue(const char *name, std::string &value)
{
    if (config[itemname].Get(name, value))
        JSON_INFO("from %s set: %s ==> %s", itemname.c_str(), name, value.c_str());
}

ReadSeting::ReadSeting(const char *dir)
{
    _dir=dir;
    std::ifstream infile;
    infile.open(dir,std::ios::in); //将文件流对象与文件连接起来
    if (infile.is_open())
    {

        char c;
        infile >> std::noskipws;
        std::stringstream ss;
        while (!infile.eof())
        {
            infile >> c;
            ss << c;
        }

        config = neb::CJsonObject(ss.str());
    }
    infile.close(); //关闭文件输入流
    if (config.IsEmpty())
    {
        if (print)
            JSON_INFO("Can not loding Init file : %s", dir);
        getchar();
        exit(0);
    }
    else
    {
        if (print)
            JSON_INFO("Init is ok!");
    }
}
ReadSeting::~ReadSeting()
{
}
void ReadSeting::printfinitinfo()
{
    printf("init======>\n%s\n=============\n", config.ToFormattedString().c_str());
}
void ReadSeting::reload()
{
    std::ifstream infile;
    infile.open(_dir.c_str(),std::ios::in); //将文件流对象与文件连接起来
    if (infile.is_open())
    {
        char c;
        infile >> std::noskipws;
        std::stringstream ss;
        while (!infile.eof())
        {
            infile >> c;
            ss << c;
        }
        config = neb::CJsonObject(ss.str());
        if (print)
            JSON_INFO("reload ok");
    }
    else
    {
       if (print)
            JSON_INFO("reload error");
    }
    
    infile.close(); //关闭文件输入流
}
void ReadSeting::GetValue(const char *name, double &value)
{
    if (config.Get(name, value))
        if (print)
            JSON_INFO("set: %s ==> %f", name, value);
}
void ReadSeting::GetValue(const char *name, float &value)
{
    if (config.Get(name, value))
        if (print)
            JSON_INFO("set: %s ==> %f", name, value);
}
void ReadSeting::GetValue(const char *name, bool &value)
{
    if (config.Get(name, value))
        if (print)
            JSON_INFO("set: %s ==> %d", name, value);
}
void ReadSeting::GetValue(const char *name, int &value)
{
    if (config.Get(name, value))
        if (print)
            JSON_INFO("set: %s ==> %d", name, value);
}
void ReadSeting::GetValue(const char *name, std::string &value)
{
    if (config.Get(name, value))
        if (print)
            JSON_INFO("set: %s ==> %s", name, value);
}
void ReadSeting::setprint(bool en)
{
   print=en;
}