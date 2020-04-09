/*
 * @Author: Yaodecheng
 * @Date: 2020-04-09 11:33:18
 * @LastEditTime: 2020-04-09 14:55:53
 * @LastEditors: Yaodecheng
 * @Description: 加载配置项
 * @Adeall licence@2020
 */
#pragma once
#include "CJson/CJsonObject.hpp"
#include <fstream>
#include <sstream>

#define JSON_INFO(fmt, ...) {printf("[json]");printf(fmt "\n", ##__VA_ARGS__);}
class Readconfig
{
private:
    std::string itemname;

public:
    neb::CJsonObject config;
    void printfinitinfo();
    Readconfig(const char *dir);
    void setitem(std::string item);
    void GetValue(const char *name, double &value);
    void GetValue(const char *name, int &value);
    void GetValue(const char *name, bool &value);
    void GetValue(const char *name, std::string &value);
    ~Readconfig();
};
class ReadSeting
{
private:
bool print=false;
std::string _dir;
public:
    neb::CJsonObject config;
    void printfinitinfo();
    ReadSeting(const char *dir);
    void setprint(bool en);
    void reload();
    void GetValue(const char *name, double &value);
    void GetValue(const char *name, float &value);
    void GetValue(const char *name, int &value);
    void GetValue(const char *name, bool &value);
    void GetValue(const char *name, std::string &value);
    ~ReadSeting();
};
