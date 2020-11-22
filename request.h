#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>

using namespace std; 

int posts(string url, string &body,  string* response);
int get(string url, string* response);