/*******************************************************************
 *	文件名称: dcq_conf.h
 *	简要描述: domain company query 配置解析
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: json 配置文件解析相关逻辑
 ******************************************************************/
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;

#ifndef __DCQ_CONF_H__
#define __DCQ_CONF_H__

#define DEFAULT_DB "/home/domain.db"
#define DEFAULT_DEBUG_LEVEL (0)
#define DEFAULT_TIME_OUT (60)
#define DEFAULT_KEY "主办单位名称"

struct global_context
{
    string db = DEFAULT_DB;
    int debug_level = DEFAULT_DEBUG_LEVEL;
    int time_out = DEFAULT_DEBUG_LEVEL;
};

struct icp_context
{
    string curl = "";
    string type = "";
    vector<string> key;
};

struct conf_context
{
    struct global_context global;
    vector<struct icp_context> icp;
};

/*******************************************************************
 *	函数名称: dcq_config_load
 *	简要描述: 将 config_file 指向的配置信息加载到 out_conf 中
 *	输入参数: config_file 配置文件路径
 *	输出参数: out_conf 输出的配置信息
 *	返回值:   errno  0 成功, <0 打开文件失败或格式不匹配
 ******************************************************************/
extern int dcq_config_load(string config_file, struct conf_context **out_conf);

/*******************************************************************
 *	函数名称: dcq_config_load
 *	简要描述: 释放记载的配置信息
 *	输入参数: conf 配置信息
 *	返回值:   无
 ******************************************************************/
extern void dcq_config_free(struct conf_context *conf);

#endif