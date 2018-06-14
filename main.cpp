/* 
 * File:   main.cpp
 * Author: xy
 *
 * Created on April 18, 2016, 10:36 AM
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

#include <map> 
#include <string> 
#include <vector>

#include "DataTransfer.h"
#include "LogTask.h"
#include "LogSysMsg.h"

using namespace std;

void print_time();
void getDiskSize(char *path);
void sendData();
void testDataTransfer();
void testLog();
void testRegOrigImg();
void testUploadTemperature();
void testUploadVacuum();
void testUploadFocus();
void testUploadMountStatus();
void testUploadCameraStatus();
void testUploadObsCtlSysStatus();

int main(int argc, char *argv[]) {

    testUploadFocus();
    testUploadMountStatus();
    testUploadCameraStatus();
    testUploadObsCtlSysStatus();

    testUploadTemperature();
    testUploadVacuum();
    //  testRegOrigImg();
    //  testDataTransfer();
    //    testLog();

    return 0;
}

void testUploadFocus() {
    int fbfId = 4242973;
    int focus = 98;
    int cameraId = 001;

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadFocus(fbfId, focus, cameraId, statusstr);
    printf("%s\n", statusstr);
}

void testUploadMountStatus() {
    char *groupId = "001";
    char *unitId = "003";
    char *utc = "2018-06-14T08:11:30";
    int state = 1;
    float errcode = 123;
    float ra = 60.1;
    float dec = 60.2;
    float objRa = 60.3;
    float objDec = 60.4;

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadMountStatus(groupId, unitId, utc, state, errcode, ra, dec, objRa, objDec, statusstr);
    printf("%s\n", statusstr);
}

void testUploadCameraStatus() {

    char *groupId = "001";
    char *unitId = "003";
    char *camId = "032";
    char *utc = "2018-06-14T08:11:30";
    int mcState = 1;
    int focus = 123;
    int coolget = -60;
    char* filter = "R";
    int state = 2;
    int errcode = 5;
    char *imgType = "obj";
    char *objName = "G180614_C00012";
    int frmNo = 120;
    char *fileName = "abc.fit";

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadCameraStatus(groupId, unitId, camId, utc, mcState,
            focus, coolget, filter, state, errcode, imgType,
            objName, frmNo, fileName, statusstr);
    printf("%s\n", statusstr);
}

void testUploadObsCtlSysStatus() {
    char *groupId = "001";
    char *unitId = "003";
    char *utc = "2018-06-14T08:11:30";
    int state = 1;
    int opSn = 128;
    char *opTime = "2018-06-14T08:19:30";
    int mountStatus = 12;
    int cameraStatus = 23;

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadObsCtlSysStatus(groupId, unitId, utc, state, opSn, opTime, mountStatus, cameraStatus, statusstr);
    printf("%s\n", statusstr);
}

void testUploadTemperature() {
    char *groupId = "g001";
    char *unitId = "u002";
    char *camId = "045";
    float voltage = 10.3;
    float current = 12.3;
    float thot = -40;
    float coolget = -35;
    float coolset = -25;
    char *time = "2017-12-10T11:50:25";

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadTemperature(groupId, unitId, camId, voltage, current, thot, coolget, coolset, time, statusstr);
    printf("%s\n", statusstr);
}

void testUploadVacuum() {
    char *groupId = "g001";
    char *unitId = "u002";
    char *camId = "044";
    float voltage = 10.3;
    float current = 12.3;
    float pressure = 1000;
    char *time = "2016-12-10T10:30:29";

    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadVacuum(groupId, unitId, camId, voltage, current, pressure, time, statusstr);
    printf("%s\n", statusstr);
}

void testRegOrigImg() {
    char *groupId = "g001";
    char *unitId = "u002";
    char *camId = "c007";
    char *gridId = "g004";
    char *fieldId = "f005";
    char *imgName = "G002_Mon_objt_162319T11233342.fit";
    char *imgPath = "/home/gwac/data/data-test";
    char *genTime = "20170104143423888";

    char statusstr[10240];
    char *rootUrl = "http://190.168.1.177:8080/gwac/";
    //    char *rootUrl = "http://190.168.1.27/";
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->regOrigImage(groupId, unitId, camId, gridId, fieldId, imgName, imgPath, genTime, statusstr);
    printf("%s\n", statusstr);
}

void testLog() {
    char *logPath = "/home/gwac/data/data-test/logs";
    unsigned int redays = 7;
    char statusstr[200];

    LogSysMsg *log = new LogSysMsg();
    log->InitLog(logPath, redays, statusstr);
    log->logTaskProc(0, 0);
}

void testDataTransfer() {

    char *rootUrl = "http://190.168.1.177:8080/gwac/";
    //    char *rootUrl = "http://190.168.1.27/";
    char *rootPath = "/home/gwac/data/data-test/";
    char *ot1list = "G002_001_objt_161230T11041213.crsot1"; //"G001_001_objt_161230T110412.subot1"
    char *imqty = "G002_Mon_objt_161219T11523152.imqty";
    char *impre = "G002_Mon_objt_161219T11523152.jpg";
    char *ot2Name = "G170103_C01113";
    int ot2Flag = 1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    ST_MSGBUF *msg = (ST_MSGBUF *) malloc(sizeof (ST_MSGBUF));
    msg->msgtype = STATE;
    msg->msgmark = 4008;
    msg->timeval = tv;
    sprintf(msg->msgtext, "%s", "test msg upload3.");

    char statusstr[10240];
    DataTransfer *gt = new DataTransfer(rootUrl);
    printf("start\n");
    //发送OT1列表文件
    //  gt->sendOT1ListFile(rootPath, ot1list, statusstr);
    //  printf("%s\n", statusstr);
    //图像参数文件
    //  gt->sendImageQualityFile(rootPath, imqty, statusstr);
    //  printf("%s\n", statusstr);
    //CCD fits预览图像
    gt->sendFitsPreview(rootPath, impre, statusstr);
    printf("%s\n", statusstr);
    //  //OT2回看结果
    //  gt->sendLookBackResult(ot2Name, ot2Flag, statusstr);
    //  printf("%s\n", statusstr);

    //程序日志
    //  gt->sendLogMsg(msg, statusstr);
    //  printf("%s\n", statusstr);
    free(msg);

    /**   */
    //切图文件
    //  vector<char *> ot2ims;
    //  ot2ims.push_back("M170102_C00005_0019.fit");
    //  ot2ims.push_back("M170102_C00005_0019.jpg");
    //  ot2ims.push_back("M170102_C00005_0020.fit");
    //  ot2ims.push_back("M170102_C00005_0020.jpg");
    //  vector<char *> ot2imrs;
    //  ot2imrs.push_back("M170102_C00005_0019_ref_20160114T113345.fit");
    //  ot2imrs.push_back("M170102_C00005_0019_ref_20160114T113345.jpg");
    //  gt->sendOT2CutImage(rootPath, ot2ims, statusstr);
    //  printf("%s\n", statusstr);
    //  gt->sendOT2CutImageRef(rootPath, ot2imrs, statusstr);
    //  printf("%s\n", statusstr);

}

/**getDiskSize("/home/xy/Downloads/myresource");*/
void getDiskSize(char *path) {
    struct statvfs fiData;
    struct statvfs *fpData;

    int aa = statvfs(path, &fiData);
    if (aa < 0) {
        printf("Failed to stat %s:\n", path);
    } else {
        printf("Disk %s: \n", path);
        printf("block size: %u\n", fiData.f_bsize);
        printf("total no blocks: %.2f\n", fiData.f_blocks * fiData.f_bsize / 1024.0 / 1024 / 1024 / 1024);
        printf("free blocks: %.2f\n", fiData.f_bfree * fiData.f_bsize / 1024.0 / 1024 / 1024 / 1024);
    }
}

void sendData() {

    char *rootUrl = "http://190.168.1.161:8080/gwac/commonFileUpload.action";
    char *ot1Url = "commonFileUpload.action";
    char *rootPath = "/home/gwac/data/data-test/";
    char *ot1list = "G002_001_objt_161230T11041213.crsot1"; //"G001_001_objt_161230T110412.subot1"
    char *imqty = "G002_Mon_objt_161219T11523152.imqty";
    char *impre = "G002_Mon_objt_161219T11523152.jpg";

    multimap<string, string> params;
    multimap<string, string> files;

    params.insert(std::pair<string, string>("fileType", "crsot1"));
    files.insert(std::pair<string, string>("fileUpload", ot1list));

    char statusstr[10240];
    DataTransfer *gt = new DataTransfer(rootUrl);
    gt->uploadDatas(rootUrl, rootPath, params, files, statusstr);
    printf("%s\n", statusstr);
}

void print_time() {
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    strftime(time_string, sizeof (time_string), "%Y-%m-%dT%H:%M:%S", ptm);
    milliseconds = tv.tv_usec / 1000;
    printf("%s.%03ld\n", time_string, milliseconds);
}