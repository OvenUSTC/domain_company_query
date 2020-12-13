#include <iostream>
#include <gtest/gtest.h>
#include "libdcq.h"

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
    dcq test("./example.json", "/var/log");
    EXPECT_EQ(0, test.query_domain_prepare("baidu.com"));
    EXPECT_EQ(0, test.query_domain_prepare("xxx.com"));
    EXPECT_EQ(0, test.query_domain_prepare("qq.com"));
    EXPECT_EQ(3, test.size());
    EXPECT_EQ(0, test.cache_clear());
    EXPECT_EQ(0, test.size());
}

int main(int argc,char **argv)
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}