/**
 * @file vp_jobqueue.h
 * @brief 工作任务队列, 支持多线程并发执行
 * @author 杨柳
 * @date 2020-03-01
 */

#ifndef VP_TOOLS_VP_JOBQUEUE_H
#define VP_TOOLS_VP_JOBQUEUE_H

typedef struct vp_job *vp_job_p;

/**
 * 创建工作任务
 *
 * @param func [in] 任务函数
 * @param arg [in] 任务函数参数
 *
 * @return vp_job_p
 * @retval !NULL 成功
 * @retval NULL 失败
 */
vp_job_p vp_job_create(void (*func)(void *), void *arg);

/**
 * 执行工作任务
 *
 * @param job [in] 工作任务句柄
 */
void vp_job_action(vp_job_p job);

/**
 * 销毁工作任务
 *
 * @param job [in] 工作任务句柄
 */
void vp_job_destroy(vp_job_p job);

typedef struct vp_jobqueue *vp_jobqueue_p;

/**
 * 创建工作任务队列
 *
 * @return vp_jobqueue_p
 * @retval !NULL 成功
 * @retval NULL 失败
 */
vp_jobqueue_p vp_jobqueue_create();

/**
 * 获取工作任务队列任务数
 *
 * @param jobqueue [in] 工作任务队列句柄
 *
 * @return 工作任务数
 * @retval >=0 成功
 * @retval <0 失败
 */
int vp_jobqueue_count(vp_jobqueue_p jobqueue);

/**
 * 等待工作任务队列
 *
 * @param jobqueue [in] 工作任务队列句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_jobqueue_wait(vp_jobqueue_p jobqueue);

/**
 * 唤醒所有等待工作任务队列的任务
 *
 * @param jobqueue [in] 工作任务队列句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_jobqueue_post_all(vp_jobqueue_p jobqueue);

/**
 * 清空工作任务队列
 *
 * @param jobqueue [in] 工作任务队列句柄
 * @note 删除所有任务, 但是不删除任务队列
 */
void vp_jobqueue_clear(vp_jobqueue_p jobqueue);

/**
 * 将工作任务加入工作任务队列
 *
 * @param jobqueue [in] 工作任务队列句柄
 * @param job [in] 工作任务句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_jobqueue_push(vp_jobqueue_p jobqueue, vp_job_p job);

/**
 * 从工作任务队列中取出一个工作任务
 *
 * @param jobqueue [in] 工作任务队列句柄
 *
 * @return vp_job_p
 * @retval !NULL 成功
 * @retval NULL 失败
 */
vp_job_p vp_jobqueue_pull(vp_jobqueue_p jobqueue);


/**
 * 销毁工作任务队列
 *
 * @param jobqueue [in] 工作任务队列句柄
 */
void vp_jobqueue_destroy(vp_jobqueue_p jobqueue);

#endif //VP_TOOLS_VP_JOBQUEUE_H
