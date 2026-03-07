#include <linux/module.h>
#include <soc/ts/wrap/pgtable.h>


pgprot_t ts_pgprot_writecombine(pgprot_t prot)
{
	return pgprot_writecombine(prot);
}
EXPORT_SYMBOL(ts_pgprot_writecombine);

pgprot_t ts_PAGE_KERNEL(void)
{
	return PAGE_KERNEL;
}
EXPORT_SYMBOL(ts_PAGE_KERNEL);

unsigned long ts_pte_pfn(pte_t *pte)
{
	return pte_pfn((*pte));
}
EXPORT_SYMBOL(ts_pte_pfn);

int ts_pte_present(pte_t *pte)
{
	return pte_present((*pte));
}
EXPORT_SYMBOL(ts_pte_present);

int ts_pmd_none(pmd_t *pmd)
{
	return pmd_none((*pmd));
}
EXPORT_SYMBOL(ts_pmd_none);

int ts_pmd_bad(pmd_t *pmd)
{
	return pmd_bad((*pmd));
}
EXPORT_SYMBOL(ts_pmd_bad);

pmd_t *ts_pmd_offset(pud_t *pud, unsigned long addr)
{
	return pmd_offset(pud, addr);
}
EXPORT_SYMBOL(ts_pmd_offset);


int ts_pud_none(pud_t *pud)
{
	return pud_none((*pud));
}
EXPORT_SYMBOL(ts_pud_none);

int ts_pud_bad(pud_t *pud)
{
	return pud_bad((*pud));
}
EXPORT_SYMBOL(ts_pud_bad);

pud_t *ts_pud_offset(pgd_t *pgd, unsigned long address)
{
	return pud_offset(pgd, address);
}
EXPORT_SYMBOL(ts_pud_offset);


int ts_pgd_none(pgd_t *pgd)
{
	return pgd_none((*pgd));
}
EXPORT_SYMBOL(ts_pgd_none);

int ts_pgd_bad(pgd_t *pgd)
{
	return pgd_bad((*pgd));
}
EXPORT_SYMBOL(ts_pgd_bad);

pgd_t *ts_pgd_offset(struct mm_struct *mm, unsigned long addr)
{
	return pgd_offset(mm, addr);
}
EXPORT_SYMBOL(ts_pgd_offset);


