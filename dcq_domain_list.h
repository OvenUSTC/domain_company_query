/*******************************************************************
 *	文件名称: dcq_domain_list.h
 *	简要描述: domain company query 批量域名文件加载
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 批量域名文件和输入参数，域名列表加载
 ******************************************************************/
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <unistd.h>
#include "request.h"
#include <jsoncpp/json/json.h>

using namespace std;

#ifndef __DCQ_DOMAIN_LIST_H__
#define __DCQ_DOMAIN_LIST_H__

#define DOMAIN_PATTERN "^(?=^.{3,255}$)[a-zA-Z0-9][-a-zA-Z0-9]{0,62}(\\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})+$"

/*******************************************************************
 *	函数名称: dcq_read_file_list
 *	简要描述: 读取域名list文件, 将结果输入domains中
 *	输入参数: file_name 文件名
 *	输出参数: domains 域名列表
 *	返回值:   errno
 ******************************************************************/
extern int dcq_read_file_list(const char *file_name, vector<string> &domains);

/*******************************************************************
 *	函数名称: dcq_read_params_list
 *	简要描述: 检测domain域名是否合法, 并将结果输入domains中
 *	输入参数: domain  单个域名
 *	输出参数: domains 域名列表
 *	返回值:   errno
 ******************************************************************/
extern int dcq_read_params_list(const char *domain, vector<string> &domains);

#endif
