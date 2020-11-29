/*******************************************************************
 *	文件名称: request.h
 *	简要描述: curl POST GET 请求模块
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: curl 请求打印模块
 ******************************************************************/
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>

using namespace std; 

#ifndef __REQUEST_H__
#define __REQUEST_H__

/*******************************************************************
 *	函数名称: posts
 *	简要描述: http POST 请求
 *	输入参数: url
 *  输入参数: POST 请求body
 *  输出参数: response 请求应答数据 
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
int posts(string url, string &body,  string* response);

/*******************************************************************
 *	函数名称: get
 *	简要描述: http GET 请求
 *	输入参数: url
 *  输出参数: response 请求应答数据 
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
int get(string url, string* response);

#endif