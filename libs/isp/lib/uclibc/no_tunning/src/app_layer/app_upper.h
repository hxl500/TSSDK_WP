#ifndef __APP_UPPER_H__
#define __APP_UPPER_H__

#include "stream_types.h"
#include "app_config.h"

typedef enum __isp_status{
	ISP_STATUS_UNINIT = 0,
	ISP_STATUS_INIT,
	ISP_STATUS_START,
	ISP_STATUS_EXIT,
}isp_status_e;

typedef void (*data_callback_func_t)(metadata_t *meta, stream_buffer_t* pData);
typedef void (*event_callback_func_t)(metadata_event_t *ack);
typedef void (*request_callback_func_t)(void *arg);

int Isp_Cmd_Push(cmd_t *cmd);
cmd_t * App_Layer_Cmd_Pop(cmd_id_t *pcmdId, app_config_t **ppAppConfig);

int Isp_Control_Init();
void Isp_Control_Deinit();

void Isp_Register_Data_Callback(data_callback_func_t func);
void Isp_Unregister_Data_Callback();
void App_Layer_Upper_Data_Callback(metadata_t* meta, stream_buffer_t *buf);

void Isp_Register_Event_Callback(event_callback_func_t func);
void Isp_Unregister_Event_Callback();
void App_Layer_Upper_Event_Callback(metadata_event_t *ack);

void Isp_Register_Request_Unblock_Callback(request_callback_func_t func, void *arg);
void Isp_Unregister_Request_Callback();
void App_Layer_Upper_Request_Fast_Callback();

int Isp_Metadata_Set(int cam_id, uint32_t offset, void *data, size_t size);
int Isp_Metadata_Get(int cam_id, uint32_t offset, void *data, size_t size);

int Isp_Open(char *input_file);
int Isp_Start();
int Isp_Stop();
int Isp_Exit();
int Isp_Process(char *input_file);
isp_status_e Isp_Status(int cam_id);
int Isp_Config(app_config_t *config_p);
int Isp_Event_Send(metadata_event_t *evt_p);

#endif
