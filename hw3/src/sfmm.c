/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include "helper.h"
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


#define FTRP(bp) (( char *)(bp) + GET_SIZE(bp)- WSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLOCK(bp)  ((char *)(bp) + GET_SIZE(bp))
#define PREV_BLOCK(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - WSIZE))


static void *coalesce(void *bp);

static void *extend_heap(size_t words);

static void *find_fit(size_t asize);

static void place (void *bp, size_t asize  ,size_t rsize , short flag);

static void freelist_insertion(sf_free_header *newblock);

static void freelist_removal(sf_free_header *block);

size_t allignBlock(size_t size);

void initHeader(sf_header *initHead);
void initFooter(sf_footer *initFooter);

/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head =NULL;
static void* sf_heap_listp ;

void *sf_malloc(size_t size) {


	sf_heap_listp = sf_sbrk(0);

	// if(sf_sbrk(0)==NULL){

	// 	sf_mem_init();
	// 	sf_heap_listp=sf_sbrk(0);

	// 	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
	// 		return NULL;
	// 	freelist_head = sf_heap_listp;
	// 	initHeader(&freelist_head->header);
	// 	freelist_head->next =NULL;
	// 	freelist_head->prev = NULL;
	// }

	size_t asize;
  	size_t extendsize;
  	void *bp;

  	/* Empty Case & Out of range Case*/
  	if (size == 0 ||size > 16384){
  		errno =ENOENT;
		return (NULL);
  	}

  	asize = allignBlock(size);

	if ((bp = find_fit(asize)) != NULL){

    		place(bp, asize , size , 0);
    		return ((char *)bp +WSIZE);
  }


  	extendsize = MAX(asize, CHUNKSIZE);
  	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    	return (NULL);

  	place(bp, asize , size , 0);
	return ((char *)bp + WSIZE);

}

void *sf_realloc(void *ptr, size_t size) {

	sf_header * block_header = (sf_header *)((char *)ptr - WSIZE);
	sf_footer * block_footer = (sf_footer *)FTRP(ptr);

	/*Checking for all invalid ptrs*/
	if(ptr == NULL){
		errno = ENOENT;
		return (NULL);
	}
	if(block_header->alloc!=1 || block_footer->alloc !=1){
		errno = ENOENT;
		return (NULL);
	}
	if(block_header->block_size !=block_footer->block_size){
		errno =ENOENT;
		return (NULL);
	}
	if(block_footer->splinter!=block_header->splinter){
		errno =ENOENT;
		return (NULL);
	}

	/* Empty Case & Out of range Case*/
  	if (size == 0 ||size > 16384){
  		errno =ENOENT;
		return (NULL);
  	}

  	/*Divided it in 2 parts*/
  	size_t asize = allignBlock(size);
  	size_t current_size = block_header->block_size*16 - DSIZE;


  	/*Part 1: Splitting*/
  	if(asize<=current_size){
  		/*It is the same case while adding*/
  		place(block_header,asize , size ,!GET_ALLOC(NEXT_BLOCK(ptr)));
  		return ((char *)block_header +WSIZE);
  	}
  	/*Part 2: Increasing*/
  	/*If next Block is free*/
  	if(!GET_ALLOC(NEXT_BLOCK(ptr)) &&
  		GET_SIZE(NEXT_BLOCK(ptr))+current_size >=asize){

  		block_header->block_size += ((sf_header *)NEXT_BLOCK(ptr))->block_size;
		freelist_removal((sf_free_header *)NEXT_BLOCK(ptr));
		((sf_footer *)FTRP(block_header))->block_size = block_header->block_size;
		place(block_header,asize,size,0);
		return ((char *)block_header + WSIZE);
  	}
  	else{
  		/*No Splitting or Merging can be done.
  		* Malloc needs to be called.
  		*/
  		void * realloc = sf_malloc(size);
  		memcpy(realloc , ptr, current_size);
  		sf_free(ptr);
  		return realloc;
  	}

}

void sf_free(void* ptr) {

	/*Checking for all invalid ptrs*/
	if(ptr == NULL){
		errno = ENOENT;
		return;
	}

	sf_header * block_header = (sf_header *)((char *)ptr - WSIZE);
	sf_footer * block_footer = (sf_footer *)FTRP(ptr);

	if(block_header->alloc!=1 || block_footer->alloc !=1){
		errno = ENOENT;
		return;
	}
	if(block_header->block_size !=block_footer->block_size){
		errno =ENOENT;
		return;
	}
	if(block_footer->splinter!=block_header->splinter){
		errno =ENOENT;
		return;
	}

	/*Saving the size of block*/
	size_t save_size = block_header->block_size;

	/*Setting all parameters to 0*/
	initHeader(block_header);
	initFooter(block_footer);

	block_header->block_size = save_size;
	block_footer->block_size = save_size;
	/*Creating new Free Block adding to the list*/
	sf_free_header * freed_block = (sf_free_header *)ptr;
	freed_block->header = *block_header;
	freed_block->next = NULL;
	freed_block->prev = NULL;
	freelist_insertion(freed_block);
	coalesce(freed_block);

}

int sf_info(info* ptr) {
	return -1;
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
		((sf_footer *)FTRP(bp))->block_size =current->header.block_size;
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
	int splinter_size = bheader->block_size*16 -asize;

	if(splinter_size >= 32 || flag==1){

		bfooter = (sf_footer *)((char *)bheader + asize+ WSIZE);
		initFooter(bfooter);

		bheader->alloc =1;
		bfooter->alloc =1;
		bheader->block_size = (asize+DSIZE)/16;
		bfooter->block_size = (asize+DSIZE)/16;

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
		bfooter = (sf_footer *)((char *)bheader + asize+ WSIZE +splinter_size);
		bheader->alloc =1;
		bfooter->alloc =1;
		bheader->block_size = (asize+DSIZE + splinter_size)/16;
		bfooter->block_size = (asize+DSIZE + splinter_size)/16;
		bheader->splinter =1;
		bfooter->splinter =1;
		bheader->splinter_size = splinter_size;

	}
}

static void freelist_insertion(sf_free_header *newblock){

	sf_free_header *cursor = freelist_head;
	sf_footer * newblock_footer = (sf_footer *)FTRP(newblock);

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

	if(cursor->next ==NULL){
		cursor->next = newblock;
		newblock->prev = cursor;
		return;
	}

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
  	if (size <= DSIZE)
    	asize = DSIZE;
  	else
		asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);

	return asize;
}

