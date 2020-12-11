#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <unistd.h>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include <thread>
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

int dcq::push_domain(const char *domain)
{
	unique_lock<mutex> locker(lock);

	if (domain == nullptr)
	{
		DCQ_COUT << __FUNCTION__ << " parameter"
			 	 << " domain:" << domain
				 << DCQ_ENDL;
		return -EINVAL;
	}

	return dcq_read_params_list(domain, domains);
}

int dcq::push_file(const char *file_path)
{
	unique_lock<mutex> locker(lock);
	if (file_path == nullptr)
	{
		DCQ_COUT << __FUNCTION__ << " parameter"
			 	 << " file_path:" << file_path
				 << DCQ_ENDL;
		return -EINVAL;
	}

	if (domains.size() >= DOMAIN_CACHE_MAX_SIZE)
	{
		DCQ_COUT << __FUNCTION__ << " Support up to " 
		         << DOMAIN_CACHE_MAX_SIZE
				 << DCQ_ENDL;
		return -EINVAL;
	}

	return dcq_read_file_list(file_path, domains);
}

int dcq::cache_swap()
{
	unique_lock<mutex> locker(lock);
	domains.clear();
	return 0;
}

size_t dcq::size()
{
	unique_lock<mutex> locker(lock);
	return domains.size();
}

int dcq::get_info_sync(domains_info &all_info)
{
	return _get_info(this, all_info, nullptr);
}

/*int dcq::_get_info(dcq *dcq_handle, domains_info &all_info, call_back_fn fn)
{
	unique_lock<mutex> locker(dcq_handle->lock);
	for (int i = 0; i < dcq_handle->domains.size(); i++)
	{
		domain_info info;
		get_domain_info(dcq_handle->domains[i], info, dcq_handle->conf);

		all_info[dcq_handle->domains[i]] = info;
	}

	if (fn)
		fn(all_info);

	return 0;
}

int dcq::get_info_async(domains_info &all_info, call_back_fn fn)
{	
	thread work_thread(_get_info, this, all_info, fn);
}*/

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