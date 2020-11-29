# 域名备案信息查询

##　说名及使用方法
domain company query，简写dcq，可通过控制台命令行接收用户输入域名，并打印输出该域名对应的主办单位名称。

* 单个域名查询：
```
$ dcq -c conf.json www.sina.com
www.sina.com: 北京新浪互联信息服务有限公司
```

* 多个域名查询：
```
$ dcq -c conf.json www.sina.com www.163.com
www.sina.com: 北京新浪互联信息服务有限公司
www.163.com: 广州网易计算机系统有限公司
```

* 通过文件指定批量域名查询：
```
$ dcq -c conf.json -f domain.list
www.sina.com: 北京新浪互联信息服务有限公司
www.163.com: 广州网易计算机系统有限公司
```

* 其中domain.list文件内容如下（按行存放，一行一个域名）：
```
www.sina.com
www.163.com
```

## 简要实现原理

* 1.配置文件 conf.json 记录多个备案API和输出关键词
* 2.dcq 程序收集参数和list文件中的域名，通过 conf 文件设置的API获取备案信息并输出到终端

## 使用的模块

* 配置文件和API数据解析: jsoncpp
* 日志模块: glog
* POST GET 请求: curl
* 正则表达式: regex
* 参数解析: getopt

全运行方法
```
valgrind --tool=memcheck --leak-check=full --log-file=/var/log/leak_mem.log ./dcq -c ./example.json -d qq.com baidu.com -f ./domain.list
./dcq -c ./example.json -d qq.com baidu.com -f ./domain.list
```