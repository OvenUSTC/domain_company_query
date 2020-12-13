#ifndef __LIB_DCQ_H__
#define __LIB_DCQ_H__
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <future>

using namespace std;

typedef map<string, map<string, string> > domains_result;
typedef int (*call_back_fn) (domains_result &info);

class dcq
{
private:
    struct conf_context *conf;
    vector<string> domains;
    mutex lock;
    vector<thread> th;

public:
    dcq(char *conf_file, char *log_file);
    ~dcq();
    int query_domain_prepare(const char* domain);
    int query_file_prepare(const char *file_path);
    int cache_clear();
    size_t size();
    int query_domain_sync(domains_result &res);
    future<int> query_domain_async(call_back_fn fn, domains_result &res);
    int query_domain_wait(future<int> ret);
};


#endif