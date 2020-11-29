/*******************************************************************
 *	文件名称: request.cpp
 *	简要描述: curl POST GET 请求模块
 *	
 *	当前版本:1.0
 *	作者: ZJJ
 *	日期: 2020/11/29
 *	说明: curl 请求打印模块
 ******************************************************************/
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>

using namespace std;

static int write_data(char *buffer, size_t size, size_t nmemb, void *userp)
{
    std::string *str = dynamic_cast<std::string *>((std::string *)userp);
    str->append((char *)buffer, size * nmemb);
    return nmemb;
}

int get(string url, string *response)
{
    CURL *curl;
    CURLcode ret;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: Agent-007");
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        int status_code = curl_easy_perform(curl);
        ret = curl_easy_perform(curl);
        if (ret == 0)
        {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return status_code;
        }
        else
        {
            curl_easy_cleanup(curl);
            return ret;
        }
    }
    else
    {
        return -1;
    }
}

int posts(string url, string &body, string *response)
{
    CURL *curl;
    CURLcode ret;
    curl = curl_easy_init();
    long response_code = 0;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        ret = curl_easy_perform(curl);
        if (ret == 0)
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl);
            return 0;
        }
        else
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl);
            return ret;
        }
    }
    else
    {
        return -1;
    }
}

static size_t read_data(char *buffer, size_t size, size_t nitems, void *instream)
{
    size_t sizes = fread(buffer, size, nitems, (FILE *)instream);
    return nitems;
}

static int put(char *url, FILE *fd, int fsize, char *response)
{
    CURL *curl;
    CURLcode ret;
    curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "expect: ");
    headers = curl_slist_append(headers, "test: put");
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, &read_data);
        curl_easy_setopt(curl, CURLOPT_READDATA, fd);
        curl_easy_setopt(curl, CURLOPT_INFILE, fd);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, fsize);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        ret = curl_easy_perform(curl);
        if (ret == 0)
        {
            curl_easy_cleanup(curl);
            return 0;
        }
        else
        {
            curl_easy_cleanup(curl);
            return ret;
        }
    }
    else
    {
        return -1;
    }
}

static int post(string url, string &body, string *response)
{
    CURL *curl;
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char *)body.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str());
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    else
    {
        cout << "error" << endl;
    }

    return 0;
}

static void makeUrlencodedForm(map<string, string> const &params, string *content)
{
    content->clear();
    map<string, string>::const_iterator it;
    for (it = params.begin(); it != params.end(); it++)
    {
        char *key = curl_escape(it->first.c_str(), (int)it->first.size());
        char *value = curl_escape(it->second.c_str(), (int)it->second.size());
        *content += key;
        *content += '=';
        *content += value;
        *content += '&';
        curl_free(key);
        curl_free(value);
    }
}