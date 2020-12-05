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

int main(int argc, char **argv)
{
	vector<string> domains;
	string conf_file = "";
	string log_file = DEFAULT_LOG_FILE;
	struct conf_context *out_conf = nullptr;
	dcq_db_context *db = NULL;
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
	ret = dcq_config_load(conf_file, &out_conf);
	if (ret < 0)
	{
		DCQ_LOG(ERROR) << "Could not load configuration file " << ret;
		DCQ_COUT << "Could not load configuration file " << ret << DCQ_ENDL;
		return -1;
	}

	/* 设置日志等级 */
	ret = dcq_set_log_level(out_conf->global.debug_level);
	if (ret < 0)
	{
		DCQ_COUT << "Set as the default log level 0." << DCQ_ENDL;
	}

	db = dcq_db_open(out_conf);
	if (db == NULL)
	{
		DCQ_COUT << "Can not open db file." << DCQ_ENDL;
	}

	/* 获取结果并输出 */
	for (int i = 0; i < domains.size(); i++)
	{
		domain_info info;
		get_domain_info(domains[i], info, out_conf);
		if (info.size() == 0)
		{
			DCQ_COUT << domains[i] << ": not filed" << DCQ_ENDL;
			DCQ_LOG(INFO) << domains[i] << ": not filed";
			continue;
		}

		for (auto iter = info.begin(); iter != info.end(); iter++)
		{
			DCQ_COUT << domains[i] << ": " << iter->second << DCQ_ENDL;
			DCQ_LOG(INFO) << domains[i] << ": " << iter->second;
		}

		domain_info tmp_info;
		dcq_db_insert(db, domains[i], info, DCQ_DB_INSERT);
		dcq_db_search(db, domains[i], tmp_info);
	}

	dcq_db_close(db);
	dcq_config_free(out_conf);
	out_conf = nullptr;
	return 0;
}