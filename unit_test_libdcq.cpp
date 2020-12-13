#include <iostream>
#include <gtest/gtest.h>
#include "libdcq.h"
#include "request.h"
#include "dcq_conf.h"
#include "dcq_get_info.h"
#include "dcq_domain_list.h"
#include "dcq_log.h"
#include "dcq_db.h"

using namespace std;

int get_domain_info(domains_result &info)
{
    for (auto &kv : info)
    {
        cout << kv.first << " has value " << endl;
    }

    return 0;
}

TEST(testCase,domain_push){
    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(-EINVAL, test.query_domain_prepare(nullptr));
    EXPECT_EQ(-EINVAL, test.query_domain_prepare("nullptr"));
}

TEST(testCase,domain_file){
    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_file_prepare("./domain.list"));
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(-EINVAL, test.query_file_prepare(nullptr));
}

TEST(testCase,domain_size){
    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(0, test.query_domain_prepare("xxx.com"));
    EXPECT_EQ(0, test.query_domain_prepare("qq.com"));
    EXPECT_EQ(-EINVAL, test.query_domain_prepare("q#!^!^qq.com"));
    EXPECT_EQ(-EINVAL, test.query_domain_prepare("q#!^77!^qq.com"));
    EXPECT_EQ(3, test.size());
}

TEST(testCase,domain_clear){
    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(0, test.query_domain_prepare("xxx.com"));
    EXPECT_EQ(0, test.query_domain_prepare("qq.com"));
    EXPECT_EQ(3, test.size());
    EXPECT_EQ(0, test.cache_clear());
    EXPECT_EQ(0, test.size());
}

TEST(testCase,domain_query){
    domains_result result;

    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(0, test.query_domain_sync(result));
    EXPECT_STREQ("北京百度网讯科技有限公司", result["baidu.com"]["主办单位名称"].c_str());
    EXPECT_EQ(0, test.query_domain_prepare("qq.com"));
    EXPECT_EQ(0, test.query_domain_sync(result));
    EXPECT_STREQ("深圳市腾讯计算机系统有限公司", result["qq.com"]["主办单位名称"].c_str());
}


int dcq_show_domain(string &domain, domain_info &info, void* data)
{
    domains_result *result = (domains_result *)data;
    
    (*result)[domain] = info;
	return 0;
}

TEST(testCase,domain_query_async){
    domains_result result;

    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    future<int> ret = test.query_domain_async(dcq_show_domain, result);
    EXPECT_EQ(0, ret.get());
    EXPECT_STREQ("北京百度网讯科技有限公司", result["baidu.com"]["主办单位名称"].c_str());
    EXPECT_EQ(0, test.query_domain_prepare("qq.com"));
    ret = test.query_domain_async(dcq_show_domain, result);
    EXPECT_EQ(0, ret.get());
    EXPECT_STREQ("深圳市腾讯计算机系统有限公司", result["qq.com"]["主办单位名称"].c_str());
}

TEST(testCase, dcq_init){
    dcq_db_context *db_handle = nullptr;

    EXPECT_EQ(0, dcq_log_init(nullptr));

    struct conf_context *conf = nullptr;
    EXPECT_EQ(-EINVAL, dcq_config_load("./dcq", nullptr));
    EXPECT_EQ(0, dcq_config_load("./var/example.json", &conf));

    EXPECT_EQ(-EINVAL, dcq_set_log_level(-1));
    EXPECT_EQ(-EINVAL, dcq_set_log_level(4));
    EXPECT_EQ(0, dcq_set_log_level(0));
    
    EXPECT_EQ(nullptr, dcq_db_open(nullptr));
    EXPECT_NE(nullptr, db_handle = dcq_db_open(conf));

    EXPECT_EQ(-EINVAL, dcq_db_close(nullptr));
    EXPECT_EQ(0, dcq_db_close(db_handle));

    EXPECT_EQ(0, dcq_log_release());
    dcq_config_free(conf);
}

int main(int argc,char **argv)
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}