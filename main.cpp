#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <unistd.h>
#include "request.h"
#include <jsoncpp/json/json.h>
#include "dcq_conf.h"
#include "dcq_get_info.h"

using namespace std;

int get_params(int argc, char **argv, string &conf_file, vector<string> &domains)
{
	char ch;
	int i;

	while ((ch = getopt(argc, argv, "c:d:")) != -1)
	{
		switch (ch)
		{
		case 'c':
			conf_file = optarg;
			break;
		case 'd':
			i = 0;
			domains.push_back(optarg);
			while(argv[optind + i] != NULL && argv[optind + i][0] != '-')
			{
				domains.push_back(argv[optind + i++]);
			}
			
			break;

		case '?':
			printf("Unknown option: %c\n",(char)optopt);
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
	struct conf_context *out_conf = nullptr;
	int ret = 0;

	/* 读取参数 */
	if(get_params(argc, argv, conf_file, domains) != 0)
	{
		cout << "Domain name must be entered" << endl;
		return -1;
	}

	/* 读取配置文件 */
	ret = dcq_config_load(conf_file, &out_conf);
	if (ret < 0)
	{
		cout << "Could not load configuration file " << ret
			 << endl;
		return -1;
	}

	/* 获取结果并输出 */
	for (int i = 0; i < domains.size(); i++)
	{
		domain_info info;
		get_domain_info(domains[i], info, out_conf);
		for(auto iter = info.begin(); iter!=info.end(); iter++)
		{
			cout << domains[i] << ": " << iter->second << endl;
		}
	}

	dcq_config_free(out_conf);
	out_conf = nullptr;
	return 0;
}