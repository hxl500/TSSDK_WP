/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __TS_PDMIC_H_
#define __TS_PDMIC_H_

#include <linux/bitops.h>

#define PDMIC_CFG	0x00000000
#define PDMIC_CFG_HPF_DIS                       0x0
#define PDMIC_CFG_HPF_EN                        0x1
#define PDMIC_CFG_HPF_MASK                      BIT(7)
#define PDMIC_CFG_HPF_SHIFT                     (7)

#define PDMIC_CFG_STOREMODE_NOT_INTERLEAVING    0x0
#define PDMIC_CFG_STOREMODE_INTERLEAVING        0x1
#define PDMIC_CFG_STOREMODE_MASK                BIT(3)
#define PDMIC_CFG_STOREMODE_SHIFT               (3)

#define PDMIC_CFG_PCMWD_16BITS                  0x0
#define PDMIC_CFG_PCMWD_20BITS                  0x1
#define PDMIC_CFG_PCMWD_MASK                    BIT(2)
#define PDMIC_CFG_PCMWD_SHIFT                   (2)

#define PDMIC_CFG_SMPLEDGE_FALL              0x0
#define PDMIC_CFG_SMPLEDGE_RISE              0x1
#define PDMIC_CFG_SMPLEDGE_MASK                 BIT(1)
#define PDMIC_CFG_SMPLEDGE_SHIFT                (1)

#define PDMIC_CFG_STEREO_MONO                   0x0
#define PDMIC_CFG_STEREO_STEREO                 0x1
#define PDMIC_CFG_STEREO_MASK                   BIT(0)
#define PDMIC_CFG_STEREO_SHIFT                  (0)

#define PDMIC_ENABLE    0x00000004
#define PDMIC_ENABLE_PDMEN_DIS	    0x0
#define PDMIC_ENABLE_PDMEN_EN	    0x1
#define PDMIC_ENABLE_PDMEN_MASK	    BIT(0)
#define PDMIC_ENABLE_PDMEN_SHIFT	(0)

#define PDMIC_FIFO_CFG    0x00000008
#define PDMIC_FIFO_CFG_EN                 0x1
#define PDMIC_FIFO_CFG_DIS                0x0
#define PDMIC_FIFO_CFG_TRIGLVL_MASK       GENMASK(7, 4)
#define PDMIC_FIFO_CFG_TRIGLVL_SHIFT      (4)
#define PDMIC_FIFO_CFG_DMAEN_MASK         BIT(1)
#define PDMIC_FIFO_CFG_DMAEN_SHIFT        (1)
#define PDMIC_FIFO_CFG_INTEN_MASK         BIT(0)
#define PDMIC_FIFO_CFG_INTEN_SHIFT        (0)

#define PDMIC_FIFO_INTEN    0x0000000c
#define PDMIC_FIFO_INTEN_EN                 0x1
#define PDMIC_FIFO_INTEN_DIS                0x0
#define PDMIC_FIFO_INTEN_UNDERFLOW_MASK     BIT(2)
#define PDMIC_FIFO_INTEN_UNDERFLOW_SHIFT    (2)
#define PDMIC_FIFO_INTEN_OVERFLOW_MASK      BIT(1)
#define PDMIC_FIFO_INTEN_OVERFLOW_SHIFT     (1)
#define PDMIC_FIFO_INTEN_FIFOTRIG_MASK      BIT(0)
#define PDMIC_FIFO_INTEN_FIFOTRIG_SHIFT     (0)

#define PDMIC_FIFO_STAT    0x00000010
#define PDMIC_FIFO_STAT_EMPTY       BIT(4)
#define PDMIC_FIFO_STAT_FULL        BIT(3)
#define PDMIC_FIFO_STAT_UNDERFLOW   BIT(2)
#define PDMIC_FIFO_STAT_OVERFLOW    BIT(1)
#define PDMIC_FIFO_STAT_FIFOTRIG    BIT(0)

#define PDMIC_FIFO_RD    0x00000014

#define PDMIC_FIFO_LVL    0x00000018


typedef enum _PDM_FifoTrigNum {
	ONE = 0,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
	ELEVEN,
	TWELVE,
	THIRTEEN,
	FOURTEEN,
	FIFTEEN,
	SIXTEEN,
} ePDM_FifoTrigNum;


#endif
