/*******************************************************************
 *	文件名称: dcq_log.h
 *	简要描述: domain company query 日志模块
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 日志和屏幕打印模块
 ******************************************************************/
#include <iostream>
#include <string>
#include "glog/logging.h"

using namespace google;

#ifndef __DCQ_LOG_H__
#define __DCQ_LOG_H__

#define DEFAULT_MAX_LOG_SIZE (100)
#define DEFAULT_SERVER_NAME  "dcq"
#define DEFAULT_LOG_FILE "/var/log"
#define DCQ_COUT std::cout
#define DCQ_ENDL std::endl

extern int global_log_level;

#define DCQ_LOG(level) \
    (level) >= global_log_level ? 1:0 && LOG(level)

/*******************************************************************
 *	函数名称: dcq_log_init
 *	简要描述: 初始化日志模块
 *	输入参数: log_dir 日志目录
 *	输入参数: log_level 日志等级 默认值 0:INFO
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
extern int dcq_log_init(const char *log_dir, int log_level = 0);

/*******************************************************************
 *	函数名称: dcq_set_log_level
 *	简要描述: 动态设置日志等级
 *	输入参数: log_level 日志等级
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
extern int dcq_set_log_level(int log_level);

/*******************************************************************
 *	函数名称: dcq_log_release
 *	简要描述: 析构日志模块
 *	输入参数: 无
 *	返回值:   errno 0 成功, <0 错误码
 ******************************************************************/
extern int dcq_log_release();

#endif

