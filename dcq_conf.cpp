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

    if (conf == NULL)
    {
        cout << "Error conf is NULL"
             << endl;
        return -EINVAL;
    }

    ifstream in(config_file, ios::binary);
    if (!in.is_open())
    {
        cout << "Error opening file"
             << config_file
             << endl;
        return -EEXIST;
    }

    if (!reader.parse(in, root))
    {
        cout << "Error parse error"
             << endl;
        in.close();
        return -EFAULT;
    }

    if (root["global"].isNull() || root["icp"].isNull())
    {
        cout << "Error conf " << config_file
             << " can not find global or icp."
             << endl;
        in.close();
        return -EINVAL;
    }

    if (!root["icp"].isArray())
    {
        cout << "Error conf " << config_file
             << " icp must a array."
             << endl;
        in.close();
        return -EINVAL;
    }

    Json::Value global = root["global"];
    Json::Value icp = root["icp"];
    /* 解析 global 配置 */
    if (!global["db"].isNull() && global["db"].isString())
    {
        conf->global.db = global["db"].asString();
    }

    if (!global["debug_level"].isNull() && global["debug_level"].isInt())
    {
        conf->global.debug_level = global["debug_level"].asInt();
    }

    if (!global["time_out"].isNull() && global["time_out"].isInt())
    {
        conf->global.time_out = global["time_out"].asInt();
    }

    /* 解析 icp 配置 */
    for(i=0; i<icp.size(); i++)
    {
        Json::Value api = icp[i];
        struct icp_context icp_ctx;

        if (!api["curl"].isNull() && api["curl"].isString())
        {
            icp_ctx.curl = api["curl"].asString();
        }
        else
        {
            continue;
        }

        if (!api["type"].isNull() && api["type"].isString())
        {
            if (api["type"] == "POST" || api["type"] == "GET")
            {
                icp_ctx.type = api["type"].asString();
            }
            else
            {
                cout << "WARN icp[" << i
                     << "].type = " << api["type"] 
                     << " not POST or GET "
                     << " set defalut POST" 
                     << endl;
            }
        }

        if (!api["key"].isNull() && api["key"].isArray() &&
            api["key"].size() > 0 && api["key"][0].isString())
        {
            for(j=0; j<api["key"].size(); j++)
            {
                icp_ctx.key.push_back(api["key"][j].asString());
            }
        }
        else
        {
            icp_ctx.key.push_back(DEFAULT_KEY);
            continue;
        }

        conf->icp.push_back(icp_ctx);
    }
    
    in.close();
    return 0;
}

int dcq_config_load(string config_file, struct conf_context **out_conf)
{
    struct conf_context *conf = nullptr;
    int ret = 0;

    *out_conf = nullptr;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " config_file:" << config_file
                  << " out_conf" << out_conf;

    conf = new (std::nothrow)(struct conf_context);
    if (conf == nullptr)
    {
        cout << "Error alloc config memery" << endl;
        return -ENOMEM;
    }

    ret = json_file_load(config_file, conf);
    if (ret != 0)
    {
        cout << "Error loal conf form file " << config_file
             << " error:" << ret
             << endl;
        delete conf;
        conf = NULL;
        *out_conf = NULL;
        return ret;
    }

    *out_conf = conf;

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << "ret:" << ret;
    return 0;
}

void dcq_config_free(struct conf_context *out_conf)
{
    if (out_conf != nullptr)
        delete out_conf;
}