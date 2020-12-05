#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <regex>
#include <tdb.h>
#include <fcntl.h>
#include <talloc.h>
#include <jsoncpp/json/json.h>
#include "dcq_log.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "dcq_db.h"

using namespace std;

#define OPEN_DB_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static int dcq_info2josn(domain_info &info, string &json)
{
    Json::Value jobject;

    for(auto iter = info.begin(); iter != info.end(); iter++)
    {
        jobject[iter->first] = iter->second;
    }

    json = jobject.toStyledString();

    return 0;
}

static int dcq_josn2info(string &json, domain_info &info)
{
    Json::Reader reader;
    Json::Value value;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " data" << &info;

    if (json.length() <= 0)
    {
        DCQ_LOG(WARNING) << __FUNCTION__
                         << " domain info not found data";
        return -ENODATA;
    }

    if (!reader.parse(json, value))
    {
        DCQ_LOG(WARNING) << __FUNCTION__
                         << " domain info not found data";
        return -ENODATA;
    }

    Json::Value::Members members = value.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); it++)
    {
        if (value[*it].type() != Json::stringValue)
        {
            DCQ_LOG(ERROR) << __FUNCTION__
                           << " Local database corruption"
                           << " data type is not string";
            return -ENODATA;
        }

        info.insert(pair<string, string>(*it, value[*it].asString()));
    }

    return 0;
}

dcq_db_context *dcq_db_open(const conf_context *conf)
{
    dcq_db_context *fd = nullptr;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " conf:" << conf;

    if (conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " conf:" << conf;
        return nullptr;
    }

    fd = tdb_open(conf->global.db.c_str(), 0, 0, O_RDWR | O_CREAT, OPEN_DB_MODE);
    if (fd == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " can not open " << conf->global.db.c_str();
        return nullptr;
    }

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " fd:" << fd;
    return fd;
}

int dcq_db_insert(dcq_db_context *fd, string domain_name, domain_info &info, int flag)
{
    int ret = 0;
    dcq_db_data data;
    dcq_db_key key;
    string json;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " fd:" << fd
                  << " domain_name" << domain_name
                  << " data" << &info;

    if (fd == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " fd:NULL ";
        return -EINVAL;
    }

    ret = dcq_info2josn(info, json);
    if (ret < 0)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " ret: " << ret;
        return ret;
    }

    key.dptr = (unsigned char *)domain_name.c_str();
    key.dsize = domain_name.size();
    data.dptr = (unsigned char *)json.data();
    data.dsize = json.size() + 1;

    DCQ_LOG(INFO) << __FUNCTION__ << " json"
                  << " key:" << domain_name
                  << " data:" << data.dptr;

    ret = tdb_store(fd, key, data, flag);

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << ret;

    return ret;
}

int dcq_db_search(dcq_db_context *fd, string domain_name, domain_info &info)
{
    int ret = 0;
    dcq_db_data data;
    dcq_db_key key;
    std::stringstream ss;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " fd:" << fd
                  << " domain_name" << domain_name;

    if (fd == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " fd:NULL ";
        return 0;
    }

    key.dptr = (unsigned char *)domain_name.c_str();
    key.dsize = domain_name.size();

    DCQ_LOG(INFO) << __FUNCTION__ << " set data"
                  << " key:" << domain_name
                  << " data:" << data.dptr;

    if(!tdb_exists(fd, key))
    {
        return -ENOENT;
    }

    data = tdb_fetch(fd, key);
    if (data.dptr == NULL)
    {
        DCQ_LOG(INFO) << __FUNCTION__ << " search"
                  << " key:" << domain_name
                  << " not find.";
        return -ENOENT;
    }

    string json((char*)data.dptr, data.dsize);
    ret = dcq_josn2info(json, info);
    if (ret < 0)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " json to info failed"
                      << " key:" << domain_name;
        TALLOC_FREE(data.dptr);
        return ret;
    }

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << ret;
    TALLOC_FREE(data.dptr);
    return ret;
}

int dcq_db_close(dcq_db_context *fd)
{
    return tdb_close(fd);
}