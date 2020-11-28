#include <iostream>
#include <string>
#include "glog/logging.h"

using namespace google;

#ifndef __DCQ_LOG_H__
#define __DCQ_LOG_H__

#define DEFAULT_MAX_LOG_SIZE (100)
#define DEFAULT_SERVER_NAME  "dcq"
#define DEFAULT_LOG_FILE "/var/log"

#define DCQ_LOG(level) LOG(level)

extern int dcq_log_init(const char *log_dir);
extern int dcq_log_release();

#endif

