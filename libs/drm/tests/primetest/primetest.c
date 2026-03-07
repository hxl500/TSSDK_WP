/***************************************************************************
* COPYRIGHT NOTICE
* Copyright 2022 Tsingmicro, Inc.
* All rights reserved.
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "ion.h"

#define ION_DEV		"/dev/ion"
#define MAX_HEAP_CNT	8

struct buffer_object {
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	uint32_t handle;
	uint32_t size;
	uint32_t *vaddr;
	uint32_t fb_id;
	uint32_t prime_fd;
};

struct buffer_object buf[2];
static int terminate;

int ion_alloc(uint32_t size, uint32_t **virt_addr, uint32_t *prime_fd)
{
	int ret;
	int ion_fd;
	struct ion_heap_query heap_query;
	uint32_t i;
	struct ion_heap_data *heap_data;
	struct ion_allocation_data alloc_data;
	int cma_heap_id = -1;
	uint32_t *virt_buf;

	ion_fd = open(ION_DEV, O_RDWR);
	if (ion_fd < 0) {
		printf("open ion errro.\n");
		goto err_1;
	}

	/*
	 * heap_query.cnt 3
		heap 0:
		        name linux,cma:
		        type 4:
		        heap id 2:
		heap 1:
		        name ion_system_heap:
		        type 0:
		        heap id 1:
		heap 2:
		        name ion_system_contig_heap:
		        type 1:
		        heap id 0:
	 */
	memset(&heap_query, 0, sizeof(struct ion_heap_query));
	heap_query.cnt = MAX_HEAP_CNT;	// must not 0
	heap_query.heaps = (__u64)malloc(sizeof(struct ion_heap_data)
		* MAX_HEAP_CNT);
	if (!heap_query.heaps) {
		printf("malloc heaps errro.\n");
		goto err_2;
	}
	ret = ioctl(ion_fd, ION_IOC_HEAP_QUERY, &heap_query);
	if (ret) {
		printf("query heap error %d.\n", ret);
		goto err_2;
	}
	printf("heap_query.cnt %d\n", heap_query.cnt);
	heap_data = (struct ion_heap_data *)heap_query.heaps;
	for(i = 0; i < heap_query.cnt; i++){
		printf("heap %d:\n", i);
		printf("	name %s:\n", heap_data[i].name);
		printf("	type %d:\n", heap_data[i].type);
		printf("	heap id %d:\n", heap_data[i].heap_id);
		if (strcmp("linux,cma", heap_data[i].name) == 0)
			cma_heap_id = heap_data[i].heap_id;
	}
	if (cma_heap_id == -1) {
		printf("not found cma heap.\n");
		goto err_2;
	}

	memset(&alloc_data, 0, sizeof(struct ion_allocation_data));
	alloc_data.len = size;
	alloc_data.heap_id_mask = (1 << cma_heap_id);
	alloc_data.flags = 0;
	alloc_data.fd = 0;
	alloc_data.unused = 0;
	ret = ioctl(ion_fd, ION_IOC_ALLOC, &alloc_data);
	if (ret) {
		printf("alloc buf err %d.\n", ret);
		goto err_2;
	}
	printf("alloc fd %d\n", alloc_data.fd);

	virt_buf = mmap(NULL, size, PROT_READ | PROT_WRITE,
		MAP_SHARED, alloc_data.fd, 0);
	if (MAP_FAILED	== virt_buf) {
		printf("mmap buf err %d.\n", ret);
		goto err_3;
	}
    *virt_addr = virt_buf; 
	*prime_fd = alloc_data.fd;
	printf("buf addr %lx\n", (uintptr_t)virt_buf);
	printf("virt_addr %lx\n", (uintptr_t)*virt_addr);
	printf("prime_fd %d\n", *prime_fd);
	// virt_buf[0] = 0xaa;
	// printf("virt_buf[0] %x\n", virt_buf[0]);
	close(ion_fd);
	
	return 0;
err_3:
	close(alloc_data.fd);
err_2:
	close(ion_fd);
err_1:
	return -1;
}

static int modeset_create_fb(int fd, struct buffer_object *bo, uint16_t color)
{
	// struct drm_mode_create_dumb create = {};
 	// struct drm_mode_map_dumb map = {};
	uint32_t i;

	// create.width = bo->width;
	// create.height = bo->height;
	// create.bpp = 16;
	uint32_t bpp = 16;
	uint32_t ret = 0;
	uint16_t * vaddr = NULL;

	bo->size = bo->width * bo->height * bpp;
	// drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);

	bo->pitch = bo->width * 2;
	ret = ion_alloc(bo->size, &bo->vaddr, &bo->prime_fd);
	if (ret) {
		printf("ion_alloc error!\n");
		return -1;
	}
	ret = drmPrimeFDToHandle(fd, bo->prime_fd, &bo->handle);
	if (ret) {
		printf("import prime fd error!\n");
		return -1;
	}
	printf("bo->handle:%d\n", bo->handle);

	ret = drmModeAddFB(fd, bo->width, bo->height, 16, 16, bo->pitch,
			   bo->handle, &bo->fb_id);
	if (ret) {
		printf("add fb error!\n");
		return -1;
	}

	printf("bo->fb_id:%d\n", bo->fb_id);

	vaddr = (uint16_t *)bo->vaddr;
	for (i = 0; i < (bo->size / 2); i++)
		vaddr[i] = color;

	return 0;
}

static void modeset_destroy_fb(int fd, struct buffer_object *bo)
{
	// struct drm_mode_destroy_dumb destroy = {};

	drmModeRmFB(fd, bo->fb_id);

	munmap(bo->vaddr, bo->size);
	close(bo->prime_fd);
}

static void modeset_page_flip_handler(int fd, uint32_t frame,
				    uint32_t sec, uint32_t usec,
				    void *data)
{
	static int i = 0;
	uint32_t crtc_id = *(uint32_t *)data;

	i ^= 1;
	printf("i = %d\n", i);
	drmModePageFlip(fd, crtc_id, buf[i].fb_id,
			DRM_MODE_PAGE_FLIP_EVENT, data);

	usleep(500000);
}

static void sigint_handler(int arg)
{
	terminate = 1;
}

int main(int argc, char **argv)
{
	int fd;
	drmEventContext ev = {};
	drmModeConnector *conn;
	drmModeRes *res;
	uint32_t conn_id;
	uint32_t crtc_id;

	/* register CTRL+C terminate interrupt */
	signal(SIGINT, sigint_handler);

	ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = modeset_page_flip_handler;

	fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);

	res = drmModeGetResources(fd);
	crtc_id = res->crtcs[0];
	conn_id = res->connectors[0];

	conn = drmModeGetConnector(fd, conn_id);
	buf[0].width = conn->modes[0].hdisplay;
	buf[0].height = conn->modes[0].vdisplay;
	buf[1].width = conn->modes[0].hdisplay;
	buf[1].height = conn->modes[0].vdisplay;
	
	printf("buf width %d\n", conn->modes[0].hdisplay);
	printf("buf height %d\n", conn->modes[0].vdisplay);

	modeset_create_fb(fd, &buf[0], 0xf800);
	modeset_create_fb(fd, &buf[1], 0x867d);

	drmModeSetCrtc(fd, crtc_id, buf[0].fb_id,
			0, 0, &conn_id, 1, &conn->modes[0]);

	drmModePageFlip(fd, crtc_id, buf[0].fb_id,
			DRM_MODE_PAGE_FLIP_EVENT, &crtc_id);

	while (!terminate) {
		drmHandleEvent(fd, &ev);
	}

	modeset_destroy_fb(fd, &buf[1]);
	modeset_destroy_fb(fd, &buf[0]);

	drmModeFreeConnector(conn);
	drmModeFreeResources(res);

	close(fd);

	return 0;
}
