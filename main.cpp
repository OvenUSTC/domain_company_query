/*******************************************************************
 *	文件名称: main.cpp
 *	简要描述: domain company query 主程序
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 主函数及参数获取函数
 ******************************************************************/
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <unistd.h>
#include <glog/logging.h>
#include <jsoncpp/json/json.h>
#include "request.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "dcq_domain_list.h"
#include "dcq_log.h"
#include "dcq_db.h"

using namespace std;

int dcq_get_params(int argc, char **argv, string &conf_file, vector<string> &domains, string &log_file)
{
	char ch;
	int i;

	while ((ch = getopt(argc, argv, "c:d:f:l:")) != -1)
	{
		switch (ch)
		{
		case 'c':
			conf_file = optarg;
			break;
		case 'd':
			i = 0;
			domains.push_back(optarg);
			while (argv[optind + i] != NULL && argv[optind + i][0] != '-')
			{
				dcq_read_params_list(argv[optind + i++], domains);
			}

			break;
		case 'f':
			i = 0;
			dcq_read_file_list(optarg, domains);
			while (argv[optind + i] != NULL && argv[optind + i][0] != '-')
			{
				dcq_read_file_list(argv[optind + i++], domains);
			}

			break;
		case 'l':
			log_file = optarg;
			break;
		case '?':
			printf("Unknown option: %c\n", (char)optopt);
			break;

		default:
			break;
		}
	}

	if (domains.size() == 0)
		return -EINVAL;

	return 0;
}

int dcq_show_domain(string &domain, domain_info &info, void* data)
{
	if (info.size() == 0)
	{
		DCQ_COUT << domain << ": not filed" << DCQ_ENDL;
		DCQ_LOG(INFO) << domain << ": not filed";
	}

	for (auto iter = info.begin(); iter != info.end(); iter++)
	{
		DCQ_COUT << domain << ": " << iter->second << DCQ_ENDL;
		DCQ_LOG(INFO) << domain << ": " << iter->second;
	}

	return 0;
}

int main(int argc, char **argv)
{
	vector<string> domains;
	string conf_file = "";
	string log_file = DEFAULT_LOG_FILE;
	struct conf_context *conf = nullptr;

	int ret = 0;

	/* 读取参数 */
	if (dcq_get_params(argc, argv, conf_file, domains, log_file) != 0)
	{
		DCQ_COUT << "Domain name must be entered" << DCQ_ENDL;
		return -1;
	}

	/* 初始化日志模块 */
	ret = dcq_log_init(log_file.c_str());
	if (ret < 0)
	{
		DCQ_COUT << "Could not open log file " << ret
			 	 << DCQ_ENDL;
		return -1;
	}

	DCQ_LOG(INFO) << "staring domain company query server";

	/* 读取配置文件 */
	ret = dcq_config_load(conf_file, &conf);
	if (ret < 0)
	{
		DCQ_LOG(ERROR) << "Could not load configuration file " << ret;
		DCQ_COUT << "Could not load configuration file " << ret << DCQ_ENDL;
		return -1;
	}

	/* 设置日志等级 */
	ret = dcq_set_log_level(conf->global.debug_level);
	if (ret < 0)
	{
		DCQ_COUT << "Set as the default log level 0." << DCQ_ENDL;
	}

	conf->db_handle = dcq_db_open(conf);
	if (conf->db_handle == NULL)
	{
		/* 没有本地数据库就从互联网上获取，这里不会退出 */
		DCQ_COUT << "Can not open db file." << DCQ_ENDL;
	}

	/* 获取结果并输出 */
	domains_info all_info;
	get_domain_info_multithread(domains, conf, dcq_show_domain, &all_info);

	if (conf->db_handle) {
		dcq_db_close(conf->db_handle);
		conf->db_handle = nullptr;
	}
	dcq_log_release();
	dcq_config_free(conf);
	conf = nullptr;
	return 0;
}