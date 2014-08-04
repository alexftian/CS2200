#include "swapfile.h"
#include<stdio.h>

#include "statistics.h"
#include "pagetable.h"

/*******************************************************************************
 * Looks up an address in the current page table. If the entry for the given
 * page is not valid, traps to the OS.
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t pagetable_lookup(vpn_t vpn, int write) {
   int i = 0; 
   for(i = 0; i < vpn; i++) {
   }
	pte_t curr =  current_pagetable[i];
	pfn_t pfn = curr.pfn; /* CHANGE ME */
	if (current_pagetable + i != NULL && curr.valid == 1) {

		pfn = curr.pfn; /* CHANGE ME */
	} else {
		count_pagefaults++;
		pfn = pagefault_handler(vpn, write);	
	}

	pfn = pagefault_handler(vpn, write);

	return pfn;
   /* FIX ME - Part 2 
    * Determine the PFN corresponding to the passed in VPN.
    * Perform the lookup using the current_pagetable.
    */

}
