
#include <stdio.h>    
#include <curl/curl.h>

#include <string.h>
#include <map> 
#include <string> 
  
using namespace std;

#pragma comment(lib,"libcurl.lib")  

#define GWAC_SUCCESS 0
#define GWAC_ERROR 3001
#define GWAC_OPEN_FILE_ERROR 3002
#define GWAC_MALLOC_ERROR 3003
#define GWAC_FUNCTION_INPUT_NULL 3004
#define GWAC_FUNCTION_INPUT_EMPTY 3005
#define GWAC_STATUS_STR_NULL 3006
#define GWAC_STRING_NULL_OR_EMPTY 3007
#define GWAC_SEND_DATA_ERROR 3008

struct CurlCache {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int uploadFocus(char *updateFocusUrl, int focus, char *cameraId, char statusstr[]);
int uploadDatas(const char url[],
        const char path[],
        multimap<string, string> params,
        multimap<string, string> files,
        char statusstr[]);
  
int _tmain(int argc, char* argv[])    
{   
    int focus = 95;
    char *cameraId = "015";
    char statusstr[10240];
    char *rootUrl = "http://10.36.1.77:8080/gwebend/uploadFocusStatus.action";
    //char *rootUrl = "http://172.28.8.8:8080/gwebend/uploadFocusStatus.action";
    uploadFocus(rootUrl, focus, cameraId, statusstr);
    printf("%s\n", statusstr);
}  

int uploadFocus(char *updateFocusUrl, int focus, char *cameraId, char statusstr[]) {

    char tstr[64];
    multimap<string, string> params;
    multimap<string, string> files;
    
    sprintf(tstr, "%d", focus);
    params.insert(std::pair<string, string>("focus", tstr));
    sprintf(tstr, "%s", cameraId);
    params.insert(std::pair<string, string>("cameraId", tstr));
    
    return uploadDatas(updateFocusUrl, "", params, files, statusstr);
}

int uploadDatas(const char url[],
        const char path[],
        multimap<string, string> params,
        multimap<string, string> files,
        char statusstr[]) {
			
    int rstCode = GWAC_SUCCESS;
	
    if(NULL==statusstr){
        return GWAC_ERROR;
	}

    /*检测传输数据是否为空*/
    if (params.empty() && files.empty()) {
        sprintf(statusstr, "params or files cannot be empty\n");
        return GWAC_FUNCTION_INPUT_EMPTY;
    }

    CURL *curlSession;
    CURLcode curlCode;

    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";

	struct CurlCache *tmpChunk = (struct CurlCache *) malloc(sizeof (struct CurlCache));
    tmpChunk->memory = (char*) malloc(1); /* will be grown as needed by realloc above */
    tmpChunk->size = 0; /* no data at this point */
	
    curl_global_init(CURL_GLOBAL_ALL);

    //curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME, "focus",CURLFORM_COPYCONTENTS, focus,CURLFORM_END);
    //curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME, "cameraId",CURLFORM_COPYCONTENTS, cameraId,CURLFORM_END);
	
    for (multimap<string, string>::iterator iter = params.begin(); iter != params.end(); iter++) {
        curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, iter->first.data(),
                CURLFORM_COPYCONTENTS, iter->second.data(),
                CURLFORM_END);
    }
	
    curlSession = curl_easy_init();
    /* initialize custom header list (stating that Expect: 100-continue is not wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if (curlSession) {
		
        char *reqErrorBuf = (char*) malloc(sizeof (char)*1024);
        memset(reqErrorBuf, 0, sizeof (char)*1024);


        char *fullUrl = (char*) malloc(sizeof (char)*1024);
        memset(fullUrl, 0, sizeof (char)*1024);
        sprintf(fullUrl, "%s", url);
		

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
        free(tmpChunk);

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

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
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