/*******************************************************************
 *	文件名称: dcq_domain_list.cpp
 *	简要描述: domain company query 批量域名文件加载
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: 批量域名文件和输入参数，域名列表加载
 ******************************************************************/
#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <fstream>
#include <regex>
#include "dcq_domain_list.h"

using namespace std;

int dcq_read_file_list(const char *file_name, vector<string> &domains)
{
    string line = "";
    regex pattern(DOMAIN_PATTERN);
    long index = 0;

    if (file_name == nullptr) {
        cout << "Error dcq_read_list_file file_name:NULL"
             << endl;
        return -EINVAL;
    }
        
    fstream list_file(file_name);
    if (!list_file.is_open())
    {
        cout << "Error opening file"
             << file_name
             << endl;
        return -EEXIST;
    }

    while (getline(list_file, line))
    {   
        index++;
        if (regex_match(line, pattern))
        {
            domains.push_back(line);
        }
        else
        {
            cout << "Warn file:" << file_name
                 << " line:" << index
                 << " " << line
                 << " is not a domain skip." << endl;
        }
    }

    list_file.close();

    return 0;
}

int dcq_read_params_list(const char *domain, vector<string> &domains)
{
    regex pattern(DOMAIN_PATTERN);

    if (domain == NULL)
    {
        cout << "Error dcq_read_list_file domain:NULL"
             << endl;
        return -EINVAL;
    }

    if (regex_match(domain, pattern))
    {
        domains.push_back(domain);
    }
    else
    {
        cout << "Warn " << domain
             << " is not a domain skip." << endl;
    }
    
    return 0;
}