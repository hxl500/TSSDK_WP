
#include"iot_net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
 
 
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <openssl/err.h>
//#include <openssl/ssl.h>
//#include <openssl/rand.h>
//#include <openssl/crypto.h>
#define bk_printf printf
#define iot_printf printf
#define  tcp_get_time() time(NULL)
#define BUFFER_SIZE 4096
#define os_strncpy strncpy
#define os_strlen strlen
#define os_strncmp strncmp
#define os_strchr strchr
#define os_strdup strdup
#define os_free free
#define TMQTT_LOG printf
#define os_malloc malloc
#define os_memset memset
#define os_strstr strstr
#define os_memcpy memcpy
#if 0
#include "sl_tls.h"


#include "wlan_cli_pub.h"
#include "rtos_pub.h"
#include "sockets.h"

#include "string.h"
#include "stdio.h"
#include "netdb.h"
#include"iot_https.h"
#include "tcp_mqtt_client_port.h"
#include "mqtt_client_com_port.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "rtos_pub.h"
#include <stdio.h>
#include "cJSON.h"
#include "mem_pub.h"
#include "uart_pub.h"
#include "tcp_mqtt_client_port.h"
#include "mqtt_client_com_port.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "rtos_pub.h"
#include "iot_common.h"
#include "iot_wifi_fun.h"
#include "iot_update.h"
#include "iot_uart_pro.h"
#include "api.h"
#endif


char *j1data3="{\"userId\": \"%s\",\"signature\": \"%s\"}";



static const char *HttpsPostHeadersuserformat = 	"%s %s HTTP/1.%d\r\n"
								"Host: %s:%s\r\n"
								"User-Agent: Mozilla/4.0 (%s)\r\n"
								"Connection: keep-alive\r\n"
								//"Cache-Control: no-cache\r\n"
								"content-type: %s\r\n"
								"Accept: */*\r\n"
								"%s"
								"Content-Length: %d\r\n\r\n";

#define LOGIN_HOST "/iot1/device/login"
#define MQTT_GET_HOST "/iot1/device/mqtt/info"
#define TIME_GET_HOST "/iot1/time/second"
char *otastr = NULL;
char *otastr2 = NULL;
char host_buf[128] = {0};
int mqtt_usr_test = 0;



int parseurl(httpInfo*info){
	//int ret;
	int len;
	char *p = NULL;
	char *ptr = NULL;
	if(info->url == NULL){
		bk_printf("error url\n");
		return -1;
	}
	//bk_printf("url=%s\n",info->url);
	os_strncpy(info->tmpbuf,info->url,info->maxlen - 1);
	len = os_strlen("https://");
	if(os_strncmp(info->tmpbuf,"https://",len) == 0){
		info->ssl = 0;
		p = info->tmpbuf + len;
	}else{
		info->ssl = 0;
		len = os_strlen("http://");
		if(os_strncmp(info->tmpbuf,"http://",len) != 0){
			printf("info->tmpbuf=%s\n",info->tmpbuf);
			bk_printf("error url2\n");
			return -1;
		}
		p = info->tmpbuf + len;
	}
	ptr = os_strchr(p,':');
	if(ptr == NULL){
		if(info->ssl){
			info->port = os_strdup("443");
		}else{
			info->port = os_strdup("80");
		}
		ptr = os_strchr(p,'/');
		if(ptr == NULL){
			return -3;
		}
		IOT_FREE(info->url);
		info->url = os_strdup(ptr);
		*ptr = '\0';
		info->host = os_strdup(p);
		
	}else{
		*ptr = '\0';
		ptr++;
		info->host = os_strdup(p);
		p = ptr;
		ptr = os_strchr(p,'/');
		if(ptr == NULL){
			bk_printf("error url3\n");
			return -2;
		}
		IOT_FREE(info->url);
		info->url = os_strdup(ptr);
		*ptr = '\0';
		info->port = os_strdup(p);
		
	}
	if(info->ssl){
		info->iot_connect = httpsConnect;
		info->iot_read = httpsRead;
		info->iot_write = httpsWrite;
		info->iot_disconnect = httpsDisConnect;
	}else{
		info->iot_connect = httpConnect;
		info->iot_read = httpRead;
		info->iot_write = httpWrite;
		info->iot_disconnect = httpDisConnect;

	}

	return 0;
}
#if 0
static int client_connect_tcp(char *server,int port,int use4G)
{
	int sockfd;
	struct hostent *host;
	struct sockaddr_in cliaddr;
	//printf("begin\n");
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		//perror("create socket error");
		
		return -1;
	}
	

	//121.42.255.17

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family=AF_INET;
	cliaddr.sin_port=htons(port);
	if(use4G == 0){
		SCSI_LOG("use4G0=%d\n",use4G);
		if(!(host=gethostbyname(server))){
			//printf("gethostbyname(%s) error!\n", server);
			close(sockfd);
			return -2;
		}
		inet_pton(AF_INET, host, &cliaddr.sin_addr);
		cliaddr.sin_addr=*((struct in_addr *)host->h_addr);

	}else{
		SCSI_LOG("use4G1=%d\n",use4G);
		cliaddr.sin_addr.s_addr = inet_addr("121.42.255.17");
	}

	//inet_pton(AF_INET, host, &cliaddr.sin_addr);
	//cliaddr.sin_addr=*((struct in_addr *)host->h_addr);
	//cliaddr.sin_addr.s_addr = inet_addr("121.42.255.17");

	//printf("begin\n");
	if(connect_nonblock(sockfd,(struct sockaddr *)&cliaddr,sizeof(struct sockaddr),3,use4G)<0){
		//perror("[-] error");
		close(sockfd);
		return -3;
	}
#if 1
	struct timeval tv;
	tv.tv_sec = 20;
	tv.tv_usec = 0;

	if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval)) == -1)
	{
		close(sockfd);
		return -1;
	}
	if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(struct timeval)) == -1)
	{
		close(sockfd);
		return -1;
	}
#endif

	//printf("connect ok\n");

	return(sockfd);
}


int httpConnect(httpInfo *info)
{
	int skt = -1;
	int socket;
	
	{
		int retVal = -1;
		//ip_addr_t ipAddress;
		//uint32_t ipAddress;
		struct sockaddr_in sAddr;
		//iot_printf("host=%s,port=%s\n",info->host,info->port);
		socket = -1;
		
		if (0 != netconn_gethostbyname(info->host, &ipAddress))
		{
			iot_printf("gethostbyname_failed\r\n");
			goto exit;
		}

		sAddr.sin_family = AF_INET;
		sAddr.sin_port = htons(atoi(info->port));
		os_memcpy((void *)(&sAddr.sin_addr), (void *)&ipAddress, sizeof(ipAddress));

		if ((socket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP)) < 0)
		{
			iot_printf("socket_failed\r\n");
			goto exit;
		}
		bk_printf("httpConnect socket=%d\n",socket);
		//iot_printf(" create netport.socket:%d\r\n",socket);

		if ((retVal = connect(socket, (const struct sockaddr *)&sAddr, sizeof(sAddr))) < 0)
		{
			close(socket);
			
			iot_printf("connect_failed\r\n");
		    goto exit;
		}
		struct timeval tv;
		if(info->timeout == 0){
			info->timeout  = 3000;
		}
		bk_printf("timeout=%d\n",info->timeout);
		tv.tv_sec = info->timeout/1000;   
		tv.tv_usec = (info->timeout%1000) * 1000;
		//setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(tv));
		setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(tv));	
		//iot_printf("socket succeed\r\n");
		if(retVal >= 0){
			
			info->sockfd = socket;
			bk_printf("httpConnect3 info->sockfd =%d\n",info->sockfd);
			return 0;
		}else{
			bk_printf("to close=%d\n",info->sockfd);
			close(info->sockfd);
			info->sockfd = -1;
			
		}
	exit:
		bk_printf("httpConnect info->sockfd =%d\n",info->sockfd);
		return retVal;
	}
	
	return skt;
}
#endif
int httpConnect(httpInfo *info){
    int sock = 0;
    struct sockaddr_in serv_addr;
   // char buffer[BUFFER_SIZE] = {0};
    struct hostent hostbuf, *result;
    int h_errnop;
    char hostbuf_storage[BUFFER_SIZE];
	printf("to connect %s:%s\n",info->host,info->port);
    // 创建 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        return -1;
    }

    // 使用 gethostbyname_r 解析主机名
    if (gethostbyname_r(info->host, &hostbuf, hostbuf_storage, sizeof(hostbuf_storage), &result, &h_errnop) != 0) {
        perror("gethostbyname_r failed");
		close(sock);
        return -1;
    }

    if (result == NULL) {
        fprintf(stderr, "Host not found\n");
		close(sock);
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(info->port));
    memcpy(&serv_addr.sin_addr, result->h_addr_list[0], result->h_length);

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
		close(sock);
        return -1;
    }

    printf("Connected to %s :%s,sock=%d\n", info->host,info->port,sock);
	struct timeval tv;
	if(info->timeout == 0){
		info->timeout  = 5000;
	}
	info->sockfd = sock;
	printf("timeout=%d\n",info->timeout);
	tv.tv_sec = info->timeout/1000;   
	tv.tv_usec = (info->timeout%1000) * 1000;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&tv, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&tv, sizeof(tv));	

    return sock;
}

int httpWrite(httpInfo*info,char*buf,int len)
{
	int sd_len = 0;
	//int i;
	if(info->sockfd < 0){
		return -1;
	}
	sd_len = send(info->sockfd, buf, len, 0 );
	if ( sd_len < 0 )
	{
		printf("[MQTT]tcp send ret:%d error\r\n",sd_len);
		return -1;
	}

	
	return sd_len;
}

int httpRead(httpInfo *info, char* buf, int len)
{

	unsigned int start_tick,cur_tick;
	if(info->sockfd < 0){
		return -1;
	}	
	int timeout;
	timeout = info->timeout;

	{
		int bytes = 0;
		int rc;
		

		start_tick = tcp_get_time();
		
		while (bytes < len)
		{
			fd_set readset;
			struct timeval interval;
			int ret;

			if(timeout == 0)
			{
				timeout = 1;
			}

			interval.tv_sec = timeout/1000;
			interval.tv_usec = (timeout%1000) * 1000;

			FD_ZERO(&readset);
			FD_SET(info->sockfd, &readset);

			ret = select(info->sockfd + 1, &readset, NULL, NULL, &interval);
			if(ret < 0)
			{
				bytes = -1;
				TMQTT_LOG("net_read %d:%d, break!socket select ret:%d\r\n", bytes, len,ret);
				break;
			}
			else if(ret == 0)
			{
				TMQTT_LOG("net_read %d:%d, break!socket select ret:%d\r\n", bytes, len,ret);
				break;
			}
			rc = recv(info->sockfd, &buf[bytes], (size_t)(len - bytes), MSG_DONTWAIT);
				
			if (rc == -1)
			{
				if (errno != ENOTCONN && errno != ECONNRESET)
				{
					bytes = -1;
					break;
				}
			}
			else
			{
				bytes += rc;
			}
			
			if (bytes >= len)
			{
				break;
			}
			
			cur_tick = tcp_get_time() - start_tick;
			if(cur_tick >= timeout)
			{
				break;
			}
			start_tick = tcp_get_time();
			timeout = timeout - cur_tick;
		}

		return bytes;
	}
	
	return len;
}








int httpsConnect(httpInfo*info){
	//extern MbedTLSSession * ssl_create(char *url,char *port);
#if 0
	info->tls_session = ssl_create(info->host,info->port,0);
	if(info->tls_session){
		return 0;
	}
#endif
	return -1;

}
//int httpsDisConnect(httpInfo*info);
#if 0
static int tls_demo_ssl_client_read( MbedTLSSession *session,char *buf, int maxlen){
	int ret;
	//fd_set readfds; 
	//fd_set exceptset;
	//struct timeval timeout;
	//setsockopt(session->server_fd.fd, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(xTicksToWait)); 
	ret = mbedtls_ssl_read(&session->ssl, (unsigned char *)buf, maxlen);
	return ret;

}
#endif
int httpsRead(httpInfo *info,char*buf, int len){
	int ret = 0;
#if 0
	int i = 0;
//	for(i=0;i<len;i++){
		ret =tls_demo_ssl_client_read( info->tls_session,&buf[i], len);
		if(ret <= 0){
			return -1;
		}
//	}
#endif
	return ret;
	
}
int httpsWrite(httpInfo*info,char*buf,int len){
	int ret = 0;
#if 0
	ret = ssl_txdat_sender(info->tls_session,len,buf);
#endif
	return ret;

}
void httpsDisConnect(httpInfo*info){
#if 0
	if(info->tls_session != NULL){
		mbedtls_client_close(info->tls_session);
		info->tls_session = NULL;
	}
#endif
	return;

}


void HttpDestroy(httpInfo **infop){

	bk_printf("HttpDestroy\n");
	httpInfo *info = *infop;
	info->iot_disconnect(info);

	IOT_FREE(info->host);
	IOT_FREE(info->port);
	IOT_FREE(info->body);
	IOT_FREE(info->url);
	IOT_FREE(info->headers);
	IOT_FREE(info->tmpbuf);
	IOT_FREE(*infop);

	return;
}

httpInfo* HttpInit(int ssl){
	httpInfo *info = os_malloc(sizeof(httpInfo));
	if(info == NULL){
		iot_printf(" HttpInit malloc error\n");
		return NULL;
	}
	os_memset(info,0,sizeof(httpInfo));
	info->sockfd = -1;
	info->ssl = ssl;
	if(ssl){
		info->iot_connect = httpsConnect;
		info->iot_read = httpsRead;
		info->iot_write = httpsWrite;
		info->iot_disconnect = httpsDisConnect;
	}else{
		info->iot_connect = httpConnect;
		info->iot_read = httpRead;
		info->iot_write = httpWrite;
		info->iot_disconnect = httpDisConnect;

	}
	info->timeout = 10000;
	info->maxlen = 4096*3;
	info->tmpbuf = os_malloc(info->maxlen);
	memset(info->tmpbuf,0,info->maxlen);
	if(info->tmpbuf == NULL){
		HttpDestroy(&info);
	}
	return info;
}
static int post_pack(httpInfo*info)
{

	const char *angent;
	const char *types;
	const char *headers;
	//int len;
	//angent = HttpPostPlatform;
	angent = "";
	types = "application/json";
	if(info->headers){
		headers = info->headers;
		iot_printf("headers not NULL\n");
		
	}else{
		headers= "";
		//iot_printf("headers NULL\n");
	}
	char *method;
	int httpflag = 0;
	if(info->POST){
		method = "POST";
		httpflag= 1;
	}else{
		method = "GET";
		httpflag= 0;
	}
	snprintf(info->tmpbuf,info->maxlen -1,
		HttpsPostHeadersuserformat,method,info->url,httpflag,info->host,info->port,angent,types,headers,info->bodylen);

	return os_strlen(info->tmpbuf);
}
static int post_pack2(httpInfo*info)
{

	const char *angent;
	const char *types;
	const char *headers;
	//int len;
	//angent = HttpPostPlatform;
	angent = "";
	types = "none";
	if(info->headers){
		headers = info->headers;
		iot_printf("headers not NULL\n");
		
	}else{
		headers= "";
		//iot_printf("headers NULL\n");
	}
	char *method;
	int httpflag = 0;
	if(info->POST){
		method = "POST";
		httpflag= 1;
	}else{
		method = "GET";
		httpflag = 1;
	}
	snprintf(info->tmpbuf,info->maxlen -1,
		HttpsPostHeadersuserformat,method,info->url,httpflag,info->host,info->port,angent,types,headers,info->bodylen);

	return os_strlen(info->tmpbuf);
}

static int getdatasize(char*head,int *code){
	char *p;
	char *q;
	//HTTP/1.1 200
	char buf[5];
	if(code != NULL){
		memset(buf,0,sizeof(buf));
		strncpy(buf,&head[9],3);
		*code = atoi(buf);
	}
	p = strstr(head,"Content-Length:");
	if(p == NULL){
		return -1;
	}
	p+= strlen("Content-Length:") + 1;
	q = strstr(p,"\r\n");
	if(q == NULL){
		return -1;
	}
	*q = '\0';
	return atoi(p);
	
}

static int getcode(char*head){
	//char *p;
	//char *q;
	//HTTP/1.1 200
	char buf[5];
	memset(buf,0,sizeof(buf));
	strncpy(buf,&head[9],3);
	return  atoi(buf);
}

char *tmpx = "0123456789abcdef";
static int N64toInt(char*src){
	int i;
	int num = 0;
	iot_printf("src=%s\n",src);
	while(*src){
		for(i=0;i<16;i++){
			if(*src == tmpx[i]){
				num = num << 4;
				num += i;
				break;
			}
			
		}
		src++;
	}
	iot_printf("int=%d\n",num);
	return num;
}




static int info_readHead(httpInfo*info)
{
	int reslen = -1;
#if 1
	int len = 0;
	int tmplen = 0;
	char tmpflagbegin[5];
	char tmpflag[3];
	tmpflag[0] = 0x0d;
	tmpflag[1] = 0x0a;
	tmpflag[2] = 0x00;
	tmpflagbegin[0] = 0x0d;
	tmpflagbegin[1] = 0x0a;
	tmpflagbegin[2] = 0x0d;
	tmpflagbegin[3] = 0x0a;
	tmpflagbegin[4] = 0x00;

	char *p;
	char *q;
	char *q2;
	memset(info->tmpbuf,0,info->maxlen);
	while(1){
		//if(info->ssl){
			tmplen = info->iot_read(info, &info->tmpbuf[len],1);
		//}else{
		//	tmplen = info->iot_read(info, &info->tmpbuf[len],info->maxlen - len);
		//}
		if(tmplen <=0){
			break;
		}
		if(info->tmpbuf[0] != 'H'){
			iot_printf("readinfo=%d\n",info->tmpbuf[len]);
			len = 0;
			continue;
		}
		
		len += tmplen;
		//iot_printf("head1=%s\n",info->tmpbuf);
		//iot_printf("tmplen=%d\n",tmplen);
		p = os_strstr(info->tmpbuf,"\r\n\r\n");
		if(p != NULL){
			//iot_printf("head=%s\n",info->tmpbuf);
			q = os_strstr(info->tmpbuf,"Content-Length:");
			if(q == NULL){
				q = os_strstr(info->tmpbuf,"Transfer-Encoding: chunked");
				if(q == NULL){
					iot_printf("error head=%s\n",info->tmpbuf);
					return -1;
				}
				q = os_strstr(p,tmpflagbegin);
				if(q == NULL){
					continue;
				}
				q += 4;
				q2 = os_strstr(q,tmpflag);
				if(q2 == NULL){
					continue;
				}
				*q2 = '\0';
				iot_printf("head=%s\n",info->tmpbuf);
				reslen = N64toInt(q);
				info->code = getcode(info->tmpbuf);
				p = q2 + 2;
				
			}else{
				iot_printf("head=%s\n",info->tmpbuf);
				reslen = getdatasize(info->tmpbuf,&info->code);
				p = p+ 4;
				
			}
			//iot_printf("head=%s\n",info->tmpbuf);
			tmplen = (p - info->tmpbuf);
			if(len > tmplen){
				os_memcpy(info->tmpbuf,&info->tmpbuf[tmplen],len-tmplen);
				info->tmpbuflen = len-tmplen;
				break;
			}else{
				info->tmpbuflen = 0;
			}
			break;
		}
		
	}
	//iot_printf("tmplen2=%d\n",tmplen);
	//iot_printf("info->tmpbuf=%s\n",info->tmpbuf);
	iot_printf("code=%d,reslen=%d,tmpbuflen=%d\n",info->code,reslen,info->tmpbuflen);
#if 0
	int ret;
	int i;
	tmplen = info->iot_read(info, &info->tmpbuf[info->tmpbuflen],10);
	for(i=0;i<10;i++){
		iot_printf("buf =%02x\n",info->tmpbuf[i]);
	}
	reslen = 0;
#endif
#endif
	return reslen;
	
}


int httpLenGet(httpInfo*info){
	int ret;
#if 1
	int headlen;
	int reslen;
	ret = parseurl(info);
	if(ret < 0){
		bk_printf("error url4\n");
		return ret;
	}
	ret = info->iot_connect(info);
	if(ret < 0){
		bk_printf("error connect5\n");
		return ret;
	}
	if(mqtt_usr_test == 0)
		headlen = post_pack(info);
	else{
		headlen = post_pack2(info);
		mqtt_usr_test = 0;
	}
	iot_printf("head=%s\n",info->tmpbuf);
	ret = info->iot_write(info,info->tmpbuf,headlen);
	if(ret != headlen){
		goto exit;
	}
	
	if(info->bodylen && info->body){
		iot_printf("body=%s\n",info->body);
		ret = info->iot_write(info,info->body,info->bodylen);
		if(ret != info->bodylen){
			goto exit;
		}
	}
	reslen = info_readHead(info);
	if(ret < 0){
		goto exit;
	}
	return reslen;
exit:
	info->iot_disconnect(info);
#endif
	return -1;
}
int httpLenGet_info(httpInfo*info){
	int ret;
#if 1
	int headlen;
	int reslen;
	ret = parseurl(info);
	if(ret < 0){
		bk_printf("error url4\n");
		return ret;
	}
#if 0
	ret = info->iot_connect(info);
	if(ret < 0){
		bk_printf("error connect5\n");
		return ret;
	}
#endif
	headlen = post_pack(info);
	iot_printf("head=%s\n",info->tmpbuf);
	ret = info->iot_write(info,info->tmpbuf,headlen);
	if(ret != headlen){
		iot_printf("httpLenGet_info ret3=%d\n",ret);
		goto exit;
	}
	
	if(info->bodylen && info->body){
		iot_printf("body=%s\n",info->body);
		ret = info->iot_write(info,info->body,info->bodylen);
		if(ret != info->bodylen){
			iot_printf("httpLenGet_info ret=%d\n",ret);
			goto exit;
		}
	}
	reslen = info_readHead(info);
	if(ret < 0){
		iot_printf("httpLenGet_info ret2=%d\n",ret);
		goto exit;
	}
	return reslen;
exit:
	info->iot_disconnect(info);
#endif
	return -1;
}

int httpStart(httpInfo*info){
	//unsigned long time_current_now = 0;
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart,%ld\n",time_current_now);

	int len = httpLenGet(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart head,%ld\n",time_current_now);
	int tmplen= 0;
	if(len < 0){
		return len;
	}
	if(len > info->maxlen){
		info->iot_disconnect(info);
		return -1;
	}
	
	while(len - info->tmpbuflen > 0){
		tmplen = info->iot_read(info,&info->tmpbuf[info->tmpbuflen],len - info->tmpbuflen);
		
		if(tmplen < 0){
			break;
		}
		info->tmpbuflen +=tmplen;
	}
	if(len > info->tmpbuflen){
		return -1;
	}
	info->tmpbuflen = len;
	info->tmpbuf[len] = '\0';
	iot_printf("info->tmpbuf=%s\n",info->tmpbuf);
	info->iot_disconnect(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart end,%ld\n",time_current_now);
	return 0;
	
}
int httpStart_info_continue(httpInfo*info){
//	unsigned long time_current_now = 0;
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart,%ld\n",time_current_now);

	int len = httpLenGet_info(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart head,%ld\n",time_current_now);
	int tmplen= 0;
	if(len < 0){
		return len;
	}
	if(len > info->maxlen){
		info->iot_disconnect(info);
		return -1;
	}
	
	while(len - info->tmpbuflen > 0){
		tmplen = info->iot_read(info,&info->tmpbuf[info->tmpbuflen],len - info->tmpbuflen);
		
		if(tmplen < 0){
			break;
		}
		info->tmpbuflen +=tmplen;
	}
	if(len > info->tmpbuflen){
		return -1;
	}
	info->tmpbuflen = len;
	info->tmpbuf[len] = '\0';
	iot_printf("info->tmpbuf=%s\n",info->tmpbuf);
	info->iot_disconnect(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart end,%ld\n",time_current_now);

	return 0;
	
}

int httpStart_info_connect(httpInfo*info){
//	unsigned long time_current_now = 0;
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart,%ld\n",time_current_now);

	int len = httpLenGet(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart head,%ld\n",time_current_now);
	int tmplen= 0;
	if(len < 0){
		return len;
	}
	if(len > info->maxlen){
		info->iot_disconnect(info);
		return -1;
	}
	
	while(len - info->tmpbuflen > 0){
		tmplen = info->iot_read(info,&info->tmpbuf[info->tmpbuflen],len - info->tmpbuflen);
		
		if(tmplen < 0){
			break;
		}
		info->tmpbuflen +=tmplen;
	}
	if(len > info->tmpbuflen){
		return -1;
	}
	info->tmpbuflen = len;
	info->tmpbuf[len] = '\0';
	iot_printf("info->tmpbuf=%s\n",info->tmpbuf);
	//info->iot_disconnect(info);
	//time_current_now = xTaskGetTickCount();
	//iot_printf("httpStart end,%ld\n",time_current_now);

	return 0;
	
}




//int httpConnect(httpInfo*info);
void httpDisConnect(httpInfo*info){
	bk_printf("httpDisConnect to close=%d\n",info->sockfd);

	if(info->sockfd >= 0){
		bk_printf("info->sockfd=%d\n",info->sockfd);
		close(info->sockfd);
		info->sockfd = -1;
	}
	return;
}




char* getjson(const char*url,const char*sn){
	httpInfo *info;
	int ret = -100;
	//int ret2 = -100;
	char *jsonstr =NULL;

	info = HttpInit(0);
	if(info == NULL){
		return NULL;
	}
	//info->host = strdup(data2->host1);
	//info->port = strdup(data2->port1);
	info->POST = 0;
	//#define j1data "{\"deviceSerial\": \"%s\"}"
	//snprintf(info->tmpbuf,info->maxlen,j1data,sn);
#if 0
	{"deviceSerial": "C9310B8B:48252849C"}
	snprintf(info->tmpbuf,info->maxlen,j1data2,
		data->signature,
		data->lic,
		data->sn,
		1671);
#endif
	//info->body = strdup(info->tmpbuf);
	//printf("body=%s\n",info->body);
	//info->bodylen = strlen(info->body );
	//printf("bodylen=%s\n",info->body);
	//printf("bodylen=%d\n",info->bodylen);
	
	//info->url = strdup("/api/oauth/login?timestamp=1625141571343&lang=en-US");
	//info->url = strdup("https://yun.simicloud.com/auth/dapi/login");
	char *urlsend = malloc(256);
	sprintf(urlsend,"%s?sn=%s",url,sn);
	info->url = urlsend;
	//info->url = strdup(url);
	info->headers = NULL;


	ret = httpStart_info_connect(info);
	if(ret < 0){
		goto exit;
	}
	//printf("info->tmpbuf=%s\n",info->tmpbuf);
	jsonstr = strdup(info->tmpbuf);
	
exit:
	HttpDestroy(&info);
	return jsonstr;
}
static FILE *wgetfp =NULL;
int store_ota(unsigned int addr, uint8_t *src, unsigned int len){
	fwrite(src,len,1,wgetfp);
	return 0;
}



int GetOtaBegin(char *addr ,const char*tmppath,int *filelen, int *lastlen){
	httpInfo *info;
	int ret = -100;
	//int retlen;
	bk_printf("GetOta begin\n");
	info = HttpInit(0);
	if(info == NULL){
		bk_printf("null 1\n");
		return -1;
	}
	int tmplen;
	//info->host = os_strdup(addr);
	//info->port = os_strdup(port);
	info->POST = 0;
	wgetfp = fopen(tmppath, "wb");
	if(wgetfp == NULL){
		ret = -3;
		iot_printf("fopen error \n");
		goto exit;			
	}
	if(*filelen != 0){
		sprintf(info->tmpbuf,"Range: bytes=%d-\r\n",*lastlen);
		info->headers = os_strdup(info->tmpbuf);
	}
   // os_snprintf(info->tmpbuf,info->maxlen,j1data,data->client_id,data->client_secret,data->sn);
	info->body = os_strdup("");
	//iot_printf("body=%s\n",info->body);
	info->bodylen = os_strlen(info->body);
	iot_printf("bodylen=%d\n",info->bodylen);
	
	//info->url = os_strdup(url);
	info->url = os_strdup(addr);
	
	int len;
	int tmpbuflen = 0;
	len = httpLenGet(info);
	if(len <= 0){
		ret = -3;
		iot_printf("httpLenGet error=%d\n",len);
		goto exit;		
	}
//	if(len > 0xA0000 ){
//		ret = -3;
//		iot_printf("httpLenGet error too long=%d\n",len);
//		goto exit;			
//	}
	*filelen =  len + *lastlen;
#if 0
	if(*filelen > 0xA0000 ){
		ret = -3;
		iot_printf("httpLenGet error too long2=%d\n",len);
		goto exit;			
	}
#endif
	tmpbuflen = *lastlen;


	//iot_printf("filelen=%d\n",*filelen);
	//iot_printf("info->tmpbuflen=%d\n",info->tmpbuflen);
	if(info->tmpbuflen){
		ret = store_ota(tmpbuflen, (uint8_t *)info->tmpbuf, info->tmpbuflen);
		if(ret != 0){
			iot_printf("ret=%d\n",ret);
			ret = -2;
			goto exit;
		}
		tmpbuflen+=info->tmpbuflen;
	}
	len = *filelen;
	//iot_printf("len=%d,tmpbuflen=%d\n",len,info->tmpbuflen);
	while(len - tmpbuflen > 0){
	   if(len - tmpbuflen >info->maxlen){
	   		tmplen = info->iot_read(info,info->tmpbuf,info->maxlen);
	   }else{
	   		tmplen = info->iot_read(info,info->tmpbuf,len - tmpbuflen);
	   }
	  // iot_printf("tmplen=%d,need=%d\n",tmplen,len - tmpbuflen);
	   if(tmplen <= 0){
	   		//iot_printf("error tmplen=%d\n",tmplen);
	   	   *lastlen = tmpbuflen;
		   goto exit;
	   }
		ret = store_ota(tmpbuflen, (uint8_t *)info->tmpbuf, tmplen);
		if(ret != 0){
			ret = -2;
			//iot_printf("error2 tmplen=%d\n",tmplen);
			*lastlen = tmpbuflen;
			goto exit;
		}	   
	   tmpbuflen  += tmplen;
   }
	//iot_printf("2len=%d,tmpbuflen=%d\n",len,info->tmpbuflen);
	ret = 0;
	HttpDestroy(&info);
	*lastlen = tmpbuflen;
	//mcu_notifyUpSet(2);
	//mcu_updateEndSet();
	//bk_reboot();
	if(wgetfp){
		fclose(wgetfp);
		wgetfp = NULL;
	}
	return 0;
exit:
	
	HttpDestroy(&info);

	if(wgetfp){
		fclose(wgetfp);
		wgetfp = NULL;
	}

	return -1;
	//return ret;		
}







