#include <stdio.h>
#include <curl/curl.h>
 
void function_pt(void *ptr, size_t size, size_t nmemb, void *stream){
    printf("%s", ptr);
}

int main(void)
{
  CURL *curl=NULL;
  CURLcode res;
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
	struct curl_slist *chunk = NULL;
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.originstamp.org/api/stamps");
    /* Now specify the POST data */ 
	chunk = curl_slist_append(chunk, "Authorization: Token token=854081b6dad0c28adcd2f1907f012e3f");
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER,chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"hash_sha256\" : \"f6e0a1e2ac41945a9aa7ff8a8aaa0cebc12a3bcc981a929ad5cf810a090e11ae\", \"title\":\"hola2\"}");
 
    /* Perform the request, res will get the return code */ 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return 0;
}