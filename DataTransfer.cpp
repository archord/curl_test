/* 
 * File:   DataTransfer.cpp
 * Author: xy
 * 
 * Created on May 24, 2016, 10:12 PM
 */

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "DataTransfer.h"
#include "data.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
static int joinStr(char *s1, char *s2, char **s3);
static int dateToStr(struct timeval tv, char *dateStr);

/**
 * 
 * @param groupId 缺省为""
 * @param unitId 缺省为""
 * @param ccdId 缺省为""
 * @param gridId 缺省为""
 * @param fieldId 缺省为""
 */
DataTransfer::DataTransfer(char *rootUrl) {
  this->initParameter(rootUrl);
}

DataTransfer::~DataTransfer() {
  if (this->ot1ListUrl != NULL) {
    free(this->ot1ListUrl);
  }
  if (this->getOt2CutImageListUrl != NULL) {
    free(this->getOt2CutImageListUrl);
  }
  if (this->imageQualityFileUrl != NULL) {
    free(this->imageQualityFileUrl);
  }
  if (this->lookBackResultUrl != NULL) {
    free(this->lookBackResultUrl);
  }
  if (this->ot2TmplCutImageListUrl != NULL) {
    free(this->ot2TmplCutImageListUrl);
  }
  if (this->sendOt2CutImageListUrl != NULL) {
    free(this->sendOt2CutImageListUrl);
  }
  if (this->sendLogMsgUrl != NULL) {
    free(this->sendLogMsgUrl);
  }
  if(this->sendMagCalibrationUrl !=NULL){
    free(this->sendMagCalibrationUrl);
  }
  if(this->sendFitsPreviewUrl !=NULL){
    free(this->sendFitsPreviewUrl);
  }
  
  if(this->regOrigImgUrl !=NULL){
    free(this->regOrigImgUrl);
  }
  
  if (this->tmpChunk != NULL) {
    free(this->tmpChunk);
  }
}

void DataTransfer::initParameter(char *rootUrl) {
  this->rootUrl = rootUrl;

  joinStr(rootUrl, SEND_OT1_LIST_URL, &this->ot1ListUrl);
  joinStr(rootUrl, SEND_IMAGE_QUALITY_URL, &this->imageQualityFileUrl);
  joinStr(rootUrl, SEND_LOOK_BACK_URL, &this->lookBackResultUrl);
  joinStr(rootUrl, SEND_OT2_CUT_IMAGE_LIST_URL, &this->sendOt2CutImageListUrl);
  joinStr(rootUrl, SEND_LOG_MSG_URL, &this->sendLogMsgUrl);
  joinStr(rootUrl, SEND_MAG_CALIBRATION_URL, &this->sendMagCalibrationUrl);
  joinStr(rootUrl, SEND_FITS_PREVIEW_URL, &this->sendFitsPreviewUrl);

  joinStr(rootUrl, GET_OT2_CUT_IMAGE_LIST_URL, &this->getOt2CutImageListUrl);
  joinStr(rootUrl, GET_OT2_CUT_IMAGE_REF_LIST_URL, &this->ot2TmplCutImageListUrl);

  joinStr(rootUrl, REG_ORIG_IMAGE_URL, &this->regOrigImgUrl);
  
  this->tmpChunk = (struct CurlCache *) malloc(sizeof (struct CurlCache));

}

int DataTransfer::regOrigImage(char *groupId, char *unitId, char *camId, char *gridId,
        char *fieldId, char *imgName, char *imgPath, char *genTime, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("groupId", groupId));
  params.insert(std::pair<string, string>("unitId", unitId));
  params.insert(std::pair<string, string>("camId", camId));
  params.insert(std::pair<string, string>("gridId", gridId));
  params.insert(std::pair<string, string>("fieldId", fieldId));
  params.insert(std::pair<string, string>("imgName", imgName));
  params.insert(std::pair<string, string>("imgPath", imgPath));
  params.insert(std::pair<string, string>("genTime", genTime));
  return uploadDatas(this->regOrigImgUrl, "", params, files, statusstr);
}

int DataTransfer::sendOT1ListFile(char *path, char *fName, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("fileType", "crsot1"));
  files.insert(std::pair<string, string>("fileUpload", fName));
  return uploadDatas(this->ot1ListUrl, path, params, files, statusstr);
}

int DataTransfer::sendOT2CutImage(char *path, vector<char *> &fNames, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;

  params.insert(std::pair<string, string>("fileType", "ot2im"));
  size_t len = fNames.size();
  for (size_t i = 0; i < len; i++) {
    files.insert(std::pair<string, string>("fileUpload", fNames[i]));
  }
  return uploadDatas(this->sendOt2CutImageListUrl, path, params, files, statusstr);
}

int DataTransfer::sendOT2CutImageRef(char *path, vector<char *> &fNames, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;

  params.insert(std::pair<string, string>("fileType", "ot2imr"));
  size_t len = fNames.size();
  for (size_t i = 0; i < len; i++) {
    files.insert(std::pair<string, string>("fileUpload", fNames[i]));
  }
  return uploadDatas(this->sendOt2CutImageListUrl, path, params, files, statusstr);
}

int DataTransfer::sendImageQualityFile(char *path, char *fName, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("fileType", "imqty"));
  files.insert(std::pair<string, string>("fileUpload", fName));
  return uploadDatas(this->imageQualityFileUrl, path, params, files, statusstr);
}

int DataTransfer::sendFitsPreview(char *path, char *fName, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("fileType", "impre"));
  files.insert(std::pair<string, string>("fileUpload", fName));
  return uploadDatas(this->sendFitsPreviewUrl, path, params, files, statusstr);
}

int DataTransfer::sendMagCalibrationFile(char *path, char *fName, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("fileType", "magclb"));
  files.insert(std::pair<string, string>("fileUpload", fName));
  return uploadDatas(this->sendMagCalibrationUrl, path, params, files, statusstr);
}

//lookBackResultUrl

int DataTransfer::sendLookBackResult(char *ot2Name, int flag, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  char flagStr[10];
  sprintf(flagStr, "%d", flag);
  params.insert(std::pair<string, string>("ot2name", ot2Name));
  params.insert(std::pair<string, string>("flag", flagStr));
  return uploadDatas(this->lookBackResultUrl, "", params, files, statusstr);
}

int DataTransfer::sendLogMsg(ST_MSGBUF *msg, char statusstr[]) {

  multimap<string, string> params;
  multimap<string, string> files;
  params.insert(std::pair<string, string>("logType", "logchb"));
  if (msg->msgtype == ERROR) {
    params.insert(std::pair<string, string>("msgType", "error"));
  } else if (msg->msgtype == STATE) {
    params.insert(std::pair<string, string>("msgType", "state"));
  }

  char flagStr[10];
  sprintf(flagStr, "%d", msg->msgmark);
  params.insert(std::pair<string, string>("msgCode", flagStr));

  char dateStr[40];
  dateToStr(msg->timeval, dateStr);
  params.insert(std::pair<string, string>("msgDate", dateStr));
  params.insert(std::pair<string, string>("msgContent", msg->msgtext));
  return uploadDatas(this->sendLogMsgUrl, "", params, files, statusstr);
}

/**
 * 将参数或文件上次到服务器
 * @param url 服务器地址
 * @param path 文件所在独立
 * @param params 参数键值对<参数名，参数值>
 * @param files 文件键值对<上次文件名，实际文件名>，“path+实际文件名”组成待上传文件路径
 * @param statusstr 函数返回状态字符串
 * @return 函数返回状态值
 */
int DataTransfer::uploadDatas(const char url[],
        const char path[],
        multimap<string, string> params,
        multimap<string, string> files,
        char statusstr[]) {

  int rstCode = GWAC_SUCCESS;

  /*检查错误结果输出参数statusstr是否为空*/
  CHECK_STATUS_STR_IS_NULL(statusstr);
  /*检测输入参数是否为空*/
  CHECK_STRING_NULL_OR_EMPTY(url, "url");
  //  CHECK_STRING_NULL_OR_EMPTY(path, "path");

  /*检测传输数据是否为空*/
  if (params.empty() && files.empty()) {
    sprintf(statusstr, "File %s line %d, Error Code: %d\n"
            "the input parameter objvec is empty!\n",
            __FILE__, __LINE__, GWAC_FUNCTION_INPUT_EMPTY);
    return GWAC_FUNCTION_INPUT_EMPTY;
  }

  CURL *curlSession;
  CURLcode curlCode;

  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;
  struct curl_slist *headerlist = NULL;
  static const char buf[] = "Expect:";

  tmpChunk->memory = (char*) malloc(1); /* will be grown as needed by realloc above */
  tmpChunk->size = 0; /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  for (multimap<string, string>::iterator iter = params.begin(); iter != params.end(); iter++) {
    curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, iter->first.data(),
            CURLFORM_COPYCONTENTS, iter->second.data(),
            CURLFORM_END);
  }

  for (multimap<string, string>::iterator iter = files.begin(); iter != files.end(); iter++) {
    string filePath(path, path + strlen(path));
    filePath.append(iter->second.data());
    cout << iter->first.data() << ":" << filePath.data() << endl;
    curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, iter->first.data(),
            CURLFORM_FILE, filePath.data(),
            CURLFORM_END);
  }

#ifdef DEBUG  
  string conStr = "{";
  for (multimap<string, string>::iterator iter = params.begin(); iter != params.end(); iter++) {
    conStr.append(iter->first);
    conStr.append(":");
    conStr.append(iter->second);
    conStr.append(",");
  }

  for (multimap<string, string>::iterator iter = files.begin(); iter != files.end(); iter++) {
    string filePath(path, path + strlen(path));
    filePath.append(iter->second.data());

    conStr.append(iter->first);
    conStr.append(":");
    conStr.append(filePath);
    conStr.append(",");
  }
  conStr.append("}");
  cout << "conStr: " << conStr << endl;
#endif

  curlSession = curl_easy_init();
  /* initialize custom header list (stating that Expect: 100-continue is not wanted */
  headerlist = curl_slist_append(headerlist, buf);
  if (curlSession) {
    char *reqErrorBuf = (char*) malloc(sizeof (char)*CURL_ERROR_BUFFER);
    memset(reqErrorBuf, 0, sizeof (char)*CURL_ERROR_BUFFER);

#ifdef DEBUG
    curl_easy_setopt(curlSession, CURLOPT_VERBOSE, 1);
    char *encodeParm = curl_easy_escape(curlSession, conStr.data(), conStr.length());
    char *fullUrl = (char*) malloc(sizeof (char)*URL_MAX_LENGTH);
    memset(fullUrl, 0, sizeof (char)*URL_MAX_LENGTH);
    sprintf(fullUrl, "%s?rqp=%s", url, encodeParm);
    cout << "fullUrl: " << fullUrl << endl;
    curl_free(encodeParm);
#else
    char *fullUrl = (char*) malloc(sizeof (char)*URL_MAX_LENGTH);
    memset(fullUrl, 0, sizeof (char)*URL_MAX_LENGTH);
    sprintf(fullUrl, "%s", url);
#endif

    /* what URL that receives this POST */
    curl_easy_setopt(curlSession, CURLOPT_URL, fullUrl);

    /* send all data to this function  */
    curl_easy_setopt(curlSession, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curlSession, CURLOPT_WRITEDATA, (void *) tmpChunk);

    curl_easy_setopt(curlSession, CURLOPT_ERRORBUFFER, (void *) reqErrorBuf);

    if (false) {
      curl_easy_setopt(curlSession, CURLOPT_HTTPHEADER, headerlist);
    }
    curl_easy_setopt(curlSession, CURLOPT_HTTPPOST, formpost);

    /* Perform the request, curlCode will get the return code */
    curlCode = curl_easy_perform(curlSession);

    /* curl执行出错 */
    if (curlCode != CURLE_OK) {
      rstCode = GWAC_SEND_DATA_ERROR;
      sprintf(statusstr, "File %s line %d, Error Code: %d\n"
              "curl_easy_perform() failed: %s\n",
              __FILE__, __LINE__, GWAC_SEND_DATA_ERROR,
              curl_easy_strerror(curlCode));
    } else {
      long http_code = 0;
      CURLcode curlCode2 = curl_easy_getinfo(curlSession, CURLINFO_RESPONSE_CODE, &http_code);
      /**curl执行正确，且http服务器正确执行，并正常返回*/
      //      if (http_code == 200 && curlCode != CURLE_ABORTED_BY_CALLBACK) {
      if (http_code == 200 && curlCode2 == CURLE_OK) {
        rstCode = GWAC_SUCCESS;
        sprintf(statusstr, "%s\n", tmpChunk->memory);
      } else {
        /**curl执行正确，且http服务器执行异常，并返回错误*/
        rstCode = GWAC_SEND_DATA_ERROR;
        sprintf(statusstr, "File %s line %d, Error Code of http: %d\n"
                "curl_easy_perform() error: %s, server response: %s\n",
                __FILE__, __LINE__, http_code, curl_easy_strerror(curlCode), reqErrorBuf);
      }
    }

    free(fullUrl);
    free(reqErrorBuf);

    /* always cleanup */
    curl_easy_cleanup(curlSession);

    /* then cleanup the formpost chain */
    curl_formfree(formpost);
    /* free slist */
    curl_slist_free_all(headerlist);


    free(tmpChunk->memory);

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
  } else {
    rstCode = GWAC_SEND_DATA_ERROR;
    sprintf(statusstr, "File %s line %d, Error Code: %d\n"
            "In uploadDatas, the input parameter objvec is empty!\n",
            __FILE__, __LINE__, GWAC_SEND_DATA_ERROR);
  }

  return rstCode;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct CurlCache *mem = (struct CurlCache *) userp;

  mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

static int joinStr(char *s1, char *s2, char **s3) {

  *s3 = (char*) malloc(strlen(s1) + strlen(s2) + 1);
  if (*s3 == NULL) {
    return GWAC_MALLOC_ERROR;
  }
  strcpy(*s3, s1);
  strcat(*s3, s2);

  return GWAC_SUCCESS;
}

static int dateToStr(struct timeval tv, char *dateStr) {

  struct tm* ptm;
  char time_string[40];
  long milliseconds;
  ptm = localtime(&tv.tv_sec);
  strftime(time_string, sizeof (time_string), "%Y-%m-%dT%H:%M:%S", ptm);
  milliseconds = tv.tv_usec / 1000;
  sprintf(dateStr, "%s.%03ld", time_string, milliseconds);
  return GWAC_SUCCESS;
}
