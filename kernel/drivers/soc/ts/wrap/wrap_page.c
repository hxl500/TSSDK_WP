#include <linux/module.h>
#include <soc/ts/wrap/page.h>
#include <linux/gfp.h>
#include <linux/mm.h>

int ts_PageHighMem(struct page *page)
{
	return PageHighMem(page);
}
EXPORT_SYMBOL(ts_PageHighMem);

int ts_PageReserved(struct page *page)
{
	return PageReserved(page);
}
EXPORT_SYMBOL(ts_PageReserved);


void ts_ClearPageReserved(struct page *page)
{
	ClearPageReserved(page);
}
EXPORT_SYMBOL(ts_ClearPageReserved);

void ts_SetPageReserved(struct page *page)
{
	SetPageReserved(page);
}
EXPORT_SYMBOL(ts_SetPageReserved);

void ts_SetPageDirty(struct page *page)
{
	SetPageDirty(page);
}
EXPORT_SYMBOL(ts_SetPageDirty);

size_t ts_page_get_size(void)
{
	return sizeof(struct page);
}
EXPORT_SYMBOL(ts_page_get_size);


struct page *ts_alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	return alloc_pages(gfp_mask, order);
}
EXPORT_SYMBOL(ts_alloc_pages);

void ts_put_page(struct page *page)
{
	put_page(page);
}
EXPORT_SYMBOL(ts_put_page);

int ts_get_page_unless_zero(struct page *page)
{
	return get_page_unless_zero(page);
}
EXPORT_SYMBOL(ts_get_page_unless_zero);

