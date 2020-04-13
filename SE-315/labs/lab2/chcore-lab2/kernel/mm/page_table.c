/*
 * Copyright (c) 2020 Institute of Parallel And Distributed Systems (IPADS), Shanghai Jiao Tong University (SJTU)
 * OS-Lab-2020 (i.e., ChCore) is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *   http://license.coscl.org.cn/MulanPSL
 *   THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 *   PURPOSE.
 *   See the Mulan PSL v1 for more details.
 */

#ifdef CHCORE
#include <common/util.h>
#include <common/kmalloc.h>
#endif
#include <common/vars.h>
#include <common/macro.h>
#include <common/types.h>
#include <common/errno.h>
#include <common/printk.h>
#include <common/mm.h>
#include <common/mmu.h>

#include "page_table.h"

/* Page_table.c: Use simple impl for debugging now. */

extern void set_ttbr0_el1(paddr_t);
extern void flush_tlb(void);

void set_page_table(paddr_t pgtbl)
{
	/* XXX Perf: maybe check whether write ttbr0_el1 is needed */
	set_ttbr0_el1(pgtbl);
}

#define USER_PTE 0
#define KERNEL_PTE 1
/*
 * the 3rd arg means the kind of PTE.
 */
static int set_pte_flags(pte_t *entry, vmr_prop_t flags, int kind)
{
	if (flags & VMR_WRITE)
		entry->l3_page.AP = ARM64_MMU_ATTR_PAGE_AP_HIGH_RW_EL0_RW;
	else
		entry->l3_page.AP = ARM64_MMU_ATTR_PAGE_AP_HIGH_RO_EL0_RO;

	if (flags & VMR_EXEC)
		entry->l3_page.UXN = ARM64_MMU_ATTR_PAGE_UX;
	else
		entry->l3_page.UXN = ARM64_MMU_ATTR_PAGE_UXN;

	// EL1 cannot directly execute EL0 accessiable region.
	entry->l3_page.PXN = ARM64_MMU_ATTR_PAGE_PXN;
	entry->l3_page.AF = ARM64_MMU_ATTR_PAGE_AF_ACCESSED;

	// not global
	//entry->l3_page.nG = 1;
	// inner sharable
	entry->l3_page.SH = INNER_SHAREABLE;
	// memory type
	entry->l3_page.attr_index = NORMAL_MEMORY;

	return 0;
}

#define GET_PADDR_IN_PTE(entry) \
	(((u64)entry->table.next_table_addr) << PAGE_SHIFT)
#define GET_NEXT_PTP(entry) phys_to_virt(GET_PADDR_IN_PTE(entry))

#define NORMAL_PTP (0)
#define BLOCK_PTP (1)

/*
 * Find next page table page for the "va".
 *
 * cur_ptp: current page table page
 * level:   current ptp level
 *
 * next_ptp: returns "next_ptp"
 * pte     : returns "pte" (points to next_ptp) in "cur_ptp"
 *
 * alloc: if true, allocate a ptp when missing
 *
 */
static int get_next_ptp(ptp_t *cur_ptp, u32 level, vaddr_t va,
						ptp_t **next_ptp, pte_t **pte, bool alloc)
{
	u32 index = 0;
	pte_t *entry;

	if (cur_ptp == NULL)
		return -ENOMAPPING;

	switch (level)
	{
	case 0:
		index = GET_L0_INDEX(va);
		break;
	case 1:
		index = GET_L1_INDEX(va);
		break;
	case 2:
		index = GET_L2_INDEX(va);
		break;
	case 3:
		index = GET_L3_INDEX(va);
		break;
	default:
		BUG_ON(1);
	}

	entry = &(cur_ptp->ent[index]);
	if (IS_PTE_INVALID(entry->pte))
	{
		if (alloc == false)
		{
			return -ENOMAPPING;
		}
		else
		{
			/* alloc a new page table page */
			ptp_t *new_ptp;
			paddr_t new_ptp_paddr;
			pte_t new_pte_val;

			/* alloc a single physical page as a new page table page */
			new_ptp = get_pages(0);
			BUG_ON(new_ptp == NULL);
			memset((void *)new_ptp, 0, PAGE_SIZE);
			new_ptp_paddr = virt_to_phys((vaddr_t)new_ptp);

			new_pte_val.pte = 0;
			new_pte_val.table.is_valid = 1;
			new_pte_val.table.is_table = 1;
			new_pte_val.table.next_table_addr = new_ptp_paddr >> PAGE_SHIFT;

			/* same effect as: cur_ptp->ent[index] = new_pte_val; */
			entry->pte = new_pte_val.pte;
		}
	}

	*next_ptp = (ptp_t *)GET_NEXT_PTP(entry);
	*pte = entry;
	if (IS_PTE_TABLE(entry->pte))
		return NORMAL_PTP;
	else
		return BLOCK_PTP;
}

/*
 * Translate a va to pa, and get its pte for the flags
 */
/*
 * query_in_pgtbl: translate virtual address to physical 
 * address and return the corresponding page table entry
 * 
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ query virtual address
 * pa @ return physical address
 * entry @ return page table entry
 * 
 * Hint: check the return value of get_next_ptp, if ret == BLOCK_PTP
 * return the pa and block entry immediately
 */
int query_in_pgtbl(vaddr_t *pgtbl, vaddr_t va, paddr_t *pa, pte_t **entry)
{
	// printk("called <query_in_pgtbl>. pgtbl = %p, va = %p\n", pgtbl, va);

	int ret;

	ptp_t *l1_ptp, *l2_ptp, *l3_ptp, *l4_ptp;
	pte_t *l1_pte, *l2_pte, *l3_pte, *l4_pte;

	ret = get_next_ptp((ptp_t *)pgtbl, 0, va, &l1_ptp, &l1_pte, false);
	// // printk("1st called get_next_ptp. l1_ptp = %p, l1_pte = %p, ret = %d\n", l1_ptp, l1_pte, ret);
	if (ret == BLOCK_PTP)
	{
		*pa = (paddr_t)l1_ptp;
		*entry = l1_pte;
		return ret;
	}
	else if (ret == -ENOMAPPING)
	{
		return ret;
	}

	ret = get_next_ptp((ptp_t *)((u64)l1_pte->table.next_table_addr << PAGE_SHIFT), 1, va, &l2_ptp, &l2_pte, false);
	// printk("2nd called get_next_ptp. l2_ptp = %p, l2_pte = %p, ret = %d\n", l2_ptp, l2_pte, ret);
	if (ret == BLOCK_PTP)
	{
		*pa = (paddr_t)l2_ptp;
		*entry = l2_pte;
		return ret;
	}
	else if (ret == -ENOMAPPING)
	{
		return ret;
	}

	ret = get_next_ptp((ptp_t *)((u64)l2_pte->table.next_table_addr << PAGE_SHIFT), 2, va, &l3_ptp, &l3_pte, false);
	// printk("3rd called get_next_ptp. l3_ptp = %p, l3_pte = %p, ret = %d\n", l3_ptp, l3_pte, ret);
	if (ret == BLOCK_PTP)
	{
		*pa = (paddr_t)l3_ptp;
		*entry = l3_pte;
		return ret;
	}
	else if (ret == -ENOMAPPING)
	{
		return ret;
	}

	ret = get_next_ptp((ptp_t *)((u64)l3_pte->table.next_table_addr << PAGE_SHIFT), 3, va, &l4_ptp, &l4_pte, false);
	// printk("4th called get_next_ptp. l4_ptp = %p, l4_pte = %p, ret = %d\n", l4_ptp, l4_pte, ret);

	if (ret == -ENOMAPPING)
	{
		return ret;
	}
	*pa = *(paddr_t *)l4_pte->pte;
	*entry = l4_pte;
	return ret;
}

/*
 * map_range_in_pgtbl: map the virtual address [va:va+size] to 
 * physical address[pa:pa+size] in given pgtbl
 *
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ start virtual address
 * pa @ start physical address
 * len @ mapping size
 * flags @ corresponding attribution bit
 *
 * Hint: In this function you should first invoke the get_next_ptp()
 * to get the each level page table entries. Read type pte_t carefully
 * and it is convenient for you to call set_pte_flags to set the page
 * permission bit. Don't forget to call flush_tlb at the end of this function 
 */
int map_range_in_pgtbl(vaddr_t *pgtbl, vaddr_t base_va, paddr_t base_pa,
					   size_t len, vmr_prop_t flags)
{

	int ret;
	ptp_t *l1_ptp, *l2_ptp, *l3_ptp, *l4_ptp;
	pte_t *l1_pte, *l2_pte, *l3_pte, *l4_pte;

	for (u64 i = 0; i < (u64)len; i += (u64)PAGE_SIZE)
	{
		vaddr_t va = base_va + i;
		paddr_t pa = base_pa + i;

		// printk("called <map_range_in_pgtbl>. pgtbl: %p, va: %p, pa: %p, len: %lu, flags: %lu\n", pgtbl, va, pa, len, flags);

		ret = get_next_ptp((ptp_t *)(pgtbl), 0, va, &l1_ptp, &l1_pte, true);
		// printk("1st called get_next_ptp. l1_ptp = %p, l1_pte = %p, ret = %d\n", l1_ptp, l1_pte, ret);

		set_pte_flags(l1_pte, flags, 42);

		ret = get_next_ptp((ptp_t *)((u64)l1_pte->table.next_table_addr << PAGE_SHIFT), 1, va, &l2_ptp, &l2_pte, true);
		// printk("2nd called get_next_ptp. l2_ptp = %p, l2_pte = %p, ret = %d\n", l2_ptp, l2_pte, ret);

		set_pte_flags(l2_pte, flags, 42);

		ret = get_next_ptp((ptp_t *)((u64)l2_pte->table.next_table_addr << PAGE_SHIFT), 2, va, &l3_ptp, &l3_pte, true);
		// printk("3rd called get_next_ptp. l3_ptp = %p, l3_pte = %p, ret = %d\n", l3_ptp, l3_pte, ret);

		set_pte_flags(l3_pte, flags, 42);

		ret = get_next_ptp((ptp_t *)((u64)l3_pte->table.next_table_addr << PAGE_SHIFT), 3, va, &l4_ptp, &l4_pte, true);
		// printk("4th called get_next_ptp. l4_ptp = %p, l4_pte = %p, ret = %d\n", l4_ptp, l4_pte, ret);

		if (ret != NORMAL_PTP)
		{
			flush_tlb();
			return ret;
		}

		*(paddr_t *)l4_pte->pte = pa;
	}

	flush_tlb();
	return ret;
}

/*
 * unmap_range_in_pgtble: unmap the virtual address [va:va+len]
 * 
 * pgtbl @ ptr for the first level page table(pgd) virtual address
 * va @ start virtual address
 * len @ unmapping size
 * 
 * Hint: invoke get_next_ptp to get each level page table, don't 
 * forget the corner case that the virtual address is not mapped.
 * call flush_tlb() at the end of function
 * 
 */
int unmap_range_in_pgtbl(vaddr_t *pgtbl, vaddr_t base_va, size_t len)
{

	int ret;
	ptp_t *l1_ptp, *l2_ptp, *l3_ptp, *l4_ptp;
	pte_t *l1_pte, *l2_pte, *l3_pte, *l4_pte;

	for (u64 i = 0; i < (u64)len; i += (u64)PAGE_SIZE)
	{
		vaddr_t va = base_va + i;
		// printk("called <unmap_range_in_pgtbl>. pgtbl: %p, va: %p, len: %lu\n", pgtbl, va, len);

		ret = get_next_ptp((ptp_t *)(pgtbl), 0, va, &l1_ptp, &l1_pte, true);
		// printk("1st called get_next_ptp. l1_ptp = %p, l1_pte = %p, ret = %d\n", l1_ptp, l1_pte, ret);

		ret = get_next_ptp((ptp_t *)((u64)l1_pte->table.next_table_addr << PAGE_SHIFT), 1, va, &l2_ptp, &l2_pte, true);
		// printk("2nd called get_next_ptp. l2_ptp = %p, l2_pte = %p, ret = %d\n", l2_ptp, l2_pte, ret);

		ret = get_next_ptp((ptp_t *)((u64)l2_pte->table.next_table_addr << PAGE_SHIFT), 2, va, &l3_ptp, &l3_pte, true);
		// printk("3rd called get_next_ptp. l3_ptp = %p, l3_pte = %p, ret = %d\n", l3_ptp, l3_pte, ret);

		ret = get_next_ptp((ptp_t *)((u64)l3_pte->table.next_table_addr << PAGE_SHIFT), 3, va, &l4_ptp, &l4_pte, true);
		// printk("4th called get_next_ptp. l4_ptp = %p, l4_pte = %p, ret = %d\n", l4_ptp, l4_pte, ret);

		if (ret != NORMAL_PTP)
		{
			flush_tlb();
			return ret;
		}

		l4_pte->pte = (u64)0;
	}

	flush_tlb();
	return ret;
}
