////Josh Urmann
//
//#include <stdio.h>
//
//#define LOBYTE(w) w & 0xff
//#define HIBYTE(w) w >> 8 & 0xff
//
//#define PAGE_SIZE 256
//#define PAGE_FRAMES 128
//#define TLB_SIZE 16
//
//typedef unsigned short ushort;
//typedef unsigned char uchar;
//
//
//struct pts {//page table struct
//	ushort page_n;
//	unsigned int ref;
//};
//struct tlbs {//translation lookaside buffer struct
//	ushort page_n;
//	ushort frame_n;
//	unsigned int ref;//used for FIFO
//};
//
//char MM[PAGE_FRAMES * PAGE_SIZE] = {0};//main memory
//struct pts PT[PAGE_FRAMES] = {0};//page table
//struct tlbs TLB[TLB_SIZE] = {0};//translation lookaside buffer
//
//int g_pagefaultcount = 0;
//int g_tlbhitcount = 0;
//
//
///*
//* adds a page to the page table. determines which page to replace if necessary using FIFO
//* IN - page number to add
//* RETURN - page frame (index) of the added page
//*/
//int add_to_page_table(ushort page_n) {
//	static unsigned int ref = 0;//count to keep track of oldest (smallest is oldest)
//	ref++;//add one to ref every time another page is added to page table
//	if (ref == 0) {//ref overflow, so previous value is MAX_INT and new value is zero, page table ref values need to be adjusted so that current ref is the largest 
//		unsigned int max = 0;
//		for (int i = 0; i < PAGE_FRAMES; i++) {
//			PT[i].ref %= PAGE_FRAMES;
//			if (PT[i].ref > max)
//				max = PT[i].ref;
//		}
//		ref = max + 1;
//	}
//
//	unsigned int smallest = -1;//(unsigned -1) is highest possible value for an integer (easier to find smallest values)
//	int si = 0;//index of smallest
//
//	for (int i = 0; i < PAGE_FRAMES; i++) {//loop through page table
//		//use empty page table entries first
//		if (PT[i].ref == 0) {
//			si = i;
//			break;
//		}
//		//track smallest ref in page table
//		if (PT[i].ref < smallest) {
//			si = i;
//			smallest = PT[i].ref;
//		}
//	}
//
//	//replace 'smallest' ref index in page table
//	PT[si].page_n = page_n;
//	PT[si].ref = ref;
//
//	return si;
//}
//
//
//
//
///*
//* adds page frame and page number to tbl, replaces via FIFO
//* IN - page number, page frame
//* RETURN void
//*/
//void add_to_tlb(ushort page_n, int page_frame) {
//	static unsigned int ref = 0;//count to keep track of oldest (smallest is oldest)
//	ref++;//add one to ref every time another page is added to tlb
//
//	if (ref == 0) {//ref overflow, so previous value is MAX_INT and new value is zero, tlb ref values need to be adjusted so that current ref is the largest 
//		unsigned int max = 0;
//		for (int i = 0; i < TLB_SIZE; i++) {
//			TLB[i].ref = TLB[i].ref % TLB_SIZE;
//			if (TLB[i].ref > max)
//				max = TLB[i].ref;
//		}
//		ref = max + 1;
//	}
//
//	unsigned int smallest = -1;//(unsigned -1) is highest possible value for an integer (easier to find smallest values)
//	int si = 0;//index of smallest
//
//	for (char i = 0; i < TLB_SIZE; i++) {//loop through tlb
//		//use unreferenced tlb entries first
//		if (TLB[i].ref == 0) {
//			si = i;
//			break;
//		}
//		//track smallest ref in tlb
//		if (TLB[i].ref < smallest) {
//			si = i;
//			smallest = TLB[i].ref;
//		}
//	}
//
//	//replace 'smallest' index in TLB (smallest is oldest, or the first one it)
//	TLB[si].frame_n = page_frame;
//	TLB[si].page_n = page_n;
//	TLB[si].ref = ref;
//}
//
//
//
//
///*
//* loads virtual memory into main memory, updates page table and tlb
//* IN - logical address page number
//* RETURN - page frame of page loaded from virtual memory
//*/
//int load_virtual_mem(ushort page_n) {
//	FILE* f = fopen("BACKING_STORE.bin", "r");//open "virtual memory" file
//
//	fseek(f, (long)(page_n * PAGE_SIZE), SEEK_SET);//goto logical address's page number in file
//	char page[PAGE_SIZE];
//	fread(&page, PAGE_SIZE, 1, f);//load page from virtual memory
//	fclose(f);
//
//	//add new entry to page table, and get the frame to move virtual memory into
//	int page_frame = add_to_page_table(page_n);
//
//	//copy page into main memory
//	for (int i = 0; i < PAGE_SIZE; i++)
//		MM[PAGE_SIZE * page_frame + i] = page[i];
//
//	//return the new page frame
//	return page_frame;
//}
//
//
//
///*
//* finds a page in the page table, also handles page faults
//* IN - page number
//* RETURN - page frame for page number (-1 if not found [page fault])
//*/
//int get_page_frame(uchar page_n) {
//	//check TLB first
//	for (char i = 0; i < TLB_SIZE; i++) {
//		if (TLB[i].page_n == page_n) {
//			g_tlbhitcount++;
//			return TLB[i].frame_n;
//		}
//	}
//
//	//check page table
//	for (int i = 0; i < PAGE_FRAMES; i++) {
//		if (PT[i].ref && PT[i].page_n == page_n) {
//			return i;//page hit
//		}
//	}
//
//	//page not found so there was a page fault
//	g_pagefaultcount++;
//
//	int page_frame = load_virtual_mem(page_n);//load page from virtual memory
//
//	//TLB miss occured so add page #/ page frame to TLB
//	add_to_tlb(page_n, page_frame);
//
//	return page_frame;
//}
//
//
//
///*
//* translates a logical address into a physical address. checks tlb for a hit, then checks page table
//* IN - logical address
//* RETURN - physical address
//*/
//int translate_address(ushort logical_addr) {
//	uchar page_n = HIBYTE(logical_addr);
//	uchar offset = LOBYTE(logical_addr);
//
//	int page_frame = get_page_frame(page_n);
//
//	return page_frame * PAGE_SIZE + offset;
//}
//
//
//
//int main(int argc, char **argv) {
//	FILE* f;
//	if (argc > 1) {
//		f = fopen(argv[1], "r");
//	}
//	else
//		f = fopen("addresses.txt", "r");//default filename, when no filename is specified in argurments
//
//	if (!f) {
//		printf("unable to open file\n");
//		return 0;
//	}
//
//	int reference_count = 0;//counter for number of addresses in file
//
//	ushort logical_addr;
//	while (fscanf(f, "%d", &logical_addr) != EOF) {//loop through address file, getting next logical address
//		reference_count += 1;//somehow this wasn't being incremented when i was testing   ????
//
//		int physical_addr = translate_address(logical_addr);//get physical address from logical address
//
//		char val = MM[physical_addr];//get value of address
//
//		printf("Virtual address: %d Physical address: %d Value: %d\n", logical_addr, physical_addr, val);
//	}
//	fclose(f);
//
//	reference_count = 1000;
//
//	printf("Page fault rate: %f%%\t(%d/%d)\n", ((double)g_pagefaultcount / (double)reference_count) * 100, g_pagefaultcount, reference_count);
//	printf("TLB hit rate: %f%%\t(%d/%d)\n", ((double)g_tlbhitcount / (double)reference_count) * 100, g_tlbhitcount, reference_count);
//
//	return 0;
//}
