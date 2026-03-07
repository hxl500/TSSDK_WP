#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include "wifi_log.h"
#include <string.h>
#include <errno.h>
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
#include <signal.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <math.h>

#include "arrr_diff.h"

static struct catPicInfo *g_picdata =NULL;
static int cat_pic_num = 0;

static struct cat_data *g_catdata =NULL;
static struct cat_data *g_catdata2 =NULL;

static int cat_data_num = 0;
static struct catNameInfo *g_cat_nameinfo = NULL;
static int g_cat_nameinfo_num = 0;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static float SimilarityThres = 0.4;
static ALG_CatDetect_DET_PARAM_S det_param_arrar;
static int isgray = 0;
TS_VOID CatSetGrayMode(TS_CHAR flag)//设置夜间模式,0表示白天，1表示夜间
{
	printf("CatSetGrayMode=%d\n",flag);
	if(flag){
		isgray = 1;
	}else{
		isgray = 0;
	}
	
}
TS_CHAR CatGetGrayMode(TS_VOID)//获取夜间模式,0表示白天，1表示夜间
{
	return isgray;
}


static float compute_l2_norm(const float *arr) {
    float sum = 0;
	 float sumreturn = 0;
    for (int i = 0; i < 128; i++) {
        sum += arr[i] * arr[i];
    }
	//printf("sum=%f\n",sum);
	sumreturn = sqrt(sum);
	//printf("sumreturn=%f\n",sumreturn);
    return sumreturn;
}


void normalize_array(float *arr) {
	return;
#if 1
    float l2_norm = compute_l2_norm(arr);

    if (l2_norm > 0) {
        for (int i = 0; i < 128; i++) {
            arr[i] /= l2_norm;
        }
    }
#endif
}
static float cosine_similarity(float *a, float *b, int size){
	float dot_product =0.0;
	float norm_a=0.0;
	float norm_b=0.0;
	//Calculate dot product and norms
	for(int i=0;i<size; i++){
		dot_product += a[i]* b[i];
		norm_a += a[i]* a[i];
		norm_b += b[i]* b[i];
	}
	// Calculate magnitudes
	norm_a= sqrt(norm_a);
	norm_b=sqrt(norm_b);
	// Check for zero vectors to avoid division by zero
	if(norm_a==0.0|| norm_b == 0.0){
		return 0.0;
	}
	// Return cosine similarity
	return dot_product /(norm_a*norm_b);
}

static float compute_l2_distance(const float *arr1, const float *arr2) {
    float sum = 0.0;
	float sum_return = 0.0;
    for (int i = 0; i < 128; i++) {
        float diff = arr1[i] - arr2[i];
		//printf("sum[%d]=%f\n",i,sum);
        sum += diff * diff;  // Squaring the difference and adding to sum
    }
	//printf("compute_l2_distance sum=%f\n",sum);
	sum_return = sqrt(sum);
	//printf("compute_l2_distance sum_return=%f\n",sum_return);
    return sum_return;
}


float compare_vector(float *dst,float *src){
	float ret =compute_l2_distance(dst,src);

	printf("compare_vector ret2=%f\n",ret);
	return ret;
	//return -1;

}
float Simi = 0;
void set_simi(float data)
{
	Simi = data;
}

float get_simi()
{
	return Simi;
}
struct catNameInfo * lib_nameInfo = NULL;

static struct catNameInfo * get_catNameInfo(float *src){
	if(g_catdata ==NULL || cat_data_num == 0){
		return NULL;
	}
	if(g_cat_nameinfo ==NULL || g_cat_nameinfo_num == 0){
		//return NULL;
	}
	if(lib_nameInfo == NULL)
	{
		lib_nameInfo = (struct catNameInfo*)malloc(sizeof(struct catNameInfo));
		memset(lib_nameInfo,0,sizeof(struct catNameInfo));
	}
	//printf("begin2\n");
	int i;
	int j;
	float ret;
	float retmin= 100;
	float retmax= 0;
	int minnum = 1000;
	 
	TS_ALG_CatDetect_GetParam(&det_param_arrar);
	if(isgray){
		SimilarityThres = det_param_arrar.SimilarityThres_Night;
	}else{
		SimilarityThres = det_param_arrar.SimilarityThres_Day;
	}
	for(i=0;i<cat_data_num;i++){
		//printf("i=%d\n",i);
		float ptmp[CAT_ARRAY_LEN];
		printf("isgray=%d\n",isgray);
		if(isgray){
			for(j=0;j<CAT_ARRAY_LEN;j++){
				ptmp[j] = g_catdata2[i].data[j];
			}			
		}else{
			for(j=0;j<CAT_ARRAY_LEN;j++){
				ptmp[j] = g_catdata[i].data[j];
			}
		}
#if 0
		//memcpy(ptmp,g_catdata[i].data,sizeof(ptmp));
		printf("ptmp data int\n");
		for(j=0;j<128;j++){
			printf("%d ",g_catdata[i].data[j]);
		}
		printf("\n");
#endif
#if 0
		printf("ptmp data\n");
		for(j=0;j<128;j++){
			printf("%f ",ptmp[j]);
		}
		printf("\n");
#endif
		normalize_array(ptmp);
#if 0
		printf("ptmp data2\n");
		for(j=0;j<128;j++){
			printf("%f ",ptmp[j]);
		}
		printf("\n");
#endif
		#if 0
		ret = compare_vector(ptmp,src);
		printf("===============id=%d,ret=%f,retmin=%f\n",g_catdata[i].id,ret,retmin);
		if(ret < retmin && ret <= SimilarityThres){
			retmin = ret;
			minnum = i;
			printf("minnum=%d\n",minnum);
		}
		
		#else
		ret = cosine_similarity(ptmp,src,CAT_ARRAY_LEN);
		printf("===============id=%s,ret=%f,retmin=%f\n",g_catdata[i].id,ret,retmin);
		if(ret > retmax){
			retmax = ret;
			minnum = i;
			printf("minnum=%d\n",minnum);
		}

		#endif
#if 0
		//int *p = (int *)(g_catdata[i].data[0]);
		if(compare_vector(ptmp,src) == 0){
			for(j=0;j<g_cat_nameinfo_num;j++){
				printf("i=%d,j=%d,id=%d,tid=%d\n",i,j,g_catdata[i].id,g_cat_nameinfo[j].id);
				if(g_catdata[i].id == g_cat_nameinfo[j].id){
					return &g_cat_nameinfo[i];
				}
			}
			return NULL;
		}
#endif
	}
	printf("=========minnum=%d,retmax=%f\n",minnum,retmax);
	//if(minnum == 1000 || retmin > 0.51){
//	if(minnum == 1000 || retmin > 0.8){
//		return NULL;
//	}
	set_simi(retmax);
	if(minnum == 1000 || retmax < SimilarityThres){
		return NULL;
	}
	strcpy(lib_nameInfo->id,g_catdata[minnum].id);
	//lib_nameInfo->id = g_catdata[minnum].id;
	printf("id=%s,tid=%s\n",g_catdata[minnum].id,lib_nameInfo->id);
	return lib_nameInfo;
}
extern int rsn_detect_file(char *src_file,int *result);
extern int rsn_detect_file2(char *src_file,float *result,float *result2);

#if 0
static int wifidata_file(char*buf,size_t size,char*file){
    //char head[256];
    FILE*fp;
	printf("file=%s\n",file);
    fp = fopen(file,"w");
	if(fp == NULL){
		return -1;
	}
	int tmplen = 0;
	while(size-tmplen> 0){
		printf("tmplen=%d\n",tmplen);
		if(size-tmplen > 512){
			 fwrite(&buf[tmplen],1,512,fp);
			 tmplen+=512;
		}else{
			 fwrite(&buf[tmplen],1,size-tmplen,fp);
			 tmplen+=size-tmplen;
		}
	}
    fclose(fp);
    return 0;
}
#endif
char picpath_head[128] = {0};
void set_picpath_head(char *path)
{
	//memcpy(picpath_head,path,strlen(path));
	strcpy(picpath_head,path);
}
static int pic2data(struct cat_data *cat_data, struct catPicInfo *picdata,struct cat_data *cat_data2){
	char picpath[128] = {0};
	memset(picpath,0,128);
	#if 1 //this set path
	strcpy(picpath,picpath_head);
	strcat(picpath,picdata->data);
	#else
	strcpy(picpath,picdata->data);
	#endif
	float pp[CAT_ARRAY_LEN];
	float pp2[CAT_ARRAY_LEN];
	memset(pp,0,sizeof(pp));
	memset(pp2,0,sizeof(pp2));
	rsn_detect_file2(picpath,pp,pp2);
	//int j;
#if 0//lx test
	printf("111rsn_detect_file=%s\n",picpath);
	for(j=0;j<128;j++){
		printf("%f ",pp[j]);
	}
	printf("\n");
#endif
	memcpy(cat_data->data,pp,sizeof(pp));
	cat_data->size = picdata->size;
	strcpy(cat_data->id,picdata->id);
	
	memcpy(cat_data2->data,pp2,sizeof(pp2));
	cat_data2->size = picdata->size;
	strcpy(cat_data2->id,picdata->id);

#if 0
	strcpy(picpath,PIC_TMP_DATA_PATH);
	strcat(picpath,picdata->data);
	strcat(picpath,".dat");
	printf("data=%s\n",picpath);
	wifidata_file((char*)&pp[0],128*4,picpath);
#endif
	return 0;
#if 0
	static struct catPicInfo *g_picdata =NULL;
	static int cat_pic_num = 0;


    char picpath[128] = {0};
	struct catPicInfo *picinfo = (struct catPicInfo*)malloc(Cfg->picnum*sizeof(struct catPicInfo));
	int i;
	for(i=0;i<Cfg->picnum;i++){
		strncpy(picinfo[i].data,Cfg->pic[i].petpic,sizeof(picinfo[i].data));
		picinfo[i].id = Cfg->pic[i].petid;
		picinfo[i].size = Cfg->pic[i].len;
	}
	change_catName_info2(picinfo,Cfg->picnum, (struct catNameInfo*)local_namecfg.data,local_namecfg.namenum);
	free(picinfo);
#if 1

    struct cat_data* result_data = (struct cat_data*)malloc(Cfg->picnum*sizeof(struct cat_data));

    for(int i = 0;i<Cfg->picnum;i++){
        printf("petid3111:%d,petpic:%s,len:%d\n",Cfg->pic[i].petid,Cfg->pic[i].petpic,Cfg->pic[i].len);
        memset(picpath,0,128);
        strcpy(picpath,PIC_TMP_PATH);
        strcat(picpath,Cfg->pic[i].petpic);
	int *p = (int *)(result_data[i].data[0]);
		int pp[128];
		int cflag = 0;
		while(1){
			int pp2[128];
			memset(pp2,0,sizeof(pp2));
        	rsn_detect_file(picpath,pp2);
			int j;
			for(j=0;j<128;j++){
				printf("%d ",pp2[j]);
			}
			printf("\n");
			if(cflag ==1 && memcmp(pp,pp2,sizeof(pp2))!= 0){
				sleep(1000);
			}
			memcpy(pp,pp2,sizeof(pp2));
			cflag = 1;

		}
        memset(picpath,0,128);
        strcpy(picpath,PIC_TMP_DATA_PATH);



        strcat(picpath,Cfg->pic[i].petpic);
		strcat(picpath,".dat");
		wifidata_file((char*)&pp[0],128*4,picpath);

	//	#define PIC_TMP_DATA_PATH "/root/sd/picdata"
//#define PIC_TMP_V_PATH "/root/sd/vdata"
		//test_crop_resize_detect();
		printf("IpcSetPetPicCfg=%s",picpath);
		int j;
		for(j=0;j<128;j++){
			printf("%d ",pp[j]);
		}
		printf("\n");

		//normalize_array((float*)pp);
		memcpy(result_data[i].data,pp,sizeof(result_data[i].data));
		int k;
		printf("IpcSetPetPicCfg2=%s",picpath);
		for(k=0;k<128;k++){
			printf("%d ",result_data[i].data[k]);
		}
		printf("\n");

		printf("IpcSetPetPicCfg end\n");
		//test_crop_resize_detect();
        result_data[i].id = Cfg->pic[i].petid;
        result_data[i].size = Cfg->pic[i].len;
    }
    change_catName_info(result_data,Cfg->picnum, (struct catNameInfo*)local_namecfg.data,local_namecfg.namenum);
    free(result_data);
#endif

    return 0;
#endif

}


int  get_catName_id(float *src,char*idstr){
	struct catNameInfo *p;

	int i;
	//int j;
	//printf("begin2 get_catName_id,cat_data_num=%d\n",cat_data_num);
#if 0
	for(i=0;i<cat_data_num;i++){
		printf("g_catdata[%d]=begin\n",i);
		for(j=0;j<128;j++){
			printf("%d ",g_catdata[i].data[j]);
		}
		printf("\n");
	}
#endif
	printf("begin1 get_catName_id,cat_data_num=%d\n",cat_data_num);
	float srctmp[CAT_ARRAY_LEN];
	for(i=0;i<CAT_ARRAY_LEN;i++){
		srctmp[i] = src[i];
	}
	//memcpy(srctmp,src,128*4);
#if 0
	printf("vdata int\n");
	for(i=0;i<128;i++){
		printf("%d ",src[i]);
	}
	printf("\n");
#endif
#if 0
	printf("vdata\n");
	for(i=0;i<128;i++){
		printf("%f ",srctmp[i]);
	}
	printf("\n");
#endif
	normalize_array(srctmp);
#if 0
	printf("vdata2\n");
	for(i=0;i<128;i++){
		printf("%f ",srctmp[i]);
	}
	printf("\n");
#endif
	pthread_mutex_lock(&mut);
	if(g_picdata && cat_pic_num){
		cat_data_num = cat_pic_num;
		g_catdata = (struct cat_data *)malloc(sizeof(struct cat_data)*cat_data_num);
		g_catdata2 = (struct cat_data *)malloc(sizeof(struct cat_data)*cat_data_num);
		for(i=0;i<cat_data_num;i++){
			//while(1){
			printf("g_catdata i=%d\n",i);
			pic2data(&g_catdata[i], &g_picdata[i], &g_catdata2[i]);
			

			//}
		}
		free(g_picdata);
		g_picdata = NULL;
		cat_pic_num = 0;
	}
	p = get_catNameInfo(srctmp);
	if(p != NULL){
		strcpy(idstr,p->id);
		printf("idstr=%s\n",idstr);
		pthread_mutex_unlock(&mut);
		return 0;
	}
	strcpy(idstr,"0");
	printf("idstr=%s\n",idstr);
	pthread_mutex_unlock(&mut);
	return 0;
}
int get_catName_name(int id, char*name)
{
#if 0
	int j;
	printf("get_catName_name=%d\n",id);
	pthread_mutex_lock(&mut);
	for(j=0;j<g_cat_nameinfo_num;j++){
		if(g_cat_nameinfo[j].id == id){
			printf("name=%s\n",g_cat_nameinfo[j].name);
			strcpy(name,g_cat_nameinfo[j].name);
			pthread_mutex_unlock(&mut);
			return 0;
		}
	}
	pthread_mutex_unlock(&mut);
	printf("get_catName_name=%d,-1\n",id);
#endif
	return -1;

}

int  change_catName_info(struct cat_data*data,int datanum, struct catNameInfo *cat_nameinfo,int namenum)
{
	pthread_mutex_lock(&mut);
	if(g_catdata){
		free(g_catdata);
	}
	if(data && datanum){
		g_catdata = (struct cat_data*)malloc(sizeof(struct cat_data)*datanum);
		memcpy(g_catdata,data,sizeof(struct cat_data)*datanum);
		cat_data_num = datanum;

		int j;
		int i;
		for(i=0;i<cat_data_num;i++){
			printf("g_catdata[%d]=begin\n",i);
			for(j=0;j<128;j++){
				printf("%f ",g_catdata[i].data[j]);
			}
			printf("\n");
		}


	}else{
		g_catdata = NULL;
		cat_data_num = 0;
	}


	if(g_cat_nameinfo){
		free(g_cat_nameinfo);
	}
	if(cat_nameinfo && namenum){
		g_cat_nameinfo = (struct catNameInfo*)malloc(sizeof(struct catNameInfo)*namenum);
		memcpy(g_cat_nameinfo,cat_nameinfo,sizeof(struct catNameInfo)*namenum);
		g_cat_nameinfo_num = namenum;
	}else{
		g_cat_nameinfo = NULL;
		g_cat_nameinfo_num = 0;
	}
	pthread_mutex_unlock(&mut);
	return 0;
}
int  change_catName_info2(struct catPicInfo*data,int datanum, struct catNameInfo *cat_nameinfo,int namenum)
{
	pthread_mutex_lock(&mut);
	if(g_picdata){
		free(g_picdata);
		g_picdata = NULL;

	}
	cat_data_num = 0;
	int i;
	printf("IpcSetPetPicCfg 1\n");
	if(data && datanum){
		g_picdata = (struct catPicInfo*)malloc(sizeof(struct catPicInfo)*datanum);
		memcpy(g_picdata,data,sizeof(struct catPicInfo)*datanum);
		cat_pic_num = datanum;
		for(i=0;i<cat_pic_num;i++){
			printf("g_picdata[%d]=%s,id=%s\n",i,g_picdata[i].data,g_picdata[i].id);
		}
	}else{
		g_picdata = NULL;
		cat_pic_num = 0;
	}
	printf("IpcSetPetPicCfg 2\n");
	if(g_catdata){
		free(g_catdata);
		g_catdata = NULL;
	}
	if(g_catdata2){
		free(g_catdata2);
		g_catdata2 = NULL;
	}

	cat_data_num= 0;

	if(g_cat_nameinfo){
		free(g_cat_nameinfo);
		g_cat_nameinfo = NULL;
	}
	printf("IpcSetPetPicCfg 3\n");
#if 1
	g_cat_nameinfo_num = 0;
	if(cat_nameinfo && namenum){
		g_cat_nameinfo = (struct catNameInfo*)malloc(sizeof(struct catNameInfo)*namenum);
		memcpy(g_cat_nameinfo,cat_nameinfo,sizeof(struct catNameInfo)*namenum);
		g_cat_nameinfo_num = namenum;
		for(i=0;i<g_cat_nameinfo_num;i++){
			printf("g_cat_nameinfo[%d]=%s,id=%s\n",i,g_cat_nameinfo[i].name,g_cat_nameinfo[i].id);
		}

	}else{
		g_cat_nameinfo = NULL;
		g_cat_nameinfo_num = 0;
	}
#endif
	printf("IpcSetPetPicCfg 4\n");

	pthread_mutex_unlock(&mut);
	return 0;
}


#if 0
int main(){
	int src[128];
	int ret;
	printf("begin\n");
	memset(src,0,128*4);
	int data_num;
	struct cat_data *catdata;
	int cat_nameinfo_num;
	struct catNameInfo *cat_nameinfo;
	data_num=10;
	catdata = malloc(data_num*sizeof(struct cat_data));
	int i;
	memset(catdata ,0,data_num*sizeof(struct cat_data));
	for(i=0;i<9;i++){
		memset(catdata[i].data,100,128*4);
	}
	catdata[9].id = 100;

	cat_nameinfo_num=10;
	cat_nameinfo = malloc(cat_nameinfo_num*sizeof(struct catNameInfo));
	memset(cat_nameinfo ,0,cat_nameinfo_num*sizeof(struct catNameInfo));
	for(i=0;i<9;i++){
		cat_nameinfo[i].id = i;
	}
	cat_nameinfo[9].id = 100;
	change_catName_info(catdata,data_num,cat_nameinfo,cat_nameinfo_num);
	ret = get_catName_id(src);
	printf("end,ret=%d\n",ret);
	return 0;
}
#endif




