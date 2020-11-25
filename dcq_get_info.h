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

extern int get_domain_info(string dmain_url, domain_info &info, struct conf_context *out_conf);
#endif