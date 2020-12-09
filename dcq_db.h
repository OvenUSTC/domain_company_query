#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <regex>
#include <tdb.h>
#include "dcq_conf.h"
#include "dcq_get_info.h"

using namespace std;

#ifndef __DCQ_DB_H__
#define __DCQ_DB_H__

#define DCQ_DB_INSERT TDB_INSERT
#define DCQ_DB_MODIFY TDB_MODIFY

typedef TDB_CONTEXT dcq_db_context;
typedef TDB_DATA dcq_db_key;
typedef TDB_DATA dcq_db_data;

extern dcq_db_context *dcq_db_open(const struct conf_context *conf);
extern int dcq_db_insert(dcq_db_context *fd, string domain_name, domain_info &info, int flag);
extern int dcq_db_search(dcq_db_context *fd, string domain_name, domain_info &info);
extern int dcq_db_close(dcq_db_context *fd);

#endif
