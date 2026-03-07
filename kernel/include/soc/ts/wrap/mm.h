/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_MM_H__
#define __TS_WRAP_MM_H__

#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/spinlock.h>

struct page *ts_nth_page(struct page *page, int n);
void *ts_page_address(const struct page *page);
int  ts_get_MAX_ORDER(void);
struct page *ts_virt_to_page(void *kaddr);
phys_addr_t ts_virt_to_phys(void *kaddr);

struct page *ts_phys_to_page(phys_addr_t paddr);
phys_addr_t ts_page_to_phys(struct page *page);
struct page *ts_pfn_to_page(unsigned long pfn);
unsigned long ts_page_to_pfn(struct page *page);

int ts_pfn_valid(unsigned long pfn);
int ts_virt_addr_valid(unsigned long kaddr);


void ts_pte_unmap_unlock(pte_t *pte, spinlock_t *ptl);
unsigned long ts_pte_pfn(pte_t *pte);
int ts_pte_present(pte_t *pte);
pte_t *ts_pte_offset_map_lock(struct mm_struct *mm, pmd_t *pmd,
	unsigned long addr, spinlock_t **ptl);



#endif

