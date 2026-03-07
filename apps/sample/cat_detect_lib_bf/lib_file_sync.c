#include "file_sync.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//#include "ipc_base.h"
#define SCSI_LOG printf
pthread_t					update_pic_pid;

static int renew_flag = 0;
static char*url_g = NULL;
static char*sn_g = NULL;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void destory_catUrlInfo(struct catUrlInfo **urlInfop ,int urlInfoNum);
#if 0
int  change_catName_info2(struct catPicInfo*data,int datanum, struct catNameInfo *cat_nameinfo,int namenum)
{
	return 0;
}
#endif


//
//{"code":"200","msg":"操作成功",
//"data":{"pets":
//[{"petId":"1c6ed6942b9f4ebea03c2cc1f2c8d952",
//"imagesUrl":
//["http://cat-dog-data-video.oss-cn-chengdu.aliyuncs.com/alUI3sCvjT1742369867.rgba"]}]}}
#define MAXURL 256
int file_sync(char*jsonstr,struct catUrlInfo ***urlInfopp,int *urlInfoNump){
	int ret = 0;
	cJSON *obj;
	char *p;
	cJSON *pets_obj;
	cJSON *pets_one_obj;
	//cJSON *pet_obj;
	cJSON *petId_obj;
	cJSON *code_obj;
	//cJSON *msg_obj;
	//cJSON *data_obj;
	struct catUrlInfo **urlInfop = NULL;
	*urlInfopp = (struct catUrlInfo **)malloc(256*sizeof(struct catUrlInfo *));
	urlInfop = *urlInfopp;
	if(urlInfop == NULL){
		return -6;
	}
	int urlInfoNum = 0;
	obj = cJSON_Parse(jsonstr);
	cJSON *imagesUrl_obj;
	cJSON *imagesUrl_one_obj;
	code_obj = cJSON_GetObjectItem(obj,"data_status");
	if(code_obj == NULL){
		printf("erro\n");
		ret = -1;
		goto exit;
	}else{
		if(code_obj->valuestring == NULL){
			ret = -2;
			goto exit;

		}
		printf("code=%s\n",code_obj->valuestring);

		if(strcmp(code_obj->valuestring,"true") != 0){
			ret = -3;
			goto exit;

		}
	}	
#if 0
	msg_obj = cJSON_GetObjectItem(obj,"msg");
	if(msg_obj == NULL){
		printf("erro\n");
	}else{
		printf("msg=%s\n",msg_obj->valuestring);
	}	
#endif
	pets_obj = cJSON_GetObjectItem(obj,"pets");
	if(pets_obj == NULL){
		printf("erro\n");
		ret = -5;
		goto exit;

	}else{
		printf("pets_obj ok\n");
		//printf("msg=%s\n",tmpobj->valuestring);
	}	
	int i;
	int j;
	for(i=0;i<50;i++){
		pets_one_obj = cJSON_GetArrayItem(pets_obj, i);
		if (pets_one_obj == NULL)
		{
		// ret = 0x80;
		break;
		}
		petId_obj = cJSON_GetObjectItem(pets_one_obj,"petId");
		if(petId_obj == NULL){
			printf("erro\n");
			continue;
		}else{
			printf("petId[%d]=%s\n",i,petId_obj->valuestring);
		//printf("msg=%s\n",tmpobj->valuestring);
		}
		imagesUrl_obj = cJSON_GetObjectItem(pets_one_obj,"photos");
		if(imagesUrl_obj == NULL){
			printf("imagesUrl_obj erro\n");
			continue;
		}else{
			//printf("petId[%d]=%s\n",i,petId_obj->valuestring);
		//printf("msg=%s\n",tmpobj->valuestring);
		}	
		if(urlInfoNum >= MAXURL){
			break;
		}
		for(j=0;j<50;j++){
			imagesUrl_one_obj = cJSON_GetArrayItem(imagesUrl_obj, j);
			if(imagesUrl_one_obj == NULL){
				//printf("imagesUrl_one_obj[%d]=NULL\n",j);
				break;
			}
			if(urlInfoNum >= MAXURL){
				break;
			}			
			urlInfop[urlInfoNum] = (struct catUrlInfo *)malloc(sizeof(struct catUrlInfo));
			p = strrchr(imagesUrl_one_obj->valuestring,'/');
			if(p == NULL){
				break;
			}
			p+=1;
			
			sprintf(urlInfop[urlInfoNum]->id,"%s",petId_obj->valuestring);
			urlInfop[urlInfoNum]->url = strdup(imagesUrl_one_obj->valuestring);
			urlInfop[urlInfoNum]->filename = strdup(p);
			//strcpy(urlInfop[urlInfoNum]->url,imagesUrl_one_obj->valuestring);
			printf("urlInfop[%d].id=%s\n",urlInfoNum,urlInfop[urlInfoNum]->id);
			printf("urlInfop[%d].id=%s\n",urlInfoNum,urlInfop[urlInfoNum]->url);
			urlInfoNum++;
			//printf("imagesUrl_one_obj[%d]=%s\n",j,imagesUrl_one_obj->valuestring);
		}
	}
exit:
	if(obj){
		cJSON_Delete(obj);
	}
	*urlInfoNump = urlInfoNum;
	return ret;

	
}
void destory_catUrlInfo(struct catUrlInfo **urlInfop ,int urlInfoNum){
	int i;
	for(i=0;i<urlInfoNum;i++){
		//printf("urlInfop[%d].id=%s\n",i,urlInfop[i]->id);
		//printf("urlInfop[%d].filename=%s\n",i,urlInfop[i]->filename);
		//printf("urlInfop[%d].url=%s\n",i,urlInfop[i]->url);
		if(urlInfop[i]->filename){
			free(urlInfop[i]->filename);
		}

		if(urlInfop[i]->url){
			free(urlInfop[i]->url);
		}

		free(urlInfop[i]);

	}
	if(urlInfop){
		free(urlInfop);
	}	
	return;
}
static char pic_path[128];

void set_pic_path(const char *path){
	if(path == NULL){
		printf("set_pic_path error, input is null\n");
		return;
	}
	printf("set_pic_path\n");
	start_file_sync_i4s_server();
	char tmpbuf[128];
#define WEBDAV_DIR_MODE  S_IRWXU | S_IRWXG | S_IRWXO
	sprintf(tmpbuf,"%s/%s",path,"pic");
	mkdir(tmpbuf, WEBDAV_DIR_MODE);
	//printf("mkdir %s\n",tmpbuf);
	memcpy(pic_path, tmpbuf, strlen(tmpbuf)+1);
	return;
}


int pic_renew(char*url,char*sn)
{
	int snflag = 0;
	int urlflag = 0;
	if(url == NULL){
		return -1;
	}
	if(sn == NULL){
		return -2;
	}
	pthread_mutex_lock(&mut);
	if(sn_g){
		if(strcmp(sn_g,sn) != 0){
			snflag = 1;
		}
	}else{
		snflag = 1;
	}
	if(urlflag){
		if(strcmp(url_g,url) != 0){
			urlflag = 1;
		}
	}else{
		urlflag = 1;
	}
	if(snflag){
		sn_g =strdup(sn);
	}
	if(urlflag){
		url_g =strdup(url);
	}

	renew_flag = 1;
	pthread_mutex_unlock(&mut);
	return 0;
}



char *https_get_json(char*url,char*sn){
	
	char *jsonbuf;
#if 0
	static int num = 0;
	if(num %2 == 1){
		jsonbuf = read_file("a.txt");
	}else{
		//jsonbuf = read_file("a.txt");
		jsonbuf = getjson(url,sn);
	}

	num++;
#endif
	jsonbuf = getjson(url,sn);
	return jsonbuf;
}
#if 1
struct catPicInfo *url2pic(char*dirpath,struct catUrlInfo**urlinfo,int urlInfoNum)
{
	struct catPicInfo *p;
	p = malloc(sizeof(struct catPicInfo)*urlInfoNum);
	if(p == NULL){
		printf("url2pic malloc error\n");
	}
	int i;
	for(i=0;i<urlInfoNum;i++){
		sprintf(p[i].data,"%s/%s",dirpath,urlinfo[i]->filename);
		sprintf(p[i].id,"%s",urlinfo[i]->id);
		p[i].size = 200704;
		printf("p[%d].data=%s\n",i,p[i].data);
		printf("p[%d].id=%s\n",i,p[i].id);
	}
	return p;
}
#endif
static void getdata_fromdisk(char**buf,int *len)
{
	char namefile[256];
	sprintf(namefile,"%s/%s",pic_path,"name.txt");
	struct stat st;
	if(stat(namefile, &st) != 0){
		return;
	}
	int newlen;
	FILE*fp;
	fp = fopen(namefile,"r");
	fread(&newlen, 4, 1, fp);
	if(newlen != st.st_size-4){
		fclose(fp);
		return;
	}
	int ret;
	char *newbuf;
	newbuf = malloc(newlen);
	ret = fread(newbuf, newlen, 1, fp);
	printf("ret=%d\n",ret);
	fclose(fp);
	*buf = newbuf;
	*len = newlen;
	return;
}
static void savedata_fromdisk(char *buf,unsigned char picnum,int len)
{
	FILE*fp;
	char namefile[256];
	sprintf(namefile,"%s/%s",pic_path,"name.txt");
	fp = fopen(namefile,"w");
	len = len+1;
	fwrite(&len,4,1,fp);
	fwrite(&picnum,1,1,fp);
	fwrite(buf,len-1,1,fp);
	fclose(fp);
	printf("sync savedata_fromdisk \n");
	return;
}

static void* pthread_pic_Process2(void* p){
	char *jsonstr;
	struct catUrlInfo **urlInfop;
	int urlInfoNum;
	int ret;
	int is_init = 0;
	char *catinfo = NULL;
	int catinfobuflen = 0;
	char picinfonum = 0;
	struct catPicInfo *picinfo = NULL;
	printf("pthread_pic_Process2\n");
;

	int i;
	while(1){
		if(pic_path == NULL){
			sleep(1);
			continue;
		}
		if(url_g == NULL || sn_g == NULL){

			sleep(1);
			continue;
		}
		if(is_init == 0){
			getdata_fromdisk(&catinfo,&catinfobuflen);
			if(catinfo != NULL){
#if 0
				nameinfonum = catinfo[0];
				if(nameinfonum){
					nameinfo = (struct catNameInfo *)&catinfo[1];
				}
#endif
				picinfonum = catinfo[0];
				if(picinfonum){
					picinfo = (struct catPicInfo *)&catinfo[1];
				}
				SCSI_LOG("picinfonum=%d\n",picinfonum);
				for(i=0;i<picinfonum;i++){
					SCSI_LOG("picinfo[%d]=%s\n",i,picinfo[i].id);
					SCSI_LOG("pic-path[%d]=%s\n",i,picinfo[i].data);
				}
				change_catName_info2(picinfo,picinfonum, NULL,0);
				free(catinfo);
				catinfo = NULL;
			}
			is_init = 1;
		}

		if(renew_flag == 0){
			sleep(1);
			continue;
		}
		renew_flag = 0;
		while(1){
			if(renew_flag){
				break;
			}
			jsonstr = https_get_json(url_g,sn_g);
			if(jsonstr == NULL){
				sleep(1);
				continue;				
			}
			if(file_sync(jsonstr,&urlInfop,&urlInfoNum) == 0){
				printf("to dir_sync\n");
				ret =dir_sync(pic_path,urlInfop,urlInfoNum,&renew_flag);
				printf("to dir_sync,ret=%d\n",ret);
				if(ret > 0){
					struct catPicInfo * catinfost;
					catinfost = url2pic(pic_path,urlInfop,urlInfoNum);
					change_catName_info2(catinfost,urlInfoNum, NULL,0);
					savedata_fromdisk((char*)catinfost,urlInfoNum,urlInfoNum*sizeof(struct catPicInfo));
					free(catinfost);
					sync();
					destory_catUrlInfo(urlInfop,urlInfoNum);
				}else{
					destory_catUrlInfo(urlInfop,urlInfoNum);
				}
				free(jsonstr);
				break;
			}else{
				printf("to get json\n");
				free(jsonstr);
				sleep(1);
				continue;
			}
		}
		
		
		
		
	}

	return NULL;
}
static char up = 0;
void start_file_sync_i4s_server(){
	printf("start_file_sync_i4s_server\n");
	if(up){
		return;
	}
	pthread_create(&update_pic_pid, 0, pthread_pic_Process2, NULL);
	up = 1;
	return;
}


#if 0
int main(){
#if 1
	//char *jsonbuf = read_file("a.txt");
	//int filelen = 0;
	//int lastlen = 0;
	//GetOtaBegin("http://cat-dog-data-video.oss-cn-chengdu.aliyuncs.com/alUI3sCvjT1742369867.rgba","pic/rgba.tmp",&filelen,&lastlen);

	//getjson("http://t-api.homerunsmart.com/open/petPic/device","C9310B8B:48252849C");
   // pthread_create(&update_pic_pid, 0, pthread_pic_Process2, NULL);
	set_pic_path("pic");
	pic_renew("http://t-api.homerunsmart.com/open/petPic/device","A9015601:A18DOQ1QA");
	sleep(10);
	//pic_renew("http://t-api.homerunsmart.com/open/petPic/device","C9310B8B:48252849C");
	while(1){
		//sleep(10);
		
		sleep(10);
		//pic_renew("http://t-api.homerunsmart.com/open/petPic/device","C9310B8B:48252849C");
	}

	//dirsync();
#endif
	return 0;
	
}
#endif
