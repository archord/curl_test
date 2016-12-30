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

#include <log4cxx/logger.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/logmanager.h>

#include "DataTransfer.h"

using namespace std;

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

  char *tpath = "/home/xy/gwac_data/test/";

  char *url = "http://localhost:8080/gwac/uploadAction.action";
  multimap<string, string> params;
  multimap<string, string> files;

  //  params["dpmname"] = "M10";
  //  params["date"] = "160527";
  //  params["curprocnumber"] = "0003";
  //  params["dfinfo"] = "/dev/sdc1      ext4  2.7T  2.4T  247G  91% /data";
  //  params["timeSend"] = "20160527234116";
  //  
  //  files["configFile"] = "M5_10_160527_1_223020_0003.properties";
  //  files["fileUpload"] = "M5_10_160527_1_223020_0003.fit.skyOT";
  //  files["fileUpload"] = "M5_10_160527_1_223020_0003.fit.monitorParaLog";

  params.insert(std::pair<string, string>("dpmName", "M10"));
  params.insert(std::pair<string, string>("currentDirectory", "160527"));
  params.insert(std::pair<string, string>("curprocnumber", "0003"));
  params.insert(std::pair<string, string>("dfinfo", "/dev/sdc1      ext4  2.7T  2.4T  247G  91% /data"));
  params.insert(std::pair<string, string>("timeSend", "20160527234116"));

  files.insert(std::pair<string, string>("configFile", "M5_10_160527_1_223020_0003.properties"));
  files.insert(std::pair<string, string>("fileUpload", "M5_10_160527_1_223020_0003.fit.skyOT"));
  files.insert(std::pair<string, string>("fileUpload", "M5_10_160527_1_223020_0003.fit.monitorParaLog"));


  char *groupId = "g001";
  char *unitId = "u001";
  char *ccdId = "c001";
  char *gridId = "g001";
  char *fieldId = "f001";
  char statusstr[10240];
  DataTransfer *gt = new DataTransfer(groupId, unitId, ccdId, gridId, fieldId, url);
  gt->uploadDatas(url, tpath, params, files, statusstr);
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

int main(int argc, char *argv[]) {

//  sendData();
  print_time();


  return 0;
}
