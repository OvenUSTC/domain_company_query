/*******************************************************************
 *	文件名称: dcq_log.cpp
 *	简要描述: domain company query 日志模块
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 日志和屏幕打印模块
 ******************************************************************/
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "glog/logging.h"
#include "dcq_conf.h"
#include "dcq_log.h"

using namespace google;

int global_log_level = google::INFO;

int dcq_log_init(const char *log_dir, int log_level)
{
    struct stat st;
    int ret;

    if (log_dir == nullptr) {
        FLAGS_log_dir = DEFAULT_LOG_FILE;
        DCQ_COUT << "Warn log dir is NULL, set log dir:" << DEFAULT_LOG_FILE << DCQ_ENDL;
    }
    else 
    {
        ret = stat(log_dir, &st);
        if (S_ISDIR(st.st_mode) || ret == 0)
        {
            FLAGS_log_dir = log_dir;
        }
        else
        {
            DCQ_COUT << "Warn " << log_dir 
                     << " is not a directory, set log dir:" << DEFAULT_LOG_FILE 
                     << DCQ_ENDL;

            FLAGS_log_dir = DEFAULT_LOG_FILE;
        }
    }
    global_log_level = log_level;
    FLAGS_logbufsecs = 0;
    FLAGS_logtostderr = false;
    FLAGS_alsologtostderr = false;
    FLAGS_colorlogtostderr = false;
    FLAGS_max_log_size = DEFAULT_MAX_LOG_SIZE;
    FLAGS_stop_logging_if_full_disk = true;
    InitGoogleLogging(DEFAULT_SERVER_NAME);
    return 0;
}

int dcq_set_log_level(int log_level)
{
    if(log_level < 0 || log_level > google::FATAL)
    {
        DCQ_COUT << __FUNCTION__ 
                 << " log_level:" << log_level
                 << " is invalid parameter" << DCQ_ENDL;
        return -EINVAL;
    }

    global_log_level = log_level;
    return 0;
}

int dcq_log_release()
{
    ShutdownGoogleLogging();
    return 0;
}