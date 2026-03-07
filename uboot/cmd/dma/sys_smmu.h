#ifndef __SYS_SMMU__
#define __SYS_SMMU__

//#include "io.h"

void smmu_dma0_bypass(void);
int smmu_dma0_is_idle(void);
void smmu_dma0_lp_en(void);
void smmu_dma0_lp_quit(void);
#endif
