/*******************************************************************
 *	文件名称: libdcq.h
 *	简要描述: domain company query 库模块
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/13
 *	说明: dcq lib 库
 ******************************************************************/

#ifndef __LIB_DCQ_H__
#define __LIB_DCQ_H__
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <future>

using namespace std;

typedef map<string, map<string, string> > domains_result;
typedef map<string, string> domain_info;
typedef int (*call_back_fn) (string &domain, domain_info &info, void* data);

class dcq
{
private:
    struct conf_context *conf;
    vector<string> domains;
    mutex lock;
    vector<thread> th;

public:
/*******************************************************************
 *	函数名称: dcq
 *	简要描述: class dcq 构造函数
 *	输入参数: conf_file  配置文件路径
 *	输入参数: log_file   日志文件路径
 *	输出参数: 无
 *	返回值:   无
 ******************************************************************/
    dcq(char *conf_file, char *log_file);

/*******************************************************************
 *	函数名称: ~dcq
 *	简要描述: class dcq 析构函数
 *	输入参数: 无
 *	输出参数: 无
 *	返回值:   无
 ******************************************************************/
    ~dcq();

/*******************************************************************
 *	函数名称: query_domain_prepare
 *	简要描述: 域名批量获取准备函数
 *	输入参数: domain 输入到cache 准备获取的域名
 *	输出参数: 无
 *	返回值:   errno 正确:0 错误:<0
 ******************************************************************/
    int query_domain_prepare(const char* domain);

/*******************************************************************
 *	函数名称: query_file_prepare
 *	简要描述: 域名批量获取准备函数
 *	输入参数: file_path 批量传入域名文件
 *	输出参数: 无
 *	返回值:   errno 正确:0 错误:<0
 ******************************************************************/
    int query_file_prepare(const char *file_path);

/*******************************************************************
 *	函数名称: cache_clear
 *	简要描述: 清除 domain cache
 *	输入参数: 无
 *	输出参数: 无
 *	返回值:   errno 正确:0 错误:<0
 ******************************************************************/
    int cache_clear();

/*******************************************************************
 *	函数名称: size
 *	简要描述: 获取 domain cache size
 *	输入参数: 无
 *	输出参数: 无
 *	返回值:   size
 ******************************************************************/
    size_t size();

/*******************************************************************
 *	函数名称: query_domain_sync
 *	简要描述: 同步获取 domain cache 里的所有域名信息
 *	输入参数: 无
 *	输出参数: res 获取 domain 信息
 *	返回值:   errno 正确:0 错误:<0
 ******************************************************************/
    int query_domain_sync(domains_result &res);

/*******************************************************************
 *	函数名称: query_domain_async
 *	简要描述: 异步获取 domain cache 里的所有域名信息
 *	输入参数: fn 异步回调函数, 每获取到一个域名信息便会调用
 *	输出参数: res 获取 domain 信息
 *	返回值:   async 结果
 ******************************************************************/
    future<int> query_domain_async(call_back_fn fn, domains_result &res);

/*******************************************************************
 *	函数名称: query_domain_wait
 *	简要描述: 异步获取 domain cache 里的所有域名信息
 *	输入参数: fn 异步回调函数, 每获取到一个域名信息便会调用
 *	输出参数: res 获取 domain 信息
 *	返回值:   errno 正确:0 错误:<0
 ******************************************************************/
    int query_domain_wait(future<int> ret);
};


#endif