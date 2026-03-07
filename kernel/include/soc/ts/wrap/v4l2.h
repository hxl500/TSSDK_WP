/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_V4L2_H__
#define __TS_WRAP_V4L2_H__

#include <media/v4l2-dev.h>

typedef void release_func(struct video_device *vdev);

int ts_video_get_minor(struct video_device *vdev);
struct device *ts_video_get_dev(struct video_device *vdev);
const char *ts_video_get_dev_kobj_name(struct video_device *vdev);
char *ts_video_get_name(struct video_device *vdev);
void ts_video_set_lock(struct video_device *vdev, struct mutex *lock);
void ts_video_set_tvnorms(struct video_device *vdev, v4l2_std_id tvnorms);
void ts_video_set_queue(struct video_device *vdev, struct vb2_queue *queue);
struct vb2_queue * ts_video_get_queue(struct video_device *vdev);
void ts_video_set_v4l2_dev(struct video_device *vdev, struct v4l2_device *v4l2_dev);
void ts_video_set_release(struct video_device *vdev, release_func *release);
void ts_video_set_dev_caps(struct video_device *vdev, u32 device_caps);
void ts_video_set_ioctl_ops(struct video_device *vdev, const struct v4l2_ioctl_ops *ioctl_ops);
spinlock_t *ts_video_get_fh_lock(struct video_device *vdev);
struct list_head *ts_video_get_fh_list(struct video_device *vdev);
void ts_video_set_fops(struct video_device *vdev, const struct v4l2_file_operations *fops);
void ts_video_set_dev_parent(struct video_device *vdev, struct device *dev_parent);

void *ts_video_get_drvdata(struct video_device *vdev);
void ts_video_set_drvdata(struct video_device *vdev, void *data);
void *ts_video_drvdata(struct file *file);
const char *ts_video_device_node_name(struct video_device *vdev);
int ts_video_is_registered(struct video_device *vdev);
int ts_video_register_device(struct video_device *vdev,
		int type, int nr);
bool ts_vb2_is_busy(struct vb2_queue *q);
void *ts_vb2_get_drv_priv(struct vb2_queue *q);
unsigned long ts_vb2_plane_size(struct vb2_buffer *vb, unsigned int plane_no);
void ts_vb2_set_plane_payload(struct vb2_buffer *vb,
				 unsigned int plane_no, unsigned long size);
struct v4l2_device *ts_alloc_st_v4l2_device(void);
char *ts_v4l2_dev_get_name(struct v4l2_device *dev);
void ts_v4l2_dev_set_name(struct v4l2_device *dev, char *name, int inst);
void ts_v4l2_dev_set_release(struct v4l2_device *dev,
	void (*release)(struct v4l2_device *v4l2_dev));
void ts_v4l2_dev_set_notify(struct v4l2_device *dev,
	void (*notify)(struct v4l2_subdev *sd,
			unsigned int notification, void *arg));
struct video_device *ts_alloc_st_video_device(void);
struct vb2_queue *ts_alloc_st_vb2_queue(void);

#endif

