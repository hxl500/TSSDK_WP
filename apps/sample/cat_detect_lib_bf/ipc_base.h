#ifndef __IPC_BASE_H
#define __IPC_BASE_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#define CAT_ARRAY_LEN 512 //128

struct catInfo{
	char *data;
	char *name;
	char *sid;
}__attribute__((__packed__)); 

struct catNameInfo{
	char id[64];
	char name[128];
}__attribute__((__packed__)); 
struct catPicInfo{
	char id[64];
	int size;
	char data[CAT_ARRAY_LEN];

}__attribute__((__packed__)); 
struct cat_data{
	char id[64];
	int size;
	float data[CAT_ARRAY_LEN];
}__attribute__((__packed__)); 
#define PIC_TMP_PATH "/home/ipc_pro/pic/"
//#define PIC_TMP_PATH "/tmp/pic/"
#define CAT_DATA_MAX 30000
//#define PIC_TMP_DATA_PATH "/root/sd/picdata/"
//#define PIC_TMP_V_PATH "/root/sd/vdata/"
#define PIC_TMP_DATA_PATH "/home/ipc_pro/pic/picdata/"
#define PIC_TMP_V_PATH "/home/ipc_pro/pic/vdata/"

#ifdef __cplusplus
}
#endif

#endif

