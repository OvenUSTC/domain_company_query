#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;

#ifndef __DCQ_CONF_H__
#define __DCQ_CONF_H__

#define DEFAULT_LOG_FILE "/var/log/dcq.log"
#define DEFAULT_DB "/home/domain.db"
#define DEFAULT_DEBUG_LEVEL (0)
#define DEFAULT_TIME_OUT (60)
#define DEFAULT_KEY "主办单位名称"

struct global_context {
    string log_file = DEFAULT_LOG_FILE;
    string db = DEFAULT_DB;
    int debug_level = DEFAULT_DEBUG_LEVEL;
    int time_out = DEFAULT_DEBUG_LEVEL;
};

struct icp_context {
    string curl = "";
    string type = "";
    vector<string> key; 
};

struct conf_context {
    struct global_context global;
    vector<struct icp_context> icp;
};

extern int dcq_config_load(string config_file, struct conf_context **out_conf);
extern void dcq_config_free(struct conf_context *out_conf);

#endif