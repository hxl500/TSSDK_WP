/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SEQFILE_H__
#define __TS_WRAP_SEQFILE_H__

#include <linux/seq_file.h>

void ts_seqfile_set_priv(struct seq_file *seqf, void *priv);
void *ts_seqfile_get_priv(struct seq_file *seqf);


#endif

