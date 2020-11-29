/*******************************************************************
 *	文件名称: dcq_get_info.h
 *	简要描述: domain company query 获取查询域名信息
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 根据配置文件传入的关键字输出网站相关信息
 ******************************************************************/
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "dcq_conf.h"

using namespace std;

#ifndef __DCQ_GET_INFO_H__
#define __DCQ_GET_INFO_H__

typedef map<string, string> domain_info;

/*******************************************************************
 *	函数名称: get_domain_info
 *	简要描述: 检测domain域名是否合法, 并将结果输入domains中
 *	输入参数: dmain_url 域名网址
 *	输出参数: info 域名相关备案信息
 *	输入参数: out_conf 配置信息
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
extern int get_domain_info(string dmain_url, domain_info &info, struct conf_context *out_conf);
#endif