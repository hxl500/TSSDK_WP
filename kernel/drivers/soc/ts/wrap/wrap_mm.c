#include <linux/module.h>
#include <soc/ts/wrap/mm.h>

struct page *ts_nth_page(struct page *page, int n)
{
	return nth_page(page, n);
}
EXPORT_SYMBOL(ts_nth_page);

void *ts_page_address(const struct page *page)
{
	return page_address(page);
}
EXPORT_SYMBOL(ts_page_address);


int  ts_get_MAX_ORDER(void)
{
	return MAX_ORDER;
}
EXPORT_SYMBOL(ts_get_MAX_ORDER);


struct page *ts_virt_to_page(void *kaddr)
{
	return virt_to_page(kaddr);
}
EXPORT_SYMBOL(ts_virt_to_page);

phys_addr_t ts_virt_to_phys(void *kaddr)
{
	return virt_to_phys(kaddr);
}
EXPORT_SYMBOL(ts_virt_to_phys);

struct page *ts_phys_to_page(phys_addr_t paddr)
{
	return phys_to_page(paddr);
}
EXPORT_SYMBOL(ts_phys_to_page);

struct page *ts_pfn_to_page(unsigned long pfn)
{
	return pfn_to_page(pfn);
}
EXPORT_SYMBOL(ts_pfn_to_page);


phys_addr_t ts_page_to_phys(struct page *page)
{
	return page_to_phys(page);
}
EXPORT_SYMBOL(ts_page_to_phys);

unsigned long ts_page_to_pfn(struct page *page)
{
	return page_to_pfn(page);
}
EXPORT_SYMBOL(ts_page_to_pfn);


int ts_pfn_valid(unsigned long pfn)
{
	return pfn_valid(pfn);
}
EXPORT_SYMBOL(ts_pfn_valid);

int ts_virt_addr_valid(unsigned long kaddr)
{
	return virt_addr_valid(kaddr);
}
EXPORT_SYMBOL(ts_virt_addr_valid);

pte_t *ts_pte_offset_map_lock(struct mm_struct *mm, pmd_t *pmd,
	unsigned long addr, spinlock_t **ptl)
{
	return pte_offset_map_lock(mm, pmd, addr, ptl);
}
EXPORT_SYMBOL(ts_pte_offset_map_lock);

void ts_pte_unmap_unlock(pte_t *pte, spinlock_t *ptl)
{
	pte_unmap_unlock(pte, ptl);
}
EXPORT_SYMBOL(ts_pte_unmap_unlock);


