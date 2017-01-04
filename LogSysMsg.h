/* 
 * File:   LogSysMsg.h
 * Author: xy
 *
 * Created on 2017年1月4日, 下午12:40
 */

#ifndef LOGSYSMSG_H
#define	LOGSYSMSG_H

#define OPEN_FILE_ERROR 4001
#define READ_FILE_PROPERTY_ERROR 4002
#define DELETE_FILE_ERROR 4003
#define CREATE_FILE_ERROR 4004

class LogSysMsg {
public:
  LogSysMsg();
  virtual ~LogSysMsg();

  int InitLog(char logPath[], unsigned int redays, char statusstr[]);
  int logTaskProc(int sendsec, int logmsgid);
private:
  char *logFile;
};

#endif	/* LOGSYSMSG_H */

