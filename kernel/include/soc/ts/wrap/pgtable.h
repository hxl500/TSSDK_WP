/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_PGTABLE_H__
#define __TS_WRAP_PGTABLE_H__

#include <asm/pgtable.h>

pgprot_t ts_pgprot_writecombine(pgprot_t prot);
pgprot_t ts_PAGE_KERNEL(void);

unsigned long ts_pte_pfn(pte_t *pte);
int ts_pmd_none(pmd_t *pmd);
int ts_pmd_bad(pmd_t *pmd);
int ts_pmd_bad(pmd_t *pmd);
pmd_t *ts_pmd_offset(pud_t *pud, unsigned long addr);

int ts_pud_none(pud_t *pud);
int ts_pud_bad(pud_t *pud);
pud_t *ts_pud_offset(pgd_t *pgd, unsigned long address);

int ts_pgd_none(pgd_t *pgd);
int ts_pgd_bad(pgd_t *pgd);
pgd_t *ts_pgd_offset(struct mm_struct *mm, unsigned long addr);


#endif

