#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <time.h>
#include <utime.h>

#include <sys/wait.h>
#include "file_sync.h"

struct catUrlInfo **urlInfop_g;
int urlInfoNum_g;
static int changenum =0;

static  int  delete_loop_dir(char *name)
{
	struct dirent *dirp = NULL;
	struct stat st;
	DIR *dp = NULL;
    int namelen;
    if(name == NULL){
        printf("name||fuc error\n");
        return -1;
    }
    namelen = strlen(name);
	if(stat(name,&st) == -1)
	{
        printf("stat error file: %s\n",name);
		return -2;
	}
	if(!S_ISDIR(st.st_mode)){
		return 0;
	}
	dp = opendir(name);
	if( dp == NULL ){
        printf("opendir error file: %s\n",name);
		return -2;
	}
    name[namelen] = '/';
	while((dirp = readdir(dp)) != NULL)
	{
        if(dirp->d_name[0] == '.'){
            continue;
        }
        strcpy(&name[namelen+1],dirp->d_name);
		
		if(stat(name,&st) == -1)
		{
			printf("stat error file: %s\n",name);
			return -2;
		}
		if(S_ISDIR(st.st_mode)){
			continue;
		}
		char *p;
		p = strrchr(name,'.');
		if(p == NULL){
			continue;
		}
		if(strcmp(p,".rgba") == 0)
		{
			//printf("check delete name=%s\n",name);
			int i;
			int remove = 1;
			for(i=0;i<urlInfoNum_g;i++){
				if(strcmp(urlInfop_g[i]->filename,dirp->d_name) == 0){
					if(st.st_size == 200704){
						remove = 0;
					}
				}
			}
			if(remove == 1){
				unlink(name);
				changenum++;
				printf("delete ok name=%s\n",name);
			}
		}

		//if((ret = fuc(name,&st,data)) == -1){
		//	return -1;
		//}

	}
	closedir(dp);
    name[namelen] = '\0';
	return 0;
}
static int pic_is_ok(char*path,char *pic){
	char tmpbuf[1024];
	sprintf(tmpbuf,"%s/%s",path,pic);
	struct stat st;
	if(stat(tmpbuf, &st) != 0){
		return 0;
	}
	if(st.st_size != 200704){
		remove(tmpbuf);
		return 0;
	}
	return 1;
}

static int down_pic(char*dirpath,struct catUrlInfo *urlInfo){
	char buf[1024];
	char tmpbuf[1024];
	int filelen = 0;
	int lastlen = 0;
	int ret;
	changenum++;
	sprintf(tmpbuf,"%s/rgba.tmp",dirpath);
	printf("download name=%s\n",urlInfo->filename);
	ret = GetOtaBegin(urlInfo->url,tmpbuf,&filelen,&lastlen);
	if(ret < 0){
		return ret;
	}
	//return -1;
	//sprintf(buf,"wget %s -O tmp",urlInfo->url);
	//system(buf);
	sprintf(buf,"%s/%s",dirpath,urlInfo->filename);
	rename(tmpbuf,buf);
	return 0;
}

int dir_downfile(char*dirpath,struct catUrlInfo **urlInfop,int urlInfoNum,int *renew_flag){
	int i;
	for(i=0;i<urlInfoNum;i++){
		if(pic_is_ok(dirpath,urlInfop[i]->filename) == 0){
			printf("to download %s\n",urlInfop[i]->url);
			while(down_pic(dirpath,urlInfop[i]) != 0){;
				sleep(1);
				printf("re download %s\n",urlInfop[i]->url);
				if(*renew_flag){
					return -1;
				}
			}
		}
	}
	return 0;
}



int dir_delete_file(char*dirpath,struct catUrlInfo **urlInfop,int urlInfoNum,int *renew_flag){
	urlInfop_g = urlInfop;
	urlInfoNum_g = urlInfoNum;
	delete_loop_dir(dirpath);
	return 0;
}


int dir_sync(char*dirpath,struct catUrlInfo **urlInfop,int urlInfoNum,int *renew_flag){
	int ret;
	changenum = 0;
	ret = dir_downfile(dirpath,urlInfop,urlInfoNum,renew_flag);
	if(ret < 0){
		return ret;
	}
	dir_delete_file(dirpath,urlInfop,urlInfoNum,renew_flag);
	printf("changenum=%d\n",changenum);
	return changenum;
}

