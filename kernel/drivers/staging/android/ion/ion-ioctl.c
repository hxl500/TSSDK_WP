/*
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/scatterlist.h>

#include <linux/seq_file.h>
#include <linux/slab.h>
#include "ion.h"

union ion_ioctl_arg {
	struct ion_allocation_data allocation;
	struct ion_allocation_data_name allocation_name;
	struct ion_heap_query query;
};

static int validate_ioctl_arg(unsigned int cmd, union ion_ioctl_arg *arg)
{
	int ret = 0;

	switch (cmd) {
	case ION_IOC_HEAP_QUERY:
		ret = arg->query.reserved0 != 0;
		ret |= arg->query.reserved1 != 0;
		ret |= arg->query.reserved2 != 0;
		break;
	default:
		break;
	}

	return ret ? -EINVAL : 0;
}

/* fix up the cases where the ioctl direction bits are incorrect */
static unsigned int ion_ioctl_dir(unsigned int cmd)
{
	switch (cmd) {
	default:
		return _IOC_DIR(cmd);
	}
}

long ion_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	unsigned int dir;
	union ion_ioctl_arg data;

	dir = ion_ioctl_dir(cmd);

	if (_IOC_SIZE(cmd) > sizeof(data))
		return -EINVAL;

	/*
	 * The copy_from_user is unconditional here for both read and write
	 * to do the validate. If there is no write for the ioctl, the
	 * buffer is cleared
	 */
	if (copy_from_user(&data, (void __user *)arg, _IOC_SIZE(cmd)))
		return -EFAULT;

	ret = validate_ioctl_arg(cmd, &data);
	if (ret) {
		pr_warn_once("%s: ioctl validate failed\n", __func__);
		return ret;
	}

	if (!(dir & _IOC_WRITE))
		memset(&data, 0, sizeof(data));

	switch (cmd) {
	case ION_IOC_ALLOC:
	{
		int fd;
		struct ion_buffer *buffer;
		struct sg_table *table;
		struct page *page;

		fd = ion_alloc(data.allocation.len,
			       data.allocation.heap_id_mask,
			       data.allocation.flags,
			       &buffer);
		if (fd < 0)
			return fd;

		data.allocation.fd = fd;
		table = buffer->sg_table;
		page = sg_page(table->sgl);
		data.allocation.paddr = PFN_PHYS(page_to_pfn(page));
		cma_sys_entry_list_add(data.allocation.paddr, data.allocation.len, "NULL");
		break;
	}
	case ION_IOC_ALLOC_NAME:
	{
		int fd;
		struct ion_buffer *buffer;
		struct sg_table *table;
		struct page *page;

		fd = ion_alloc(data.allocation_name.len,
			       data.allocation_name.heap_id_mask,
			       data.allocation_name.flags,
			       &buffer);
		if (fd < 0)
			return fd;

		data.allocation_name.fd = fd;
		table = buffer->sg_table;
		page = sg_page(table->sgl);
		data.allocation_name.paddr = PFN_PHYS(page_to_pfn(page));
		if (strlen(data.allocation_name.name) == 0)
			strcpy(data.allocation_name.name, "NULL");
		if (strcmp(data.allocation_name.name, "vb_pool") == 0)
			break;
		cma_sys_entry_list_add(data.allocation_name.paddr, data.allocation_name.len, data.allocation_name.name);
		break;
	}
	case ION_IOC_HEAP_QUERY:
		ret = ion_query_heaps(&data.query);
		break;
	default:
		return -ENOTTY;
	}

	if (dir & _IOC_READ) {
		if (copy_to_user((void __user *)arg, &data, _IOC_SIZE(cmd)))
			return -EFAULT;
	}
	return ret;
}
