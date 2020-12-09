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
#include <sstream>
#include <iomanip>
#include <chrono>
#include "dcq_log.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "request.h"

using namespace std;
using namespace std::chrono;

#define DCQ_INSERT_DATE "collect_time"
#define SEC_PER_HOUR (3600)

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

static int dcq_date2str(string &str, time_point<system_clock, nanoseconds> &time_point)
{
    time_t time_p = system_clock::to_time_t(time_point);
    std::tm tm = *std::localtime(&time_p);
    auto time = std::put_time(&tm, "%b %d %Y %H:%M:%S");
    std::stringstream ss;
    ss << time;
    str = ss.str();
	return 0;
}

static int dcq_is_time_out(const string &str,
                           struct conf_context *out_conf)
{
    std::tm tm;
    std::stringstream ss;
    auto time_cur = system_clock::now();

    ss << str;
    ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
    time_point<system_clock, nanoseconds> time_point = system_clock::from_time_t(std::mktime(&tm));
    duration<double> diff = time_cur - time_point;

    if(out_conf->global.time_out < diff.count() / SEC_PER_HOUR)
    {
        DCQ_LOG(INFO) << __FUNCTION__
                      << " db date:" << str
                      << " is time out";
        return 1;
    }

    return 0;
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

static int need_search_api(struct conf_context *out_conf, domain_info &db_info, domain_info &out_info)
{
    int i, key_num = 0, ret = 0;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                  << " out_conf:" << out_conf
                  << " db_info:" << &db_info;

    if(out_conf == nullptr)
    {
        DCQ_LOG(ERROR) << __FUNCTION__ << " exit"
                       << " out_conf:" << out_conf
                       << " ret:" << -EINVAL;
        return -EINVAL;
    }

    if (db_info.count(DCQ_INSERT_DATE) == 0)
    {
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                       << " db_info:" << &db_info
                       << " can not find " << DCQ_INSERT_DATE
                       << " ret:" << -EINVAL;
        return -EINVAL;
    }

    if (dcq_is_time_out(db_info[DCQ_INSERT_DATE], out_conf))
    {
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                       << " db_info:" << &db_info
                       << " time out ret:" << -EINVAL;
        return -EINVAL;
    }

    key_num = out_conf->global.key.size();
    for(i=0; i<key_num; i++)
    {
        if(db_info.count(out_conf->global.key[i]) == 0)
            break;

        out_info[out_conf->global.key[i]] = db_info[out_conf->global.key[i]];
    }

    if (i == key_num)
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << ret;
    return ret;
}

int get_domain_info(string dmain_url, domain_info &info, struct conf_context *out_conf)
{
    int i, j;
    int api_num = out_conf->icp.size();
    int flags = DCQ_DB_INSERT;
    int status_code;
    string body;
    domain_info db_info;
    auto now = system_clock::now();
    string date_str;

    DCQ_LOG(INFO) << __FUNCTION__ << " enter"
                     << " dmain_url:" << dmain_url
                     << " out_conf:" << out_conf;

    /* 搜索本地 tdb 数据库 */
    status_code = dcq_db_search(out_conf->db_handle, dmain_url, db_info);
    if (status_code == 0 &&
        !need_search_api(out_conf, db_info, info))
    {
        /* 本地数据库中保存来所有想要的数据，不需要搜索互联网 */
        return 0;
    }

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
        else if (out_conf->icp[i].type == "GET")
        {
            status_code = get(api_url, &response);
        }
        else 
        {
            DCQ_LOG(WARNING) << __FUNCTION__
                             << " request " << api_url
                             << " type:" << out_conf->icp[i].type
                             << " error skip current request";
            continue;
        }
        
		if (status_code != CURLcode::CURLE_OK)
		{
            DCQ_LOG(WARNING) << __FUNCTION__
                             << " request " << api_url
                             << " status:" << status_code
                             << " skip current request";
			continue;
		}
        
        int key_num = out_conf->global.key.size();
        for(j=0; j<key_num; j++) {
            get_respose_info(response, out_conf->global.key[j], info);
        }

        if (info.size() >= key_num)
            break;
    }

    if (info.size() == 0)
    {
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                         << " Can not get domain " << dmain_url
                         << " info ret:" << 0;
        return 0;
    }

    if (db_info.size() > 0)
    {
        flags = DCQ_DB_MODIFY;
    }
    else
    {
        flags = DCQ_DB_INSERT;
    }

    dcq_date2str(date_str, now);

    db_info = info;
    db_info[DCQ_INSERT_DATE] = date_str;
    /* 更新本地 tdb 数据库 */
    status_code = dcq_db_insert(out_conf->db_handle, dmain_url, db_info, flags);
    if (status_code < 0)
    {
        /* 写入数据库错误 */
        DCQ_LOG(WARNING) << __FUNCTION__ << " exit"
                         << " domain " << dmain_url
                         << " can not insert local db"
                         << " status:" << status_code;
    }

    DCQ_LOG(INFO) << __FUNCTION__ << " exit"
                  << " ret:" << 0;
    return 0;
}