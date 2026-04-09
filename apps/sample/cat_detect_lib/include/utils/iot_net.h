#ifndef IOT_NET_H
#define IOT_NET_H
#ifdef __cplusplus
extern "C"
{
#endif
#include<stdio.h>
#include<stdlib.h>


//#include "iot_flash.h"
//#include "tls_client.h"
struct httpInfo_t;

typedef int (*http_connect_callback)(struct httpInfo_t*info);
typedef int (*http_read_callback)(struct httpInfo_t*info, char*buf, int len);
typedef int (*http_write_callback)(struct httpInfo_t*info, char*buf, int len);
typedef void (*http_disconnect_callback)(struct httpInfo_t*info);	
	typedef struct httpInfo_t{
		char *host;
		char *port;
		int timeout;
		int POST;
		char *body;
		int bodylen;
		char *url;
		char *headers;
		char *tmpbuf;
		int code;
		int tmpbuflen;
		int maxlen;
		//MbedTLSSession *tls_session;
		int sockfd;
		int ssl;
		http_connect_callback iot_connect;
		http_read_callback iot_read;
		http_write_callback iot_write;
		http_disconnect_callback iot_disconnect;
	}httpInfo;


	int httpsRead(httpInfo *info, char* buf, int len);
	int httpsWrite(httpInfo*info,char*buf,int len);
	int httpsConnect(httpInfo*info);
	void httpsDisConnect(httpInfo*info);

	int httpRead(httpInfo* info, char*buf, int len);
	int httpWrite(httpInfo*info,char*buf,int len);
	int httpConnect(httpInfo*info);
	void httpDisConnect(httpInfo*info);	
	void testconforzero(void);
	int httpLenGet(httpInfo*info);
	int GetOtaBegin(char *addr ,const char*tmppath,int *filelen, int *lastlen);
#ifndef IOT_FREE
#define IOT_FREE(x) do{if(x != NULL)os_free(x); x= NULL;}while(0)
#endif


#ifdef __cplusplus
}
#endif


#endif





