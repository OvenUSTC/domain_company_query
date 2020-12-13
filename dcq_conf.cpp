/*******************************************************************
 *	文件名称: dcq_conf.cpp
 *	简要描述: domain company query 配置解析
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: json 配置文件解析相关逻辑
 ******************************************************************/

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "dcq_conf.h"
#include "dcq_log.h"

using namespace std;

static int json_file_load(string config_file, struct conf_context *conf)
{
    Json::Reader reader;
    Json::Value root;
    int i, j;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " config_file:" << config_file
                  << " conf:" << conf;

    if (conf == NULL)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " conf:" << conf;
        return -EINVAL;
    }

    ifstream in(config_file, ios::binary);
    if (!in.is_open())
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " can not open file:" << config_file;
        return -EEXIST;
    }

    if (!reader.parse(in, root))
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " " << config_file
                       << " file format is not json.";
        in.close();
        return -EFAULT;
    }

    if (root["global"].isNull() || root["icp"].isNull())
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " " << config_file
                       << " file must have global and icp configure.";
        in.close();
        return -EINVAL;
    }

    if (!root["icp"].isArray())
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " " << config_file
                       << " icp is not a array";
        in.close();
        return -EINVAL;
    }

    Json::Value global = root["global"];
    Json::Value icp = root["icp"];
    /* 解析 global 配置 */
    if (!global["db"].isNull() && global["db"].isString())
    {
        conf->global.db = global["db"].asString();
        DCQ_LOG(INFO) << __FUNCTION__
                      << " set global db:" << conf->global.db;
    }

    if (!global["debug_level"].isNull() && global["debug_level"].isInt())
    {
        conf->global.debug_level = global["debug_level"].asInt();
        DCQ_LOG(INFO) << __FUNCTION__
                      << " set global debug_level:" << conf->global.debug_level;
    }

    if (!global["time_out"].isNull() && global["time_out"].isInt())
    {
        conf->global.time_out = global["time_out"].asInt();
        DCQ_LOG(INFO) << __FUNCTION__
                      << " set global time_out:" << conf->global.time_out;
    }

    if (!global["key"].isNull() && global["key"].isArray() &&
        global["key"].size() > 0 && global["key"][0].isString())
    {
        for (j = 0; j < (int)global["key"].size(); j++)
        {
            conf->global.key.push_back(global["key"][j].asString());
            DCQ_LOG(INFO) << __FUNCTION__
                          << "global key:" << global["key"][j].asString();
        }
    }
    else
    {
        conf->global.key.push_back(DEFAULT_KEY);
        DCQ_LOG(INFO) << __FUNCTION__
                      << "global key:" << DEFAULT_KEY;
    }

    /* 解析 icp 配置 */
    for (i = 0; i < (int)icp.size(); i++)
    {
        Json::Value api = icp[i];
        struct icp_context icp_ctx;

        if (!api["curl"].isNull() && api["curl"].isString())
        {
            icp_ctx.curl = api["curl"].asString();
            DCQ_LOG(INFO) << __FUNCTION__
                          << "icp[" << i
                          << "] curl:" << icp_ctx.curl;
        }
        else
        {
            DCQ_LOG(INFO) << __FUNCTION__
                          << "icp[" << i
                          << "] curl is wrong skip.";
            continue;
        }

        if (!api["type"].isNull() && api["type"].isString())
        {
            if (api["type"] == "POST" || api["type"] == "GET")
            {
                icp_ctx.type = api["type"].asString();
                DCQ_LOG(INFO) << __FUNCTION__
                              << "icp[" << i
                              << "] type:" << icp_ctx.type;
            }
            else
            {
                DCQ_LOG(WARNING) << __FUNCTION__
                                 << "icp[" << i
                                 << "] type:" << api["type"]
                                 << " not POST or GET "
                                 << " set defalut POST";
            }
        }

        conf->icp.push_back(icp_ctx);
    }

    in.close();

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << 0;
    return 0;
}

int dcq_config_load(string config_file, struct conf_context **out_conf)
{
    struct conf_context *conf = nullptr;
    int ret = 0;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " config_file:" << config_file
                  << " out_conf:" << out_conf;

    if (out_conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << "exit"
                       << " out_conf:" << out_conf
                       << endl;
        return -EINVAL;
    }

    *out_conf = nullptr;

    conf = new (std::nothrow)(struct conf_context);
    if (conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << "exit"
                       << " alloc config memery failed"
                       << endl;
        return -ENOMEM;
    }

    ret = json_file_load(config_file, conf);
    if (ret != 0)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << "exit"
                       << "Failed to load configuration from " << config_file
                       << "ret:" << ret;
        delete conf;
        conf = NULL;
        *out_conf = NULL;
        return ret;
    }

    *out_conf = conf;

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << ret;
    return 0;
}

void dcq_config_free(struct conf_context *conf)
{
    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " out_conf:" << conf;

    if (conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " conf" << conf;
        return;
    }

    DCQ_LOG(INFO) << __FUNCTION__ << " exit";
    delete conf;
}