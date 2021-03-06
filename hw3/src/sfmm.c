/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

/*Basic Macros and constants*/
/* I used WSIZE and DSIZE to keep the implementation similar
 * to textbook and easier to understand.
*/
#define WSIZE 8
#define DSIZE (2 * WSIZE)
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned long *)(p))
#define PUT(p, val) (*(unsigned long *)(p) = (val))

/*Get the Size an Allocation of the bits*/
#define GET_SIZE(p)   ((GET(p) & 0xFFFFFFF0))
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_SPLINTER (p) (GET(p) & 0x4)

/*Computes the address of the footer from the Header*/
#define FTRP(bp) (( char *)(bp) + GET_SIZE(bp)- WSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLOCK(bp)  ((char *)(bp) + GET_SIZE(bp))
#define PREV_BLOCK(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - WSIZE))

/*Get the pointer to Next Block*/
#define GET_NEXT_PTR(bp)  (*((char **)(bp) + GET_SIZE(bp)))
#define GET_PREV_PTR(bp) (*(char **)(bp) - GET_SIZE((char *)(bp) - WSIZE))

/*To Keep track of all memory operations*/
static size_t allocatedBlocks =0;
static size_t coalesces= 0;
static double aggregate_payload =0.0;
static double max_payload =0.0;

/* */

static void *coalesce(void *bp);

static void *extend_heap(size_t words);

static void *find_fit(size_t asize);

static void place (void *bp, size_t asize  ,size_t rsize , short flag);

static void freelist_insertion(sf_free_header *newblock);

static void freelist_removal(sf_free_header *block);

size_t allignBlock(size_t size);

void initHeader(sf_header *initHead);
void initFooter(sf_footer *initFooter);

int freeListLength();
int totalListLength();

void printEntireList();
void printFreeList();

/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head =NULL;
static void* sf_heap_listp = NULL ;

void *sf_malloc(size_t size) {



	if(sf_sbrk(0)==NULL){
		sf_mem_init();
	}

	size_t asize;
  	size_t extendsize;
  	void *bp;

  	/* Empty Case & Out of range Case*/
  	if (size <= 0 ){
  		errno =EINVAL;
		return (NULL);
  	}
  	if(size > 16384){
  		errno =ENOMEM;
  		return(NULL);
  	}

  	asize = allignBlock(size);

		allocatedBlocks++;
		aggregate_payload +=size;
		max_payload = MAX(max_payload,aggregate_payload);

	if ((bp = find_fit(asize)) != NULL){

    		place(bp, asize , size , 0);
    		//printEntireList();
    		return ((char *)bp +WSIZE);
  }


  	extendsize = MAX(asize, CHUNKSIZE);
  	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    	return (NULL);

  	place(bp, asize , size , 0);

  	/*Visualizing*/
  	//printEntireList();

	return ((char *)bp + WSIZE);

}

void *sf_realloc(void *ptr, size_t size) {

	// /*Visualizing*/


	sf_header * block_header = (sf_header *)((char *)ptr - WSIZE);
	sf_footer * block_footer = (sf_footer *)FTRP(block_header);

	/*Checking for all invalid ptrs*/
	if(ptr == NULL){
		errno = EINVAL;
		return (NULL);
	}
	if(block_header->alloc!=1 || block_footer->alloc !=1){
		errno = EINVAL;
		return (NULL);
	}
	if(block_header->block_size !=block_footer->block_size){
		errno =EINVAL;
		return (NULL);
	}
	if(block_footer->splinter!=block_header->splinter){
		errno =EINVAL;
		return (NULL);
	}

	/* Empty Case & Out of range Case*/
  	if (size <= 0 ||size > 16384){
  		errno =EINVAL;
		return (NULL);
  	}

  	/*Divided it in 2 parts*/
  	size_t asize = allignBlock(size);
  	size_t current_size = block_header->block_size*16 - DSIZE;
  	aggregate_payload = aggregate_payload - block_header->requested_size
  							+size;
  	max_payload = MAX(max_payload,aggregate_payload);

  	/*Part 1: Splitting*/
  	if(asize<=current_size){
  		/*It is the same case while adding*/
  		place(block_header,asize , size ,!GET_ALLOC(NEXT_BLOCK(block_header)));
		//printEntireList();
  		return ((char *)block_header +WSIZE);
  	}
  	/*Part 2: Increasing*/
  	/*If next Block is free*/
  	if(!GET_ALLOC(NEXT_BLOCK(block_header)) &&
  		GET_SIZE(NEXT_BLOCK(block_header))+current_size >=asize){

  		sf_header * next_free_block = ((sf_header *)NEXT_BLOCK(block_header));

  		block_header->block_size += next_free_block->block_size;
		freelist_removal((sf_free_header *)next_free_block);
		((sf_footer *)FTRP(block_header))->block_size = block_header->block_size;
		place(block_header,asize,size,0);
		//printEntireList();
		return ((char *)block_header + WSIZE);
  	}
  	else{
  		/*No Splitting or Merging can be done.
  		* Malloc needs to be called.
  		*/
  		void * realloc = sf_malloc(size);
  		memcpy(realloc , ptr, current_size);
  		sf_free(ptr);
  		//printEntireList();
  		return realloc;
  	}

}

void sf_free(void* ptr) {

	/*Checking for all invalid ptrs*/
	if(ptr == NULL){
		errno = EINVAL;
		return;
	}

	sf_header * block_header = (sf_header *)((char *)ptr - WSIZE);
	sf_footer * block_footer = (sf_footer *)FTRP(block_header);

	if(block_header->alloc!=1 || block_footer->alloc !=1){
		errno = EINVAL;
		return;
	}
	if(block_header->block_size !=block_footer->block_size){
		errno =EINVAL;
		return;
	}
	if(block_footer->splinter!=block_header->splinter){
		errno =EINVAL;
		return;
	}

	/*Saving the size of block*/
	size_t save_size = block_header->block_size;
	/*Memory Part*/
	aggregate_payload= aggregate_payload-((save_size -1)*16);
	allocatedBlocks--;

	/*Setting all parameters to 0*/
	initHeader(block_header);
	initFooter(block_footer);


	block_header->block_size = save_size;
	block_footer->block_size = save_size;
	/*Creating new Free Block adding to the list*/
	sf_free_header * freed_block = (sf_free_header *)block_header;
	freed_block->header = *block_header;
	freed_block->next = NULL;
	freed_block->prev = NULL;
	freelist_insertion(freed_block);
	coalesce(freed_block);

	/*Visualizing*/
  	//printEntireList();

}

int sf_info(info* ptr) {
	size_t heapSize =0;

	ptr->allocatedBlocks=allocatedBlocks;
	ptr->splinterBlocks= 0;
	ptr->padding =0;
	ptr->splintering =0;
	ptr->coalesces =coalesces;
	ptr->peakMemoryUtilization =0.0;

	void * cursor = sf_heap_listp;

	if(cursor==NULL)
		return -1;

	for (int i = 0; i < totalListLength(); i++)
	{
		if(GET_ALLOC(cursor)==1){
			ptr->splinterBlocks += ((sf_header*) cursor)->splinter;
			ptr->padding +=((sf_header*) cursor)->padding_size;
			ptr->splintering +=((sf_header*) cursor)->splinter_size;
		}

		/*The actual heap size is times 16*/
		heapSize+= ((sf_header*) cursor)->block_size *16;
	    cursor = NEXT_BLOCK(cursor);

	}

	ptr->peakMemoryUtilization = max_payload/heapSize;



	/*Delete before Submitting*/
	// sf_snapshot(true);
	// printf("Max Payload: %lf\n",max_payload);
	// printf("Heap Size: %ld\n",heapSize);
	/* */
	return 0;
}

static void *extend_heap(size_t words) {
  void *bp;
  size_t size;

  size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;

  if (size < 32){
    size = 32;
  }
  /* call for more memory space */
  if ((long)(bp = sf_sbrk(size)) == -1){
    return NULL;
  }


  /* Initialize free block header*/
  initHeader(&((sf_free_header *)bp)->header);
  ((sf_free_header *)bp)->header.block_size = (size)/16;
  ((sf_free_header *)bp)->next =NULL;
  ((sf_free_header *)bp)->prev = NULL;
  freelist_insertion(bp);

  if(sf_heap_listp==NULL){
  		sf_heap_listp = bp;
	}
  /* coalesce bp with next and previous blocks */
  return coalesce(bp);
}


 static void *find_fit(size_t asize){

	void *bp =NULL;
	sf_free_header *cursor = freelist_head;

	while(cursor!=NULL){
		if(cursor->header.block_size*DSIZE-DSIZE >=asize){
			if(bp==NULL){
				bp = &(cursor->header);
			}
			else{
				if(((sf_header *) bp)->block_size
					-cursor->header.block_size >0)
					bp = cursor;
			}
		}
		cursor = cursor->next;
	}
	return bp;
}

static void *coalesce (void* bp){

	sf_free_header * current = (sf_free_header *)bp;
	/*Setting them a random positvie value to avoid*/
	size_t next_Alloc = 10;
	size_t prev_Alloc = 10;
	coalesces++;

	sf_free_header * next_free_block =  current->next;
	sf_free_header * prev_free_block = current->prev;
	/*Checking for Special Cases*/
	if(next_free_block!=NULL)
		next_Alloc = &next_free_block->header - (sf_header *)NEXT_BLOCK(current);

	if(prev_free_block!=NULL)
		prev_Alloc = &current->header - (sf_header *)NEXT_BLOCK(prev_free_block);

	/*Only NEXT is free*/
	if(!next_Alloc && prev_Alloc){
		current->header.block_size += next_free_block->header.block_size;
		freelist_removal(next_free_block);
		((sf_footer *)FTRP(current))->block_size =current->header.block_size;
	}
	/*Only PREVOIUS is free*/
	else if(next_Alloc && !prev_Alloc){
		prev_free_block->header.block_size +=current->header.block_size;
		freelist_removal(current);
		bp =prev_free_block;
		((sf_footer *)FTRP(bp))->block_size =prev_free_block->header.block_size;
	}
	/*Both Free*/
	else if(!next_Alloc && !prev_Alloc){
		current->header.block_size += next_free_block->header.block_size;
		prev_free_block->header.block_size += current->header.block_size;

		freelist_removal(next_free_block);
		freelist_removal(current);
		bp =prev_free_block;
		((sf_footer *)FTRP(bp))->block_size =prev_free_block->header.block_size;
	}
	else{
		coalesces--;
	}

	return bp;
}

static void place(void *bp, size_t asize , size_t rsize , short flag){
	sf_header * bheader = (sf_header *)bp;
	sf_footer * bfooter =NULL;

	if(bheader->alloc ==0)
		freelist_removal(bp);


	/*Accounting_for padding*/
	int padding = asize - rsize;
	bheader->padding_size = padding;
	bheader->requested_size = rsize;


	/* Check for Splinters*/
	int splinter_size = bheader->block_size*16 -asize -DSIZE;




	if(splinter_size >= 32 || flag==1){

		bfooter = (sf_footer *)((char *)bheader + asize + WSIZE);
		initFooter(bfooter);

		bheader->alloc =1;
		bfooter->alloc =1;

		bheader->block_size = (asize+DSIZE)/16;
		bfooter->block_size = (asize+DSIZE)/16;

		/*Special case only when next block is free and there is a splinter*/
		if(splinter_size < 32){
			sf_free_header  * adjacentBlock = (sf_free_header *)((char *)bheader+
														splinter_size+ asize + DSIZE);
			sf_header *saveheader = &adjacentBlock->header;
			sf_free_header* saveNext = adjacentBlock->next;
			sf_free_header* savePrev = adjacentBlock->prev;

			sf_free_header * nptr = (sf_free_header *)((char *)bheader+ asize + DSIZE);
			nptr->header.block_size = (splinter_size)/16 + saveheader->block_size;
		  	nptr->next =saveNext;
		  	nptr->prev = savePrev;
		  	if(saveNext!=NULL)
		  		saveNext->prev = nptr;
		  	if(savePrev!=NULL)
		  		savePrev->next = nptr;

		  	((sf_footer *)FTRP(nptr))->block_size =nptr->header.block_size;
		  	return;
			}

		/*No Splinters, Hence Split the block*/
		size_t new_size = splinter_size;
		sf_free_header * nptr = (sf_free_header *)((char *)bheader+ asize + DSIZE);
		/* Initialize free block header*/
		  initHeader(&nptr->header);
		  nptr->header.block_size = (new_size)/16;
		  nptr->next =NULL;
		  nptr->prev = NULL;

		  freelist_insertion(nptr);
		  coalesce(nptr);
	}
	else{
		bfooter = (sf_footer *)FTRP(bheader);
		bheader->alloc =1;
		bfooter->alloc =1;

		bfooter->block_size = bheader->block_size;
		bheader->splinter =1;
		bfooter->splinter =1;
		bheader->splinter_size = splinter_size;

	}
}

static void freelist_insertion(sf_free_header *newblock){

	sf_free_header *cursor = freelist_head;

	sf_footer *  newblock_footer = (sf_footer *)FTRP(newblock);
	initFooter(newblock_footer);
	newblock_footer->block_size = newblock->header.block_size;

	if(freelist_head ==NULL){
		freelist_head = newblock;
		return;
	}

	if(cursor>newblock){
		newblock->next = cursor;
		cursor->prev = newblock;
		freelist_head = newblock;
		return;
	}

	while(cursor<newblock && cursor->next !=NULL){
		cursor = cursor->next;
	}

	/*For the case where tail addition needs to be done*/
	if(cursor->next ==NULL && cursor<newblock){
		cursor->next = newblock;
		newblock->prev = cursor;
		return;
	}
	/*Addition before the cursor*/
	sf_free_header *prev_cursor= cursor->prev;
	prev_cursor ->next = newblock;
	newblock->prev = prev_cursor;

	newblock->next = cursor;
	cursor->prev = newblock;
}

static void freelist_removal(sf_free_header *block){

	if(block == freelist_head){
		freelist_head = block->next;
		block->next = NULL;
		return;
	}

	if(block->next==NULL){
		block->prev->next = NULL;
		//block->prev= NULL;
		return;
	}

	block->prev->next = block->next;
	block->next->prev = block->prev;
	//block->next = NULL;
	//block->prev = NULL;
}

void initHeader(sf_header *initHead){
	initHead->alloc =0;
	initHead->splinter =0;
	initHead->block_size =0;
	initHead->requested_size =0;
	initHead->unused_bits =0;
	initHead->splinter_size =0;
	initHead-> padding_size=0;
}

void initFooter(sf_footer *initFooter){
	initFooter->alloc =0;
	initFooter->splinter =0;
	initFooter->block_size =0;
}

size_t allignBlock(size_t size){
	size_t asize;
/* Allign the Blocks */
	if(size %16==0)
		return size;

  	if (size <= DSIZE)
    	asize = DSIZE;
  	else
		asize = DSIZE * ((size + DSIZE) / DSIZE);

	return asize;
}

int freeListLength(){
	int counter = 0;
	sf_free_header * cursor = freelist_head;
	while(cursor!=NULL){
    counter++;
    cursor = cursor->next;
	}
	return counter;
}
int totalListLength(){
	return freeListLength() +allocatedBlocks;
}
void printEntireList(){
	void * cursor = sf_heap_listp;

	printf("---------------ENTIRE LIST----------------------------\n");
	for( int i =0 ; i< totalListLength();i++){
	    sf_blockprint(cursor);
	    cursor = NEXT_BLOCK(cursor);
	}
	printf("---------------ENTIRE LIST----------------------------\n");
}

void printFreeList(){
	sf_free_header * cursor = freelist_head;

	printf("---------------FREE LIST----------------------------\n");
	while(cursor!=NULL){
	    sf_blockprint(cursor);
	    cursor = cursor->next;
	}
	printf("---------------FREE LIST----------------------------\n");
}
