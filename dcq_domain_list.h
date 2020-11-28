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

extern int dcq_read_file_list(const char *file_name, vector<string> &domains);
extern int dcq_read_params_list(const char *domain, vector<string> &domains);

#endif
