#include <linux/module.h>
#include <soc/ts/wrap/pcm.h>

void *ts_snd_pcm_substream_chip(struct snd_pcm_substream *substream)
{
	return snd_pcm_substream_chip(substream);
}
EXPORT_SYMBOL(ts_snd_pcm_substream_chip);

void ts_snd_pcm_set_priv_data(struct snd_pcm		*pcm, void *priv_data)
{
	pcm->private_data = priv_data;
}
EXPORT_SYMBOL(ts_snd_pcm_set_priv_data);

void *ts_snd_pcm_substream_get_runtime(struct snd_pcm_substream *substream)
{
	return substream->runtime;
}
EXPORT_SYMBOL(ts_snd_pcm_substream_get_runtime);

void *ts_snd_pcm_substream_get_hw(struct snd_pcm_substream *substream)
{
	return &substream->runtime->hw;
}
EXPORT_SYMBOL(ts_snd_pcm_substream_get_hw);

