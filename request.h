#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>

using namespace std; 

#ifndef __REQUEST_H__
#define __REQUEST_H__

int posts(string url, string &body,  string* response);
int get(string url, string* response);

#endif