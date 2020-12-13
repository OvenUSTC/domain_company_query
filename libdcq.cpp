#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <unistd.h>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include <thread>
#include <future>
#include <utility>
#include <thread>
#include <functional>
#include <memory>
#include <exception> 
#include <numeric>
#include <vector>
#include <cmath>
#include <string>
#include <mutex>
#include "request.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "dcq_domain_list.h"
#include "dcq_log.h"
#include "dcq_db.h"
#include "libdcq.h"

#define DOMAIN_CACHE_MAX_SIZE (100000)

using namespace std;

dcq::dcq(char *conf_file, char *log_file)
{
    int ret = 0;

	if (conf_file == nullptr || log_file == nullptr)
	{
		DCQ_COUT << "Could not init"
			 	 << " conf_file:" << conf_file
				 << " log_file:" << log_file
				 << DCQ_ENDL;
		return;
	}

    /* 初始化日志模块 */
	ret = dcq_log_init(log_file);
	if (ret < 0)
	{
		DCQ_COUT << "Could not open log file " << ret
			 	 << DCQ_ENDL;
		return;
	}

    /* 读取配置文件 */
	ret = dcq_config_load(conf_file, &conf);
	if (ret < 0)
	{
		DCQ_LOG(ERROR) << "Could not load configuration file " << ret;
		DCQ_COUT << "Could not load configuration file " << ret << DCQ_ENDL;
        dcq_log_release();
		return;
	}

    /* 打开本地数据库 非必须 */
    conf->db_handle = dcq_db_open(conf);
	if (conf->db_handle == nullptr)
	{
		/* 没有本地数据库就从互联网上获取，这里不会退出 */
		DCQ_COUT << "Can not open db file." << DCQ_ENDL;
	}

    return;
}

int dcq::query_domain_prepare(const char *domain)
{
	unique_lock<mutex> locker(lock);

	if (domain == nullptr)
	{
		DCQ_LOG(ERROR) << __FUNCTION__ << " parameter"
			 	       << " domain:" << domain;
		return -EINVAL;
	}

	return dcq_read_params_list(domain, domains);
}

int dcq::query_file_prepare(const char *file_path)
{
	unique_lock<mutex> locker(lock);
	if (file_path == nullptr)
	{
		DCQ_LOG(ERROR) << __FUNCTION__ << " parameter"
			 	       << " file_path:" << file_path;
		return -EINVAL;
	}

	if (domains.size() >= DOMAIN_CACHE_MAX_SIZE)
	{
		DCQ_LOG(ERROR) << __FUNCTION__ << " Support up to " 
		               << DOMAIN_CACHE_MAX_SIZE;
		return -EINVAL;
	}

	return dcq_read_file_list(file_path, domains);
}

int dcq::cache_clear()
{
	unique_lock<mutex> locker(lock);
	DCQ_LOG(INFO) << __FUNCTION__ << "clear domain cache";
	domains.clear();
	return 0;
}

size_t dcq::size()
{
	unique_lock<mutex> locker(lock);
	return domains.size();
}

int dcq::query_domain_sync(domains_result &all_info)
{
	unique_lock<mutex> locker(lock);

	if (domains.size() == 0)
	{
		DCQ_LOG(WARNING) << __FUNCTION__
						 << " domain cache is null";
		return -EEXIST;
	}

	for (size_t i = 0; i < domains.size(); i++)
	{
		domain_info info;
		get_domain_info(domains[i], info, conf);

		all_info[domains[i]] = info;
	}

	return 0;
}

future<int> dcq::query_domain_async(call_back_fn fn, domains_result &res)
{
	auto _get_info = [&res](dcq *dcq_handle, domains_result res, call_back_fn fn) {
		int ret = 0;

		for (size_t i = 0; i < dcq_handle->domains.size(); i++)
		{
			domain_info info;
			get_domain_info(dcq_handle->domains[i], info, dcq_handle->conf);

			res[dcq_handle->domains[i]] = info;
		}

		if (fn)
		{
			ret = fn(res);
		}
		return ret;
	};

	future<int> ret = std::async(_get_info, this, res, fn);
	return ret;
}

int dcq::query_domain_wait(future<int> ret)
{
	return ret.get();
}

dcq::~dcq()
{
	if (conf->db_handle) {
		dcq_db_close(conf->db_handle);
		conf->db_handle = nullptr;
	}
	dcq_log_release();
	dcq_config_free(conf);
	conf = nullptr;
}