#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "glog/logging.h"
#include "dcq_conf.h"
#include "dcq_log.h"

using namespace google;

int dcq_log_init(const char *log_dir)
{
    struct stat st;
    int ret;

    if (log_dir == nullptr) {
        FLAGS_log_dir = DEFAULT_LOG_FILE;
        cout << "Warn log dir is NULL, set log dir:" << DEFAULT_LOG_FILE << endl;
    }
    else {
        ret = stat(log_dir, &st);
        if (S_ISDIR(st.st_mode) || ret == 0)
        {
            FLAGS_log_dir = log_dir;
        }
        else
        {
            cout << "Warn " << log_dir 
                 << " is not a directory, set log dir:" << DEFAULT_LOG_FILE 
                 << endl;

            FLAGS_log_dir = DEFAULT_LOG_FILE;
        }
    }
        
    FLAGS_logbufsecs = 0;
    FLAGS_logtostderr = false;
    FLAGS_alsologtostderr = false;
    FLAGS_colorlogtostderr = false;
    FLAGS_max_log_size = DEFAULT_MAX_LOG_SIZE;
    FLAGS_stop_logging_if_full_disk = true;
    InitGoogleLogging(DEFAULT_SERVER_NAME);
    return 0;
}

int dcq_log_release()
{
    ShutdownGoogleLogging();
    return 0;
}