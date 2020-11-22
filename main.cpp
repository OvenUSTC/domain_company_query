#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include "request.h"
#include <jsoncpp/json/json.h>

using namespace std;

#define MZH_API "https://api.66mz8.com/api/icp.php?domain="

int main(int argc, char** argv) {
	int num = argc - 1;

	for(int i=0; i<num; i++) {
		std::map<std::string, std::string> data;
		std::string body;
		std::string response;
		int status_code;
		std::string name(argv[i+1]);
		std::string url = MZH_API + name;

		status_code = posts(url, body, &response);
		if (status_code != CURLcode::CURLE_OK) {
				cout << "未备案" << endl;
				continue;
		}

		Json::Reader reader; 
		Json::Value root; 

		if (!reader.parse(response, root))
		{ 
			cout << "未备案" << endl;
		} 
		else 
		{
			Json::Value arrayObj = root["主办单位名称"];
			if (arrayObj.isNull())
				cout << name << ": " << "未备案" << endl;
			else
				cout << name << ": " << arrayObj.asString() << endl;
		}
	}

	return 0;
}