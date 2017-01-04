/* 
 * File:   LogSysMsg.cpp
 * Author: xy
 * 
 * Created on 2017年1月4日, 下午12:40
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <curl/curl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>// get file info
#include <errno.h>

#include "AllHeader.h"
#include "LogSysMsg.h"


static int getUtcDateStr(char *dateStr);
static int getDateStr(time_t t, char *dateStr);
int removeOldLogFile(char *basePath, unsigned int redays, char statusstr[]);
int checkAndMakedir(char *basePath, char statusstr[]);

LogSysMsg::LogSysMsg() {
  this->logFile = (char*) malloc(256);
}

LogSysMsg::~LogSysMsg() {
  free(this->logFile);
}

/*
 *输入：
 *char logPath[]：log文件存储目录
 *unsigned int redays：log文件保存天数
 * 功能：
 * 1)检查logPath是否存在，不存在则创建该目录
 * 2）删除logPath目录下保存日期超过redays天数的日志文件
 * 3）创建新的日志文件，命名规则为"log"+UTC年（4位）+UTC月（2位）+UTC日（2位）+UTC时（2位，24时制）+UTC分（2位）+UTC秒（2位）+“.dat”
 *    ,如该日志文件原已存在，则报错
 *
 * 输出：
 * 如正确返回，则返回GWAC_OK
 * 如错误返回，则返回4000~4999内的值，具体意义自定，此外，可以赋予statusstr[]相关错误描述，字节数小于128
 *
 * */
int LogSysMsg::InitLog(char logPath[], unsigned int redays, char statusstr[]) {
  int dirFlag = checkAndMakedir(logPath, statusstr);
  if (dirFlag == GWAC_OK) {
    char dateStr[40];
    getUtcDateStr(dateStr);
    sprintf(this->logFile, "%s/log%s.dat", logPath, dateStr);

    FILE *fp = NULL;
    if ((fp = fopen(this->logFile, "w")) == NULL) {
      sprintf(statusstr, "open file error.");
      return OPEN_FILE_ERROR;
    }
    fclose(fp);
    return removeOldLogFile(logPath, redays, statusstr);
  } else {
    return dirFlag;
  }
}

/*
 *输入:
 *sendsec:每隔sendsec秒向数据库服务器发送状态信息
 *logmsgid：与主进程通讯使用的消息队列标志符
 * 功能：
 *1）无限循环接收处理进程发送的消息，直到主进程发送SIGKILL信号
 *2）如接收消息体类型为ERROR，则首先记录该消息至log文件，然后将相关信息发送至数据库服务器；如消息体类型为STATE，则首先记录该消息至log文件，然后
 *	每隔sendsec秒将最新状态参数发送至数据库服务器；其它消息类型则记录该消息至log文件即可
 *
 * */
int LogSysMsg::logTaskProc(int sendsec, int logmsgid) {
  printf("log file name: %s\n", this->logFile);
}

int checkAndMakedir(char *basePath, char statusstr[]) {

  char tstr[128];
  if (access(basePath, F_OK) == -1) {
    for (int i = 0; i < strlen(basePath); i++) {
      if (basePath[i] == '/' && i != 0) {
        memset(tstr, 0, 128);
        strncpy(tstr, basePath, i + 1);
        if (access(tstr, F_OK) == -1) {
          if (mkdir(tstr, 0755) == -1) {
            sprintf(statusstr, "%s", strerror(errno));
            return CREATE_FILE_ERROR;
          }
        }
      }
    }
    if (access(basePath, F_OK) == -1) {
      if (mkdir(basePath, 0755) == -1) {
        sprintf(statusstr, "%s", strerror(errno));
        return CREATE_FILE_ERROR;
      }
    }
  }
  return GWAC_OK;
}

int removeOldLogFile(char *basePath, unsigned int redays, char statusstr[]) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char tmp[1000];

  time_t currentTime;
  tzset();
  currentTime = time(NULL);
  getDateStr(currentTime, tmp);

  if ((dir = opendir(basePath)) == NULL) {
    //    perror("Open dir error...");
    sprintf(statusstr, "%s", strerror(errno));
    return OPEN_FILE_ERROR;
  }

  chdir(basePath);
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == 8) {
      if (lstat(entry->d_name, &statbuf) == -1) {
        sprintf(statusstr, "%s", strerror(errno));
        return READ_FILE_PROPERTY_ERROR;
      }
      //statbuf.st_ctime st_atime st_mtime
      time_t createTime = statbuf.st_mtime;
      getDateStr(createTime, tmp);
      if ((currentTime - createTime) > (redays * 24 * 3600)) { //redays
        if (remove(entry->d_name) == -1) {
          sprintf(statusstr, "%s", strerror(errno));
          return DELETE_FILE_ERROR;
        }
      }
    }
  }
  closedir(dir);
  return GWAC_OK;
}

static int getUtcDateStr(char *dateStr) {
  char time_string[40];
  time_t t;
  tzset();
  t = time(NULL);
  struct tm *gtm = gmtime(&t);
  strftime(time_string, sizeof (time_string), "%Y%m%d%H%M%S", gtm);
  sprintf(dateStr, "%s", time_string);

  return GWAC_OK;
}

static int getDateStr(time_t t, char *dateStr) {
  char time_string[40];
  struct tm *gtm = gmtime(&t);
  strftime(time_string, sizeof (time_string), "%Y%m%d%H%M%S", gtm);
  sprintf(dateStr, "%s", time_string);
  return GWAC_OK;
}
