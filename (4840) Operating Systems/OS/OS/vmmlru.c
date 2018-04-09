//Josh Urmann

#include <stdio.h>

#define LOBYTE(w) w & 0xff
#define HIBYTE(w) w >> 8 & 0xff

#define PAGE_SIZE 256
#define PAGE_FRAMES 128
#define TLB_SIZE 16

typedef unsigned short ushort;
typedef unsigned char uchar;


struct pts {//page table struct
	ushort page_n;
	unsigned int ref;
};
struct tlbs {//translation lookaside buffer struct
	ushort page_n;
	ushort frame_n;
	unsigned int ref;//used for FIFO
};

char MM[PAGE_FRAMES * PAGE_SIZE] = {0};//main memory
struct pts PT[PAGE_FRAMES] = {0};//page table
struct tlbs TLB[TLB_SIZE] = {0};//translation lookaside buffer

int g_pagefaultcount = 0;
int g_tlbhitcount = 0;


/*
* adds a page to the page table. determines which page to replace if necessary, using LRU
* IN - page number to add
* RETURN - page frame (index) of the added page
*/
int add_to_page_table(ushort page_n) {
	int oi = 0;//stores the oldest index to be replaced
	unsigned int oldest = 0;//variable to track the oldest ref in page table

	for (int i = 0; i < PAGE_FRAMES; i++) {
		if (PT[i].ref == 0) {//use unused page table entries first
			oi = i;
			oldest = -1;
		}
		if (PT[i].ref > oldest) {//track largest ref (which is the oldest)
			oldest = PT[i].ref;
			oi = i;//save index of oldest
		}
	}

	//replace oldest index in page table
	PT[oi].page_n = page_n;
	PT[oi].ref = 1;//set ref to 1 to make it the newest

	return oi;//return index to page in page table
}




/*
* adds page frame and page number to tbl, replaces via FIFO
* IN - page number, page frame
* RETURN void
*/
void add_to_tlb(ushort page_n, int page_frame) {
	static unsigned int ref = 0;//count to keep track of oldest (smallest is oldest)
	ref++;//add one to ref every time another page is added to tlb [overflow is bad]

	//possible solution to ref overflow
	//if (ref == 0) {//ref overflow, so previous value is MAX_INT and new value is zero, tlb ref values need to be adjusted so that current ref is the largest 
	//	unsigned int max = 0;
	//	for (int i = 0; i < TLB_SIZE; i++) {
	//		TLB[i].ref = TLB[i].ref % TLB_SIZE;
	//		if (TLB[i].ref > max)
	//			max = TLB[i].ref;
	//	}
	//	ref = max + 1;
	//}

	unsigned int smallest = -1;//(unsigned -1) is highest possible value for an integer (all values should be smaller than this so current smallest can easily be tracked)
	int si = 0;//index of smallest

	for (char i = 0; i < TLB_SIZE; i++) {//loop through tlb
		//use unreferenced tlb entries first
		if (TLB[i].ref == 0) {
			si = i;
			break;
		}
		//track smallest ref in tlb
		if (TLB[i].ref < smallest) {
			si = i;//save smallest index
			smallest = TLB[i].ref;
		}
	}

	//replace 'smallest' index in TLB
	TLB[si].frame_n = page_frame;
	TLB[si].page_n = page_n;
	TLB[si].ref = ref;//update with new ref count
}




/*
* loads virtual memory into main memory, updates page table and tlb
* IN - logical address page number
* RETURN - page frame of page loaded from virtual memory
*/
int load_virtual_mem(ushort page_n) {
	FILE* f = fopen("BACKING_STORE.bin", "r");//open "virtual memory" file

	fseek(f, (long)(page_n * PAGE_SIZE), SEEK_SET);//goto logical address's page number in file
	char page[PAGE_SIZE];
	fread(&page, PAGE_SIZE, 1, f);//load page from virtual memory
	fclose(f);

	//add to page table, and get the frame to move the virtual page into
	int page_frame = add_to_page_table(page_n);

	//copy page into main memory
	for (int i = 0; i < PAGE_SIZE; i++)
		MM[PAGE_SIZE * page_frame + i] = page[i];

	//return the new page frame
	return page_frame;
}



/*
* finds a page in the page table, also handles page faults
* IN - page number
* RETURN - page frame for page number (-1 if not found [page fault])
*/
int get_page_frame(uchar page_n) {
	//check TLB first
	for (char i = 0; i < TLB_SIZE; i++) {
		if (TLB[i].page_n == page_n) {
			g_tlbhitcount++;
			return TLB[i].frame_n;
		}
	}

	//check page table
	int PThit = -1;
	for (int i = 0; i < PAGE_FRAMES; i++) {
		PT[i].ref++;//increment ref to simulate time passing for every page table entry (bad if ref can overflow, but its not possible with only 1000 total address references)
		if (PT[i].ref && PT[i].page_n == page_n) {
			PT[i].ref = 1;//page was referenced so set its lru counter to 1
			PThit = i;//index of page hit 
		}
	}
	if (PThit > -1)
		return PThit;

	//page not found so there was a page fault
	g_pagefaultcount++;

	int page_frame = load_virtual_mem(page_n);//load page from virtual memory

	//TLB miss occured so add page #/ page frame to TLB
	add_to_tlb(page_n, page_frame);

	return page_frame;
}



/*
* translates a logical address into a physical address. checks tlb for a hit, then checks page table
* IN - logical address
* RETURN - physical address
*/
int translate_address(ushort logical_addr) {
	uchar page_n = HIBYTE(logical_addr);
	uchar offset = LOBYTE(logical_addr);

	int page_frame = get_page_frame(page_n);

	return page_frame * PAGE_SIZE + offset;
}



int main(int argc, char **argv) {
	FILE* f;
	if (argc > 1) {
		f = fopen(argv[1], "r");
	}
	else
		f = fopen("addresses.txt", "r");//default filename, when no filename is specified in argurments

	if (!f) {
		printf("unable to open file\n");
		return 0;
	}

	int reference_count = 0;//counter for number of addresses in file

	FILE* out = fopen("output.txt", "wb");

	ushort logical_addr;
	while (fscanf(f, "%d", &logical_addr) != EOF) {//loop through address file, getting next logical address
		reference_count += 1;//somehow this wasn't being incremented when i was testing   ????

		int physical_addr = translate_address(logical_addr);//get physical address from logical address

		char val = MM[physical_addr];//get value of address

		fprintf(out, "Virtual address: %d Physical address: %d Value: %d\n", logical_addr, physical_addr, val);
		printf("Virtual address: %d Physical address: %d Value: %d\n", logical_addr, physical_addr, val);
	}
	fclose(out);
	fclose(f);

	reference_count = 1000;

	printf("Page fault rate: %f%%\t(%d/%d)\n", ((double)g_pagefaultcount / (double)reference_count) * 100, g_pagefaultcount, reference_count);
	printf("TLB hit rate: %f%%\t(%d/%d)\n", ((double)g_tlbhitcount / (double)reference_count) * 100, g_tlbhitcount, reference_count);

	return 0;
}