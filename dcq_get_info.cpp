#include <iostream>
#include <string>
#include <map>
#include <jsoncpp/json/json.h>
#include <time.h>
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

    if (response.size() == 0 || key.size() == 0)
    {
        return -EINVAL;
    }

	if (!reader.parse(response, root))
	{
        return -ENONET;
	}
	
    Json::Value val = root[key];
    if (val.isNull() || !val.isString())
    {
        return -ENOENT;
    }

    info[key] = val.asString();

    return 0;
}

int get_domain_info(string dmain_url, domain_info &info, struct conf_context *out_conf)
{
    int i, j;
    int api_num = out_conf->icp.size();
    int status_code;
    string body;

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
			continue;
		}
        
        int key_num = out_conf->icp[i].key.size();
        for(j=0; j<key_num; j++) {
            get_respose_info(response, out_conf->icp[i].key[j], info);
        }
    }

    return 0;
}