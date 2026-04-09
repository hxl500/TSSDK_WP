#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/marker.h>

#define PROC_FILENAME "marker"

#define MARKER_NAME_SIZE 8
#define MARKER_DEBUF_INFO_SIZE 32
#define MARKER_ITEM_DEF_NUM 300

struct marker_item {
	char name[MARKER_NAME_SIZE];
	struct timeval ts;
	char debug_info[MARKER_DEBUF_INFO_SIZE];
};

static int marker_item_num = MARKER_ITEM_DEF_NUM;
module_param(marker_item_num, int, 0644);

struct marker {
	struct proc_dir_entry *proc_entry;
	struct marker_item *items;
	atomic_t item_cnt;
	bool enable;
};

static struct marker *marker_gp;

static int proc_show(struct seq_file *p, void *v)
{
	unsigned int i, cnt;
	struct marker_item *item;

	cnt = atomic_read(&marker_gp->item_cnt);
	cnt = (cnt < marker_item_num)?cnt:marker_item_num;

	seq_printf(p,"\n==========marker==========\n");
	for (i=0; i<cnt; i++) {
		item = &marker_gp->items[i];
		seq_printf(p,">> [%03d] (%s) %s, timestamp: %lu.%06lu <<\n", i,
			item->name, item->debug_info, item->ts.tv_sec, item->ts.tv_usec);
	}
	seq_printf(p,"\n===========end============\n");
    return 0;
}

static ssize_t proc_write(struct file *file, const char __user *buffer,
                           size_t count, loff_t *ppos)
{
	int ret;
	char cmd_str[128];
	char arg0[32], arg1[32], arg2[32];
	char *token, *str;
	int len;
	//long val;

    if (count >= 128) {
		pr_err("command too long");
        return -EINVAL;
	}

    ret = copy_from_user(cmd_str, buffer, count);
	if (ret < 0)
		return -EFAULT;

	cmd_str[count] = '\0';
	str = cmd_str;
	token = strsep(&str, " ");
	len = strlen(token);
	if (len >= 32)
		return -EFAULT;

	memcpy(arg0, token, len);
	arg0[len] = '\0';
	pr_info("arg0: [%s], len=%d \n", arg0, len);

	token = strsep(&str, " ");
	if (token != NULL) {
		len = strlen(token);
		if (len >= 32)
			return -EFAULT;

		memcpy(arg1, token, len);
		arg1[len] = '\0';
		pr_debug("arg1: [%s], len=%d \n", arg1, len);
	}

	token = strsep(&str, "\n");
	if (token != NULL) {
		len = strlen(token);
		if (len >= 32)
			return -EFAULT;

		memcpy(arg2, token, len);
		arg2[len] = '\0';
		pr_debug("arg2: [%s], len=%d \n", arg2, len);
	}

#if 0
	token = strsep(&str, " ");
	if (token != NULL) {
		len = strlen(token);
		if (len >= 32)
			return -EFAULT;

		memcpy(arg1, token, len);
		arg1[len] = '\0';
		ret = kstrtol(arg1, 0, &val);
		if (ret == 0)
			pr_debug("val: [%ld]\n", val);
		else
			pr_debug("val default 0\n");
	}
#endif
	if (!strncmp(arg0, "insert", 6)) {
		insert_marker(arg1, arg2);
		pr_debug("insert success\n");
	} else if (!strncmp(arg0, "clear", 5)) {
		atomic_set(&marker_gp->item_cnt, 0);
	} else if (!strncmp(arg0, "start", 5)) {
		marker_gp->enable = true;
	} else if (!strncmp(arg0, "stop", 4)) {
		marker_gp->enable = false;
	} else if (!strncmp(arg0, "help", 4)) {
		pr_info("help: [token] [arg1] [arg2]\n");
	}

	return count;
}

int insert_marker(const char *name, const char *info)
{
	int cnt, idx;
	struct marker_item *item;

	if (name == NULL)
		return -EINVAL;

	if (!marker_gp->enable)
		return 0;

	cnt = atomic_inc_return(&marker_gp->item_cnt);
	if (cnt > marker_item_num) {
		atomic_dec(&marker_gp->item_cnt);
		return -ENOMEM;
	}

	idx = cnt - 1;
	item = &marker_gp->items[idx];
	do_gettimeofday(&item->ts);

	strncpy(item->name, name, MARKER_NAME_SIZE);
	item->name[MARKER_NAME_SIZE-1] = '\0';

	if (info != NULL) {
		strncpy(item->debug_info, info, MARKER_DEBUF_INFO_SIZE);
		item->debug_info[MARKER_DEBUF_INFO_SIZE-1] = '\0';
	} else
		memset(item->debug_info, 0, MARKER_DEBUF_INFO_SIZE);

	return 0;
}
EXPORT_SYMBOL(insert_marker);

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open,
    .read = seq_read,
    .write = proc_write,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init marker_proc_init(void)
{
	struct marker *marker;
	struct marker_item *items;
	struct proc_dir_entry *proc_entry;
	int ret;

	marker = kzalloc(sizeof(struct marker), GFP_KERNEL);
	if (marker == NULL) {
		pr_err("marker struct kzalloc failed\n");
		return -ENOMEM;
	}

	items = kzalloc(sizeof(struct marker_item) * marker_item_num, GFP_KERNEL);
	if (items == NULL) {
		pr_err("marker_item struct kzalloc failed\n");
		ret = -ENOMEM;
		goto err_cont;
	}

	marker->items = items;
	atomic_set(&marker->item_cnt, 0);
	marker->enable = false;

    proc_entry = proc_create(PROC_FILENAME, 0644, NULL, &proc_fops);
    if (proc_entry == NULL) {
        pr_err("Failed to create /proc/%s\n", PROC_FILENAME);
		goto err_proc;
    }
	marker->proc_entry = proc_entry;
	marker_gp = marker;
    
    pr_info("/proc/%s created\n", PROC_FILENAME);
	return 0;
err_proc:
	kfree(items);
err_cont:
	kfree(marker);
    return ret;
}

static void __exit marker_proc_exit(void)
{
    remove_proc_entry(PROC_FILENAME, NULL);
	kfree(marker_gp->items);
	kfree(marker_gp);
    pr_info("/proc/%s removed\n", PROC_FILENAME);
}

module_init(marker_proc_init);
module_exit(marker_proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard");
