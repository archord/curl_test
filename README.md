# curl_test

libcurl包安装：
1，源码安装：官网地址：https://curl.haxx.se/download.html，下载解压后，make&make install 完成安装。
2，命令行安装：1)fedora:yum install libcurl-devel.x86_64;

DataTransfer类对数据上传Web服务器接口进行封装。
LogSysMsg对生成日志文件及日志文件上传进行封装，目前还未实现从消息队列读取日志并定时发送消息。
这两个类的使用示例见main.cpp。
