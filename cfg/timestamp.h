#define TIMESTAMP_URL "http://www.originstamp.org/api/stamps"
#define TIMESTAMP_TOKEN "854081b6dad0c28adcd2f1907f012e3f"
#define TIMESTAMP_HEADER(T) "Authorization: Token token=%s",T
#define TIMESTAMP_POST(H,T) "{\"hash_sha256\" : \"%s\", \"title\":\"%s\"}",H,T
#define TIMESTAMP_HEADER_CONTENT "Content-Type: application/json"

#define TIMESTAMP_SSLCERTTYPE "PEM"
#define TIMESTAMP_SSLCERT "testkey.pem"
#define TIMESTAMP_KEYPASSWD ""
#define TIMESTAMP_SSLKEYTYPE "PEM"
#define TIMESTAMP_SSLKEY "testkey.pem"
#define TIMESTAMP_CAINFO "cacert.pem"
#define TIMESTAMP_SSL_VERIFYPEER 1L
