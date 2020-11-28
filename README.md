# 域名备案信息查询

需求：
设计一个程序（domain company query，简写dcq），可通过控制台命令行接收用户输入域名，并打印输出该域名对应的主办单位名称。

示例：

单个域名查询：
$ dcq www.sina.com
www.sina.com: 北京新浪互联信息服务有限公司

多个域名查询：
$ dcq www.sina.com www.163.com
www.sina.com: 北京新浪互联信息服务有限公司
www.163.com: 广州网易计算机系统有限公司

通过文件指定批量域名查询：
$ dcq -f domain.list
www.sina.com: 北京新浪互联信息服务有限公司
www.163.com: 广州网易计算机系统有限公司

其中domain.list文件内容如下（按行存放，一行一个域名）：
www.sina.com
www.163.com

方案一：
可以通过第三方API来查询www.sina.com备案信息，如：
http://icp.chinaz.com/www.sina.com
https://www.beian88.com/sina.com
http://www.beianbeian.com/search/www.sina.com.html

第三方Rest API可能不稳定，可能返回错误，可能返回输入验证码，你的代码需要尝试从多个第三方API获取信息来保证程序的输出。

方案二：
...


其它要求：
1，健壮性：尽可能考虑过程中可能出现的异常，比如严格的错误检查，有备用方案
2，可测试性、调试性：合理地记录日志，比如完善日志输出，中间数据输出


valgrind --tool=memcheck --leak-check=full --log-file=/var/log/leak_mem.log ./dcq -c ./example.json -d qq.com baidu.com -f ./domain.list
./dcq -c ./example.json -d qq.com baidu.com -f ./domain.list