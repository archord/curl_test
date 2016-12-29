/* 
 * File:   DataTransfer.h
 * Author: xy
 *
 * Created on May 24, 2016, 10:12 PM
 */

#ifndef DataTransfer_H
#define	DataTransfer_H

#include <map> 
#include <string> 
#include <vector>
#include "AllHeader.h"

using namespace std;

#define DEBUG1

#define URL_MAX_LENGTH 10240
#define CURL_ERROR_BUFFER 10240
#define ROOT_URL "http://127.0.0.1/"
#define GET_OT1_LIST_URL "uploadAction.action"
#define SEND_OT2_CUT_IMAGE_LIST_URL "uploadAction.action"
#define GET_OT2_CUT_IMAGE_LIST_URL "uploadAction.action"
#define GET_OT2_CUT_IMAGE_REF_LIST_URL "uploadAction.action"
#define SEND_IMAGE_QUALITY_URL "uploadAction.action"
#define SEND_LOOK_BACK_URL "uploadAction.action"
#define SEND_LOG_MSG_URL "uploadAction.action"
#define SEND_MAG_CALIBRATION_URL "uploadAction.action"
#define SEND_FITS_PREVIEW_URL ""

struct CurlCache {
  char *memory;
  size_t size;
};

class DataTransfer {
public:
  char *groupId;
  char *unitId;
  char *ccdId;
  char *gridId;
  char *fieldId;

  /**mkdir /mnt/gwacMem && sudo mount -osize=100m tmpfs /mnt/gwacMem -t tmpfs*/
  char *tmpPath; //temporary file store path, Linux Memory/Temporary File System
  char *rootUrl; //web server root url http://190.168.1.25
  char *ot1ListUrl;
  char *getOt2CutImageListUrl;
  char *imageQualityFileUrl;
  char *lookBackResultUrl;
  char *sendOt2CutImageListUrl;
  char *ot2TmplCutImageListUrl;
  char *sendLogMsgUrl;
  char *sendMagCalibrationUrl;
  char *sendFitsPreviewUrl;

  DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId);
  DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId, char *rootUrl);
  DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId,
          char *rootUrl, char *sendOT1ListUrl, char *sendOT2CutImageUrl,
          char *sendImageQualityFileUrl, char *sendLookBackResultUrl,
          char *getOT2CutImageListUrl, char *getOT2TmplCutImageListUrl, char *tmpPath);
  virtual ~DataTransfer();

  void initParameter(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId,
          char *rootUrl, char *sendOT1ListUrl, char *sendOT2CutImageUrl,
          char *sendImageQualityFileUrl, char *sendLookBackResultUrl,
          char *getOT2CutImageListUrl, char *getOT2TmplCutImageListUrl, char *tmpPath);

  /*send data to server*/
  int sendOT1ListFile(char *path, char *fName, char statusstr[]);
  int sendOT2CutImage(char *path, vector<char *> &fNames, char statusstr[]);
  int sendOT2CutImageRef(char *path, vector<char *> &fNames, char statusstr[]);
  int sendImageQualityFile(char *path, char *fName, char statusstr[]);
  int sendLookBackResult(char *ot2Name, int flag, char statusstr[]);
  int sendFitsPreview(char *path, char *fName, char statusstr[]);
  int sendMagCalibrationFile(char *path, char *fName, char statusstr[]);
  int sendLogMsg(ST_MSGBUF *msg, char statusstr[]);

  /*get data from server*/
  /*orig image name, image x coordinate, image y coordinate, cut image name*/
  int getOT2CutImageList(char *path, char *fName, char statusstr[]);
  int getOT2TmplCutImageList(char *path, char *fName, char statusstr[]);

  int sendParameters(multimap<string, string> params, char statusstr[]);
  int uploadDatas(const char url[], const char path[], multimap<string, string> params, multimap<string, string> files, char statusstr[]);


private:
  struct CurlCache *tmpChunk;
  int initGwacMem(char *path);

};

#endif	/* DataTransfer_H */

