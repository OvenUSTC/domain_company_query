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

using namespace std;

int get_params(int argc, char **argv, string &conf_file, vector<string> &domains, string &log_file)
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
	int ret = 0;

	/* 读取参数 */
	if (get_params(argc, argv, conf_file, domains, log_file) != 0)
	{
		cout << "Domain name must be entered" << endl;
		return -1;
	}

	ret = dcq_log_init(log_file.c_str());
	if (ret < 0)
	{
		cout << "Could not open log file " << ret
			 << endl;
		return -1;
	}

	DCQ_LOG(INFO) << "staring domain company query server";

	/* 读取配置文件 */
	ret = dcq_config_load(conf_file, &out_conf);
	if (ret < 0)
	{
		DCQ_LOG(ERROR) << "Could not load configuration file " << ret;
		return -1;
	}

	/* 获取结果并输出 */
	for (int i = 0; i < domains.size(); i++)
	{
		domain_info info;
		get_domain_info(domains[i], info, out_conf);
		if (info.size() == 0)
		{
			cout << domains[i] << ": 未备案" << endl;
			continue;
		}

		for (auto iter = info.begin(); iter != info.end(); iter++)
		{
			cout << domains[i] << ": " << iter->second << endl;
		}
	}

	dcq_config_free(out_conf);
	out_conf = nullptr;
	return 0;
}