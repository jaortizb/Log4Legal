#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <openssl/sha.h>
#include "timestamp.h"

static char *
sSHA256(unsigned char* input)
{
    SHA256_CTX context;
	unsigned long length;
	unsigned char md[SHA256_DIGEST_LENGTH];
	static char md_str[SHA256_DIGEST_LENGTH*2+1]="";
	int i=0,len=0;

    if(!SHA256_Init(&context))
        return NULL;

    if(!SHA256_Update(&context, input, strlen(input)))
        return NULL;

    if(!SHA256_Final(md, &context))
        return NULL;
	
	strcpy(md_str,"");
	
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(md_str + (i * 2), "%02x", md[i]);
    }
    md_str[64] = 0;

    return md_str;
}
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

char* timestamp(char *tohash, char *titulo)
{
  int ini=1;
  CURL *curl=NULL;
  CURLcode res;
  struct curl_slist *chunk = NULL;
  static char none[256];
  struct MemoryStruct chunk2;

  static const char *pPassphrase = NULL;
 
  static const char *pCertFile = TIMESTAMP_SSLCERT;
  static const char *pCACertFile=TIMESTAMP_CAINFO;
 
  static const char *pKeyName;
  static const char *pKeyType;
 
  static const char *pEngine;
  static char token[256]="";

  //  if(!token[0]) 
  strcpy(token,TIMESTAMP_TOKEN);
// 	if(ini){
		curl_global_init(CURL_GLOBAL_ALL);
 		curl = curl_easy_init();
 		chunk2.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
		chunk2.size = 0;    /* no data at this point */ 
//	}
  
//  if(curl!=NULL) {
//	if(ini!=0) {
		curl_easy_setopt(curl, CURLOPT_URL, TIMESTAMP_URL);
		chunk = curl_slist_append(chunk, TIMESTAMP_HEADER_CONTENT);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER,chunk);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk2);
		if(!strncmp("https",TIMESTAMP_URL,5)) {
#ifdef USE_ENGINE
			if (curl_easy_setopt(curl, CURLOPT_SSLENGINE,pEngine) != CURLE_OK)
			{                     /* load the crypto engine */ 
			  fprintf(stderr,"can't set crypto engine\n");
			  break;
			}
			if (curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT,1L) != CURLE_OK)
			{ /* set the crypto engine as default */ 
			  /* only needed for the first time you load
				 a engine in a curl object... */ 
			  fprintf(stderr,"can't set crypto engine as default\n");
			  break;
			}
#endif
			pKeyName  = TIMESTAMP_SSLKEY;
			pKeyType  = TIMESTAMP_SSLKEYTYPE;
		    curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,TIMESTAMP_SSLCERTTYPE);
			curl_easy_setopt(curl,CURLOPT_SSLCERT,pCertFile);
			if (pPassphrase) curl_easy_setopt(curl,CURLOPT_KEYPASSWD,pPassphrase);
			curl_easy_setopt(curl,CURLOPT_SSLKEYTYPE,pKeyType);
			curl_easy_setopt(curl,CURLOPT_SSLKEY,pKeyName);
			curl_easy_setopt(curl,CURLOPT_CAINFO,pCACertFile);
			curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,TIMESTAMP_SSL_VERIFYPEER);
		}
//	}
	sprintf(none,TIMESTAMP_HEADER(TIMESTAMP_TOKEN));
	chunk = curl_slist_append(chunk, none);
//	sprintf(none,TIMESTAMP_POST(sSHA256(tohash),titulo));
	sprintf(none,TIMESTAMP_POST(sSHA256(tohash),titulo));

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, none);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    curl_easy_cleanup(curl);
	ini=0;

//  }
  curl_global_cleanup();
//  strcpy(none,chunk2.memory);
  memcpy(none, chunk2.memory, strlen(chunk2.memory)+1);
  return none;
}

