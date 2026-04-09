#include <linux/module.h>
#include <linux/device.h>
#include <media/v4l2-dev.h>
#include <media/videobuf2-v4l2.h>
#include <media/v4l2-device.h>
#include <linux/slab.h>
#include <linux/slab.h>
#include <soc/ts/wrap/v4l2.h>


int ts_video_get_minor(struct video_device *vdev)
{	
	return vdev->minor;
}
EXPORT_SYMBOL_GPL(ts_video_get_minor);

struct device *ts_video_get_dev(struct video_device *vdev)
{
	return &vdev->dev;
}
EXPORT_SYMBOL_GPL(ts_video_get_dev);

const char *ts_video_get_dev_kobj_name(struct video_device *vdev)
{
	return vdev->dev.kobj.name;
}
EXPORT_SYMBOL_GPL(ts_video_get_dev_kobj_name);

char *ts_video_get_name(struct video_device *vdev)
{
	return vdev->name;
}
EXPORT_SYMBOL_GPL(ts_video_get_name);

void ts_video_set_lock(struct video_device *vdev, struct mutex *lock)
{
	vdev->lock = lock;
}
EXPORT_SYMBOL_GPL(ts_video_set_lock);

void ts_video_set_tvnorms(struct video_device *vdev, v4l2_std_id tvnorms)
{
	vdev->tvnorms = tvnorms;
}
EXPORT_SYMBOL_GPL(ts_video_set_tvnorms);

void ts_video_set_queue(struct video_device *vdev, struct vb2_queue *queue)
{
	vdev->queue = queue;
}
EXPORT_SYMBOL_GPL(ts_video_set_queue);

struct vb2_queue * ts_video_get_queue(struct video_device *vdev)
{
	return vdev->queue;
}
EXPORT_SYMBOL_GPL(ts_video_get_queue);

void ts_video_set_v4l2_dev(struct video_device *vdev, struct v4l2_device *v4l2_dev)
{
	vdev->v4l2_dev = v4l2_dev;
}
EXPORT_SYMBOL_GPL(ts_video_set_v4l2_dev);


void ts_video_set_release(struct video_device *vdev, release_func *release)
{
	vdev->release = release;
}
EXPORT_SYMBOL_GPL(ts_video_set_release);

void ts_video_set_dev_caps(struct video_device *vdev, u32 device_caps)
{
	vdev->device_caps = device_caps;
}
EXPORT_SYMBOL_GPL(ts_video_set_dev_caps);


void ts_video_set_ioctl_ops(struct video_device *vdev, const struct v4l2_ioctl_ops *ioctl_ops)
{
	vdev->ioctl_ops = ioctl_ops;
}
EXPORT_SYMBOL_GPL(ts_video_set_ioctl_ops);


void ts_video_set_dev_parent(struct video_device *vdev, struct device *dev_parent)
{
	vdev->dev_parent = dev_parent;
}
EXPORT_SYMBOL_GPL(ts_video_set_dev_parent);

void ts_video_set_fops(struct video_device *vdev, const struct v4l2_file_operations *fops)
{
	vdev->fops = fops;
}
EXPORT_SYMBOL_GPL(ts_video_set_fops);

spinlock_t *ts_video_get_fh_lock(struct video_device *vdev)
{
	return &vdev->fh_lock;
}
EXPORT_SYMBOL_GPL(ts_video_get_fh_lock);

struct list_head *ts_video_get_fh_list(struct video_device *vdev)
{
	return &vdev->fh_list;
}
EXPORT_SYMBOL_GPL(ts_video_get_fh_list);


void *ts_video_get_drvdata(struct video_device *vdev)
{
	return video_get_drvdata(vdev);
}
EXPORT_SYMBOL_GPL(ts_video_get_drvdata);

void ts_video_set_drvdata(struct video_device *vdev, void *data)
{
	return video_set_drvdata(vdev, data);
}
EXPORT_SYMBOL_GPL(ts_video_set_drvdata);

void *ts_video_drvdata(struct file *file)
{
	return video_drvdata(file);
}
EXPORT_SYMBOL_GPL(ts_video_drvdata);

const char *ts_video_device_node_name(struct video_device *vdev)
{
	return video_device_node_name(vdev);
}
EXPORT_SYMBOL_GPL(ts_video_device_node_name);

int ts_video_is_registered(struct video_device *vdev)
{
	return video_is_registered(vdev);
}
EXPORT_SYMBOL_GPL(ts_video_is_registered);

int ts_video_register_device(struct video_device *vdev,
		int type, int nr)
{
	return video_register_device(vdev, type, nr);
}
EXPORT_SYMBOL_GPL(ts_video_register_device);

bool ts_vb2_is_busy(struct vb2_queue *q)
{
	return vb2_is_busy(q);
}
EXPORT_SYMBOL_GPL(ts_vb2_is_busy);

void *ts_vb2_get_drv_priv(struct vb2_queue *q)
{
	return vb2_get_drv_priv(q);
}
EXPORT_SYMBOL_GPL(ts_vb2_get_drv_priv);

unsigned long ts_vb2_plane_size(struct vb2_buffer *vb, unsigned int plane_no)
{
	return vb2_plane_size(vb, plane_no);
}
EXPORT_SYMBOL_GPL(ts_vb2_plane_size);

void ts_vb2_set_plane_payload(struct vb2_buffer *vb,
				 unsigned int plane_no, unsigned long size)
{
	return vb2_set_plane_payload(vb, plane_no, size);
}
EXPORT_SYMBOL_GPL(ts_vb2_set_plane_payload);

struct v4l2_device *ts_alloc_st_v4l2_device(void)
{
	return kzalloc(sizeof(struct v4l2_device), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_v4l2_device);

char *ts_v4l2_dev_get_name(struct v4l2_device *dev)
{
	return dev->name;
}
EXPORT_SYMBOL_GPL(ts_v4l2_dev_get_name);

void ts_v4l2_dev_set_name(struct v4l2_device *dev, char *name, int inst)
{
	snprintf(dev->name, sizeof(dev->name), "%s-%02d", name, inst);
}
EXPORT_SYMBOL_GPL(ts_v4l2_dev_set_name);

void ts_v4l2_dev_set_release(struct v4l2_device *dev,
	void (*release)(struct v4l2_device *v4l2_dev))
{
	dev->release = release;
}
EXPORT_SYMBOL_GPL(ts_v4l2_dev_set_release);

void ts_v4l2_dev_set_notify(struct v4l2_device *dev,
	void (*notify)(struct v4l2_subdev *sd,
			unsigned int notification, void *arg))
{
	dev->notify = notify;
}
EXPORT_SYMBOL_GPL(ts_v4l2_dev_set_notify);

struct video_device *ts_alloc_st_video_device(void)
{
	return kzalloc(sizeof(struct video_device), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_video_device);

struct vb2_queue *ts_alloc_st_vb2_queue(void)
{
	return kzalloc(sizeof(struct vb2_queue), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_vb2_queue);

