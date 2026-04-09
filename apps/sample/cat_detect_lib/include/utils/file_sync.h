#ifndef __CAT_FILE_SYNC_H__
#define __CAT_FILE_SYNC_H__
#if 1

#include "arrr_diff.h"
#else


#if 1
#define CAT_ARRAY_LEN 512
struct catPicInfo{
	char id[64];
	int size;
	char data[CAT_ARRAY_LEN];

}__attribute__((__packed__)); 
#endif
struct catNameInfo{
	char id[64];
	char name[128];
}__attribute__((__packed__)); 
#endif
struct catUrlInfo{
	char id[64];
	char *url;
	char *filename;
}__attribute__((__packed__)); 

int file_sync(char*jsonstr,struct catUrlInfo ***urlInfopp,int *urlInfoNump);


void set_pic_path(const char*path);
int pic_renew(char*url,char*sn);

int dir_sync(char*dirpath,struct catUrlInfo **urlInfop,int urlInfoNum,int *renew_flag);
struct catPicInfo *url2pic(char*dirpath,struct catUrlInfo**urlinfo,int urlInfoNum);
extern char* getjson(const char*url,const char*sn);
extern int GetOtaBegin(char *addr ,char*tmppath,int *filelen, int *lastlen);
void start_file_sync_i4s_server();



#endif


