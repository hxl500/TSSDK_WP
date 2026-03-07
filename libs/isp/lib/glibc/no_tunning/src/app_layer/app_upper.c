#include "app_layer.h"
#include "app_upper.h"
#include "metadata.h"
#include "device.h"

#define MAX_PENDING_CMDS 30
#define MAX_PROCESS_EVEVTS_ONCE 10

typedef enum _usr_cmd_id
{
    USR_CMD_CFG        = CMD_MAX,
	USR_CMD_EVENT,
} usr_cmd_id_e;

typedef struct _usr_cmd_s
{
	usr_cmd_id_e cmdId;
	void *data;
}usr_cmd_t;

typedef struct _app_ctrl_s{
	isp_status_e status;
	thread_os_mutex_t rx_lock;
	obj_queue_t* rx_queue;
	data_callback_func_t data_cb;
	event_callback_func_t evt_cb;
	request_callback_func_t req_cb;
	void *req_cb_arg;
	uint32_t pending_cmd_num;
	pthread_t vi_thread;
}app_ctrl_t;

static app_ctrl_t *app_ctrl = NULL;

int Isp_Control_Init()
{
	app_ctrl = MEM_OS_MALLOC(sizeof(*app_ctrl));
	if(app_ctrl == NULL)
		return ISP_RET_NOMEM;

	obj_queue_t *queue = Obj_Queue_Create();
	if(queue == NULL)
		return ISP_RET_NOMEM;

	app_ctrl->rx_queue = queue;
	pthread_mutex_init(&app_ctrl->rx_lock, NULL);
	
	app_ctrl->status = ISP_STATUS_UNINIT;
	return 0;
}

void Isp_Control_Deinit()
{
	if(app_ctrl == NULL)
		return;

	Obj_Queue_Destroy(&app_ctrl->rx_queue);
	MEM_OS_FREE(&app_ctrl);
}

void Isp_Register_Data_Callback(data_callback_func_t func)
{
	if(app_ctrl)
		app_ctrl->data_cb = func;
}

void Isp_Unregister_Data_Callback()
{
	if(app_ctrl)
		app_ctrl->data_cb = NULL;
}

void App_Layer_Upper_Data_Callback(metadata_t *meta, stream_buffer_t *buf)
{
	//TODO zhn, check how long the callback runs
	if(app_ctrl && app_ctrl->data_cb)
		app_ctrl->data_cb(meta, buf);
}

void Isp_Register_Event_Callback(event_callback_func_t func)
{
	if(app_ctrl)
		app_ctrl->evt_cb = func;
}

void Isp_Unregister_Event_Callback()
{
	if(app_ctrl)
		app_ctrl->evt_cb = NULL;
}

void App_Layer_Upper_Event_Callback(metadata_event_t *evt_p)
{
	//TODO zhn, check how long the callback runs
	if(app_ctrl && app_ctrl->evt_cb)
		app_ctrl->evt_cb(evt_p);
}

void Isp_Register_Request_Unblock_Callback(request_callback_func_t func, void *arg)
{
	if(app_ctrl)
	{
		app_ctrl->req_cb = func;
		app_ctrl->req_cb_arg = arg;
	}
}

void Isp_Unregister_Request_Callback()
{
	if(app_ctrl)
		app_ctrl->req_cb = NULL;
}

void App_Layer_Upper_Request_Fast_Callback()
{
	//TODO zhn, check how long the callback runs
	if(app_ctrl && app_ctrl->req_cb)
		app_ctrl->req_cb(app_ctrl->req_cb_arg);
}

int Isp_Metadata_Set(int cam_id, uint32_t offset, void *data, size_t size)
{
	isp_ret_t ret;
	app_config_t *config_p;
	config_p = App_Config_Acquire(cam_id);
	
	if(config_p->status == APP_CFG_STATUS_UNINITIALIZED)
		return ISP_RET_UNINITIALIZED;

	if(config_p->pMetadata == NULL)
		return ISP_RET_UNINITIALIZED;

	if(offset + size >= sizeof(metadata_t))
		return ISP_RET_OVERFLOW;

	ret = Metadata_Set(config_p->pMetadata, offset, data, size);
	if(ret != ISP_RET_SUCCESS)
		return ret;

	return ISP_RET_SUCCESS;
}

int Isp_Metadata_Get(int cam_id, uint32_t offset, void *data, size_t size)
{
	app_config_t *config_p;
	void *ptr;
	config_p = App_Config_Acquire(cam_id);

	if(config_p->status == APP_CFG_STATUS_UNINITIALIZED)
		return ISP_RET_UNINITIALIZED;

	if(config_p->pMetadata == NULL)
		return ISP_RET_UNINITIALIZED;

	if(offset + size >= sizeof(metadata_t))
		return ISP_RET_OVERFLOW;

	ptr = Metadata_Get(config_p->pMetadata, offset);
	if(ptr == NULL)
		return ISP_RET_INVALID;

	memcpy(data, ptr, size);
	return ISP_RET_SUCCESS;
}

static isp_ret_t App_Upper_Update_Config(void *data, app_config_t **config_pp)
{
	isp_ret_t res;
	app_config_t *new_config_p = data;
	app_config_t *config_p;
	int cam_id;

	cam_id = new_config_p->camId;
	config_p = App_Config_Acquire(cam_id);
	*config_pp = config_p;

	if (APP_CFG_STATUS_UNINITIALIZED == config_p->status)
	{
		config_p->needRestart = FALSE;
        memset(&config_p->event, 0, sizeof(metadata_event_t));

		config_p->pMetadata = Metadata_Create();
		if (NULL == config_p->pMetadata)
		{
			res = ISP_RET_NOMEM;
			APP_ERR("Error! res = %d", res);
			return res;
		}
	}
	else
	{
		int i;
		stream_info_t *info;
		stream_info_t *new_info;

		config_p->needRestart = new_config_p->needRestart;
		if(!config_p->needRestart)
		{
			for(i=0; i< BUF_STREAM_IDX_VIDEO; i++)
			{
				info = &config_p->streamCfg.outputStreamInfo[i];
				new_info = &new_config_p->streamCfg.outputStreamInfo[i];
				if((info->user.fmt != new_info->user.fmt)
						|| (info->user.height != new_info->user.height)
						|| (info->user.width != new_info->user.width))
					config_p->needRestart = TRUE;
			}

			if(config_p->advSettings.hdrMode != new_config_p->advSettings.hdrMode)
				config_p->needRestart = TRUE;
		}
	}

	config_p->camId = cam_id;
	config_p->streamCfg = new_config_p->streamCfg;
	config_p->advSettings = new_config_p->advSettings;
#if 0
	config_p->event = new_config_p->event;
	config_p->dumpSettings = new_config_p->dumpSettings;
	config_p->logCfg = new_config_p->logCfg;
	config_p->sleepSecond = new_config_p->sleepSecond;
#endif

	/*fixed config*/
	config_p->streamCfg.inputStreamNum = 0;
	strncpy(config_p->advSettings.algbind.bind[ALG_LIB_AEC].attr.name, "default", ALG_LIB_NAME_SIZE_MAX);
	strncpy(config_p->advSettings.algbind.bind[ALG_LIB_AWB].attr.name, "default", ALG_LIB_NAME_SIZE_MAX);
	strncpy(config_p->advSettings.algbind.bind[ALG_LIB_AFC].attr.name, "default", ALG_LIB_NAME_SIZE_MAX);

	for (u32 i = 0; i < config_p->streamCfg.outputStreamNum; i++)
	{
		config_p->streamCfg.outputStreamInfo[i].user.identity.camId    = config_p->camId;
		config_p->streamCfg.outputStreamInfo[i].user.identity.streamId = i;

		if (i == BUF_STREAM_IDX_PREVIEW)
		{
			config_p->streamCfg.outputStreamInfo[i].user.fmt        = STREAM_FMT_RGB888;
			config_p->streamCfg.outputStreamInfo[i].user.maxBufNum  = 0;
			config_p->streamCfg.outputStreamInfo[i].user.allocType  = STREAM_ALLOC_NONE;
			config_p->streamCfg.outputStreamInfo[i].user.usage      = STREAM_USAGE_PREVIEW;
		}
		else
		{
			config_p->streamCfg.outputStreamInfo[i].user.maxBufNum  = MAX_BUF_PER_STREAM;
			config_p->streamCfg.outputStreamInfo[i].user.allocType  = STREAM_ALLOC_CONTINUE_BUF;
			config_p->streamCfg.outputStreamInfo[i].user.usage      = STREAM_USAGE_VIDEO;
		}
	}

	/*update metadata*/
	if (APP_CFG_STATUS_UNINITIALIZED == config_p->status)
		config_p->status      = APP_CFG_STATUS_INITIALIZED;
	else
		config_p->status      = APP_CFG_STATUS_UPDATED;

	res = App_Config_Update_Metadata(config_p);
	if (ISP_RET_SUCCESS != res)
	{
		APP_ERR("Error! res = %d", res);
	}
	return ISP_RET_SUCCESS;
}

static isp_ret_t App_Upper_Update_Event(void* data, app_config_t **config_pp)
{
	isp_ret_t res;
    metadata_event_t* evt_p = data;
	uint32_t cam_id = evt_p->camId;
	app_config_t* config_p = App_Config_Acquire(cam_id);
	*config_pp = config_p;

	if (cam_id >= MAX_CAMERA)
	{
		res = ISP_RET_INVALID;
		APP_ERR("Error! res = %d", res);
		return res;
	}

	if (APP_CFG_STATUS_UNINITIALIZED == config_p->status)
	{
		res = ISP_RET_UNINITIALIZED;
		APP_ERR("Error! res = %d", res);
		return res;
	}

	evt_p->needRestart = FALSE;
	evt_p->status      = META_EVT_STATUS_PROCESSING;
	res = Metadata_Push_Event(config_p->pMetadata, evt_p);
	if (ISP_RET_SUCCESS != res)
	{
		APP_ERR("Error! res = %d", res);
	}
	else
	{
		APP_INFO("camId = %u, type = %u, evtId = %u, evtDirection = %u, evtData = %u, evtBuf = %p",
				evt_p->camId, evt_p->type, evt_p->evtId, evt_p->direction, evt_p->data, evt_p->buffer);
	}

	return res;
}

cmd_t *App_Layer_Upper_Config(void *pCmd, cmd_id_t *pcmdId, app_config_t **config_pp)
{
	usr_cmd_t *ucmd_p = pCmd;
	switch(ucmd_p->cmdId)
	{
	case USR_CMD_CFG:
		App_Upper_Update_Config(ucmd_p->data, config_pp);
		*pcmdId = CMD_SET;
		break;
	case USR_CMD_EVENT:
		App_Upper_Update_Event(ucmd_p->data, config_pp);
		*pcmdId = CMD_EVT;
		break;
	default:
		break;
	}

	MEM_OS_FREE(&pCmd);
	return NULL;
}

static void App_Layer_Status_Update(int cam_id)
{	
	isp_status_e ss = ISP_STATUS_UNINIT;
	app_config_t *config_p;
	device_t *dev_p;
	config_p = App_Config_Acquire(cam_id);
	dev_p = Device_Acquire(cam_id);

	if((config_p->status == APP_CFG_STATUS_UNINITIALIZED) && 
			(dev_p->status == DEV_STATUS_UNINITIALIZED))
		ss = ISP_STATUS_UNINIT;
	else if((config_p->status >= APP_CFG_STATUS_INITIALIZED) && 
			(dev_p->status <= DEV_STATUS_INITIALIZED))
		ss = ISP_STATUS_INIT;
	else if((config_p->status >= APP_CFG_STATUS_INITIALIZED) && 
			(dev_p->status > DEV_STATUS_INITIALIZED))
		ss = ISP_STATUS_START;

	app_ctrl->status = ss;
}


int Isp_Cmd_Push(cmd_t *cmd)
{
	cmd_t* pCmd;
	if(app_ctrl == NULL)
		return ISP_RET_UNINITIALIZED;

	if(app_ctrl->pending_cmd_num > MAX_PENDING_CMDS)
		return ISP_RET_OVERFLOW;

	pCmd = MEM_OS_MALLOC(sizeof(cmd_t));
	if(pCmd == NULL)
		return ISP_RET_NOMEM;

	if((pCmd->cmdId > CMD_MAX) || (pCmd->paramLen >= APP_CONFIG_CMDLINE_MAX_LEN)
			|| pCmd->sParam[pCmd->paramLen] != '\0')
		return ISP_RET_INVALID;

	memcpy(pCmd, cmd, sizeof(cmd_t));

	pthread_mutex_lock(&app_ctrl->rx_lock);
	app_ctrl->rx_queue->Push(app_ctrl->rx_queue, pCmd);
	app_ctrl->pending_cmd_num++;
	pthread_mutex_unlock(&app_ctrl->rx_lock);

	APP_INFO("Receive App Cmd(%u)", pCmd->cmdId);
	return 0;
}

cmd_t * App_Layer_Cmd_Pop(cmd_id_t *pcmdId, app_config_t** ppAppConfig)
{
	cmd_t* pCmd;
	if(app_ctrl == NULL)
		return NULL;

	pthread_mutex_lock(&app_ctrl->rx_lock);
	pCmd = app_ctrl->rx_queue->Pop(app_ctrl->rx_queue);
	if(pCmd != NULL)
		app_ctrl->pending_cmd_num--;
	pthread_mutex_unlock(&app_ctrl->rx_lock);

	/*quit, if queue is empty or not user-defined cmd*/
	if(pCmd)
	{
		APP_INFO("Proc App Cmd(%u)", pCmd->cmdId);

		/*if get user-defined cmd, repeat pop and process*/
		if(pCmd->cmdId >= CMD_MAX)
		{
			pCmd = App_Layer_Upper_Config(pCmd, pcmdId, ppAppConfig);
		}
	}
	return pCmd;
}

int Isp_Event_Send(metadata_event_t *evt_p)
{
	int cmd_id = evt_p->camId;
	usr_cmd_t *cmd_p;

	if(cmd_id > MAX_CAMERA)
		return ISP_RET_INVALID;

	cmd_p = MEM_OS_MALLOC(sizeof(usr_cmd_t) + sizeof(metadata_event_t));
	if(cmd_p == NULL)
		return ISP_RET_NOMEM;

	cmd_p->cmdId = USR_CMD_EVENT;
	cmd_p->data = (cmd_p + 1);
	memcpy(cmd_p->data, evt_p, sizeof(metadata_event_t));

	pthread_mutex_lock(&app_ctrl->rx_lock);
	app_ctrl->rx_queue->Push(app_ctrl->rx_queue, cmd_p);
	app_ctrl->pending_cmd_num++;
	pthread_mutex_unlock(&app_ctrl->rx_lock);

	APP_INFO("Recv Event(%d), cam(%d)", evt_p->evtId, evt_p->camId);
	return ISP_RET_SUCCESS;
}

int Isp_Config(app_config_t *config_p)
{
	int cmd_id = config_p->camId;
	usr_cmd_t *cmd_p;

	if(cmd_id > MAX_CAMERA)
		return ISP_RET_INVALID;

	/*Check cfg*/
	if(config_p->streamCfg.fps > MAX_FPS)
		return ISP_RET_INVALID;
	if(config_p->streamCfg.outputStreamNum > 2)
		return ISP_RET_INVALID;

	cmd_p = MEM_OS_MALLOC(sizeof(usr_cmd_t) + sizeof(app_config_t));
	if(cmd_p == NULL)
		return ISP_RET_NOMEM;

	cmd_p->cmdId = USR_CMD_CFG;
	cmd_p->data = (cmd_p + 1);
	memcpy(cmd_p->data, config_p, sizeof(app_config_t));

	pthread_mutex_lock(&app_ctrl->rx_lock);
	app_ctrl->rx_queue->Push(app_ctrl->rx_queue, cmd_p);
	app_ctrl->pending_cmd_num++;
	pthread_mutex_unlock(&app_ctrl->rx_lock);

	APP_INFO("Recv Cfg(%d), cam(%d)", config_p->streamCfg.fps, config_p->camId);
	return ISP_RET_SUCCESS;
}

int Isp_Start()
{
	cmd_t cmd;
	cmd.cmdId = CMD_START;
	return Isp_Cmd_Push(&cmd);
}

int Isp_Stop()
{
	cmd_t cmd;
	cmd.cmdId = CMD_STOP;
	return Isp_Cmd_Push(&cmd);
}

int Isp_Exit()
{
	cmd_t cmd;
	cmd.cmdId = CMD_EXIT;
	return Isp_Cmd_Push(&cmd);
}

static void *vi_process(void* data)
{
	char *input_file = data;
	app_desc_t app_desc;
    memset(&app_desc, 0, sizeof(app_desc_t));
	
	if(input_file)
	{
		if(strlen(input_file) >= 128)
			return NULL;

    	memcpy(app_desc.cfgFile.sFilename, input_file, strlen(input_file) + 1);
	}

    App_Layer_Process(&app_desc);

	app_ctrl->status = ISP_STATUS_EXIT;
	return NULL;
}

int Isp_Process(char *input_file)
{
	app_desc_t app_desc;
    memset(&app_desc, 0, sizeof(app_desc_t));
	
	if(input_file)
	{
		if(strlen(input_file) >= 128)
			return ISP_RET_INVALID;

    	memcpy(app_desc.cfgFile.sFilename, input_file, strlen(input_file) + 1);
	}

    App_Layer_Process(&app_desc);
	app_ctrl->status = ISP_STATUS_EXIT;
	return ISP_RET_SUCCESS;
}

int Isp_Open(char *input_file)
{
	int rc;
	isp_ret_t res;
	res = Isp_Control_Init();
	if(res != ISP_RET_SUCCESS)
	{
		APP_ERR("Error! res = %d", res);
		return res;
	}

	rc = pthread_create(&app_ctrl->vi_thread, NULL, vi_process, input_file);
	if (rc < 0)
	{
		APP_ERR("Error! rc = %d", rc);
		return ISP_RET_ERROR;
	}

	return ISP_RET_SUCCESS;
}

/*Return value
 * ==0: Uninitialized
 * >=1: Initialized
 */
isp_status_e Isp_Status(int cam_id)
{
	if(app_ctrl)
	{
		if(app_ctrl->status != ISP_STATUS_EXIT)
			App_Layer_Status_Update(cam_id);

		return app_ctrl->status;
	}

	return ISP_STATUS_UNINIT;
}

