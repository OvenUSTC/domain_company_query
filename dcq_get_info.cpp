/*******************************************************************
 *	文件名称: dcq_get_info.cpp
 *	简要描述: domain company query 获取查询域名信息
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 根据配置文件传入的关键字输出网站相关信息
 ******************************************************************/
#include <iostream>
#include <string>
#include <map>
#include <jsoncpp/json/json.h>
#include <time.h>
#include "dcq_log.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "request.h"

using namespace std;

static string &replace_all(string &str, const string &old_value, const string &new_value)
{
    while (true)
    {
        string::size_type pos(0);
        if ((pos = str.find(old_value)) != string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

static int get_respose_info(string &response, const string &key, domain_info &info)
{
    Json::Reader reader;
	Json::Value root;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " response:" << response
                  << " key:" << key;

    if (response.size() == 0 || key.size() == 0)
    {
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                         << " response:" << response
                         << " key:" << key
                         << " ret:" << -EINVAL;
        return -EINVAL;
    }

	if (!reader.parse(response, root))
	{
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                         << " response is not json " << response
                         << " ret:" << -ENONET;
        return -ENONET;
	}
	
    Json::Value val = root[key];
    if (val.isNull() || !val.isString())
    {
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                         << " response does not exist " << key
                         << " ret:" << -ENOENT;
        return -ENOENT;
    }

    info[key] = val.asString();

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << 0;
    return 0;
}

int get_domain_info(string dmain_url, domain_info &info, struct conf_context *out_conf)
{
    int i, j;
    int api_num = out_conf->icp.size();
    int status_code;
    string body;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                     << " dmain_url:" << dmain_url
                     << " out_conf:" << out_conf;

    /* TODO 搜索本地 tdb 数据库 */

    /* 搜索远程 API 域名备案信息，优先最后一个 API */
    for (i = 0; i < api_num; i++)
    {
        string api_url = out_conf->icp[i].curl;
        string response;
        replace_all(api_url, "*", dmain_url);

        if (out_conf->icp[i].type == "POST")
        {
            status_code = posts(api_url, body, &response);
        }
        else
        {
            status_code = get(api_url, &response);
        }
        
		if (status_code != CURLcode::CURLE_OK)
		{
            DCQ_LOG(WARNING) << __FUNCTION__
                             << " request " << api_url
                             << " status:" << status_code
                             << " skip current request";
			continue;
		}
        
        int key_num = out_conf->icp[i].key.size();
        for(j=0; j<key_num; j++) {
            get_respose_info(response, out_conf->icp[i].key[j], info);
        }

        /* TODO 备案选择算法 */
    }

    /* TODO 更新本地 tdb 数据库 */


    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << 0;
    return 0;
}