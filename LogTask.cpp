/*
 * LogTask.cpp
 *
 *  Created on: 2016-5-14
 *      Author: chb
 */

#include "LogTask.h"
#include "AllHeader.h"


/*
 * 设计说明：
 *1）log程序使用单独进程，在初始化InitLog函数后，由主程序调用fork(），运行logTaskProc（）程序
 *2）数据处理进程与log进程间使用消息队列通讯，消息队列创建如下：
 *int logmsgid=msgget(IPC_PRIVATE,IPC_CREAT|IPC_EXCL|0660);
 *消息体定义如下：
 *
 num ENUM_MSG{STATE,ERROR,OTHER};
typedef struct
{
	ENUM_MSG msgtype;
	long msgmark;
	struct timeval timeval;
	char msgtext[256];
}ST_MSGBUF;

 *其中msgtype表示消息体是状态消息、错误消息还是其它消息
 *msgmark在状态消息时表示状态码，错误消息时表示错误码
 *timeval表示1970年1月1日到现在的时间，使用gettimeofday获得
 *msgtext：消息描述字符串
 *
 *
 *关闭log进程时，主进程将向log进程发送SIGKILL信号
 *
 *
 *
 *
 * */



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
int InitLog(char logPath[],unsigned int redays,char statusstr[])
{


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
int logTaskProc(int sendsec,int logmsgid)
{



}




