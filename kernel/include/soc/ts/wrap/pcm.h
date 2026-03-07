/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_PCM_H__
#define __TS_WRAP_PCM_H__

#include <sound/pcm.h>
void *ts_snd_pcm_substream_chip(struct snd_pcm_substream *substream);
void ts_snd_pcm_set_priv_data(struct snd_pcm		*pcm, void *priv_data);
void *ts_snd_pcm_substream_get_runtime(struct snd_pcm_substream *substream);
void *ts_snd_pcm_substream_get_hw(struct snd_pcm_substream *substream);

#endif
