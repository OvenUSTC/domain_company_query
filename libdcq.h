#ifndef __LIB_DCQ_H__
#define __LIB_DCQ_H__
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

typedef map<string, map<string, string> > domains_info;
typedef void (*call_back_fn) (domains_info &info);

class dcq
{
private:
    struct conf_context *conf;
    vector<string> domains;
    mutex lock;

public:
    dcq(char *conf_file, char *log_file);
    ~dcq();
    int push_domain(const char* domain);
    int push_file(const char *file_path);
    int cache_swap();
    size_t size();
    int get_info_sync(domains_info &info);
    //int get_info_async(domains_info &all_info, call_back_fn fn);
    static int _get_info(dcq *dcq_handle, domains_info &all_info, call_back_fn fn);
};


#endif