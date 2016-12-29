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

/**
 * 
 * @param groupId 缺省为""
 * @param unitId 缺省为""
 * @param ccdId 缺省为""
 * @param gridId 缺省为""
 * @param fieldId 缺省为""
 */
DataTransfer::DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId) {
  this->initParameter(groupId, unitId, ccdId, gridId, fieldId,
          "http://127.0.0.1/", OT1_LIST_URL, OT2_CUT_IMAGE_UPLOAD_URL, IMAGE_QUALITY_URL,
          LOOK_BACK_URL, OT2_CUT_IMAGE_LIST_DOWNLOAD_URL, OT2_TMPL_CUT_IMAGE_LIST_DOWNLOAD_URL, "");
}

DataTransfer::DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId, char *rootUrl) {
  this->initParameter(groupId, unitId, ccdId, gridId, fieldId,
          rootUrl, OT1_LIST_URL, OT2_CUT_IMAGE_UPLOAD_URL, IMAGE_QUALITY_URL,
          LOOK_BACK_URL, OT2_CUT_IMAGE_LIST_DOWNLOAD_URL, OT2_TMPL_CUT_IMAGE_LIST_DOWNLOAD_URL, "");
}

DataTransfer::DataTransfer(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId,
        char *rootUrl,
        char *sendOT1ListUrl,
        char *sendOT2CutImageUrl,
        char *sendImageQualityFileUrl,
        char *sendLookBackResultUrl,
        char *getOT2CutImageListUrl,
        char *getOT2TmplCutImageListUrl,
        char *tmpPath) {
  this->initParameter(groupId, unitId, ccdId, gridId, fieldId,
          rootUrl, sendOT1ListUrl, sendOT2CutImageUrl, sendImageQualityFileUrl,
          sendLookBackResultUrl, getOT2CutImageListUrl, getOT2TmplCutImageListUrl, tmpPath);
}

DataTransfer::~DataTransfer() {
  if (this->tmpChunk != NULL) {
    free(this->tmpChunk);
  }
}

void DataTransfer::initParameter(char *groupId, char *unitId, char *ccdId, char *gridId, char *fieldId,
        char *rootUrl,
        char *sendOT1ListUrl,
        char *sendOT2CutImageUrl,
        char *sendImageQualityFileUrl,
        char *sendLookBackResultUrl,
        char *getOT2CutImageListUrl,
        char *getOT2TmplCutImageListUrl,
        char *tmpPath) {
  this->groupId = groupId;
  this->unitId = unitId;
  this->ccdId = ccdId;
  this->gridId = gridId;
  this->fieldId = fieldId;

  this->tmpPath = tmpPath; ///mnt/gwacMem/
  this->rootUrl = rootUrl;
  this->ot1ListUrl = sendOT1ListUrl;
  this->ot2CutImageUrl = sendOT2CutImageUrl;
  this->imageQualityFileUrl = sendImageQualityFileUrl;
  this->lookBackResultUrl = sendLookBackResultUrl;
  this->ot2CutImageListUrl = getOT2CutImageListUrl;
  this->ot2TmplCutImageListUrl = getOT2TmplCutImageListUrl;

  this->tmpChunk = (struct CurlCache *) malloc(sizeof (struct CurlCache));

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
  CHECK_STRING_NULL_OR_EMPTY(path, "path");

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
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "groupId", CURLFORM_COPYCONTENTS, groupId, CURLFORM_END);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "unitId", CURLFORM_COPYCONTENTS, unitId, CURLFORM_END);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "ccdId", CURLFORM_COPYCONTENTS, ccdId, CURLFORM_END);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "gridId", CURLFORM_COPYCONTENTS, gridId, CURLFORM_END);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "fieldId", CURLFORM_COPYCONTENTS, fieldId, CURLFORM_END);

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
    curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, iter->first.data(),
            CURLFORM_FILE, filePath.data(),
            CURLFORM_END);
  }

#ifdef DEBUG
  string conStr = "";
  conStr.append("{groupId:");
  conStr.append(groupId);
  conStr.append(",unitId:");
  conStr.append(unitId);
  conStr.append(",ccdId:");
  conStr.append(ccdId);
  conStr.append(",gridId:");
  conStr.append(gridId);
  conStr.append(",fieldId:");
  conStr.append(fieldId);
  conStr.append(",");

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
#endif

  curlSession = curl_easy_init();
  /* initialize custom header list (stating that Expect: 100-continue is not wanted */
  headerlist = curl_slist_append(headerlist, buf);
  if (curlSession) {
    char *reqErrorBuf = (char*) malloc(sizeof (char)*CURL_ERROR_BUFFER);
    memset(reqErrorBuf, 0, sizeof (char)*CURL_ERROR_BUFFER);

#ifdef DEBUG
    char *encodeParm = curl_easy_escape(curlSession, conStr.data(), conStr.length());
    char *fullUrl = (char*) malloc(sizeof (char)*URL_MAX_LENGTH);
    memset(fullUrl, 0, sizeof (char)*URL_MAX_LENGTH);
    sprintf(fullUrl, "%s?rqp=%s", url, encodeParm);
    cout << fullUrl << endl;
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
