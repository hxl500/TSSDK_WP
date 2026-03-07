/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_V4L2_FH_H__
#define __TS_WRAP_V4L2_FH_H__

#include <media/v4l2-dev.h>
#include <media/v4l2-fh.h>

int ts_v4l2_fh_is_singular_file(struct file *filp);
struct v4l2_fh *ts_alloc_st_v4l2_fh(void);

#endif

