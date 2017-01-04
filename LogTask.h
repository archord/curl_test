/*
 * LogTask.h
 *
 *  Created on: 2016-5-14
 *      Author: chb
 */

#ifndef LOGTASK_H_
#define LOGTASK_H_


int InitLog(char logPath[], unsigned int redays, char statusstr[]);
int logTaskProc(int sendsec, int logmsgid);


#endif /* LOGTASK_H_ */
