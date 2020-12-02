#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <regex>
#include <tdb.h>
#include <fcntl.h>
#include <sstream>
#include <talloc.h>
#include <cereal/archives/binary.hpp>
#include "dcq_log.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "dcq_db.h"

using namespace std;

#define OPEN_DB_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


dcq_db_context *dcq_db_open(const conf_context *conf)
{
    dcq_db_context *fd = nullptr;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " conf:" << conf;

    if (conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " conf:" << conf;
        return 0;
    }

    fd = tdb_open(conf->global.db.c_str(), 0, 0, O_RDWR, OPEN_DB_MODE);
    if (fd == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " can not open " << conf->global.db.c_str();
        return 0;
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
    std::stringstream ss;
    string sdata;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " fd:" << fd
                  << " domain_name" << domain_name
                  << " data" << &info;

    if (fd == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " fd:NULL ";
        return 0;
    }

    ss >> sdata;
    key.dptr = (unsigned char *)domain_name.c_str();
    key.dsize = domain_name.size();
    data.dptr = (unsigned char *)sdata.data();
    data.dsize = sdata.size();

    DCQ_LOG(INFO) << __FUNCTION__ << " set data"
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

    data = tdb_fetch(fd, key);

    if (data.dptr == NULL)
    {
        DCQ_LOG(INFO) << __FUNCTION__ << " search"
                  << " key:" << domain_name
                  << " not find.";
        return -ENOENT;
    }
    string sdata((char*)data.dptr, data.dsize);
    ss << sdata;
    cereal::BinaryInputArchive archive(ss);

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << ret;

    TALLOC_FREE(data.dptr);
    return ret;
}

int dcq_db_close(dcq_db_context *fd)
{
    return tdb_close(fd);
}