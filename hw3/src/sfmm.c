/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>


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

#define HDRP(bp)  ((unsigned long *)(bp) - WSIZE)
#define FTRP(bp) ((unsigned long *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLOCK(bp)  ((unsigned long *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLOCK(bp) ((unsigned long *)(bp) - GET_SIZE((unsigned long *)(bp) - DSIZE))


static void *coalesce(void *bp);

static void *extend_heap(size_t words);

static void *find_fit(size_t asize);

static void place (void *bp, size_t asize  ,size_t rsize);

static void freelist_insertion(sf_free_header *newblock);

static void freelist_removal(sf_free_header *block);


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

	if(sf_sbrk(0)==NULL){

		sf_mem_init();
		sf_heap_listp=sf_sbrk(0);

		if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
			return NULL;
		freelist_head = sf_heap_listp;
		initHeader(&freelist_head->header);
		freelist_head->next =NULL;
		freelist_head->prev = NULL;
	}

	size_t asize;
  	size_t extendsize;
  	void *bp;

  	/* Empty Case. */
  	if (size == 0)
		return (NULL);


  	/* Allign the Blocks */
  	if (size <= DSIZE)
    	asize = 2 * DSIZE;
  	else
		asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);


	if ((bp = find_fit(asize)) != NULL){

    		place(bp, asize , size);
    		return (bp);
  }


  	extendsize = MAX(asize, CHUNKSIZE);
  	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    	return (NULL);

  	place(bp, asize , size);
	return (bp);

}

void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void* ptr) {
	return;
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
  ((sf_free_header *)bp)->header.block_size = size - DSIZE;
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
		if(cursor->header.block_size >=asize){
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
return bp;
}

static void place(void *bp, size_t asize , size_t rsize){
	sf_header * bheader = &((sf_free_header *)bp)->header;
	sf_footer * bfooter =NULL;

	/*Accounting_for padding*/
	int padding = asize - rsize;
	bheader->padding_size = padding;
	bheader->requested_size = rsize;

	/* Check for Splinters*/
	int splinter_size =bheader->block_size -asize;

	if(splinter_size >= 32){

		bfooter = (sf_footer *)((char *)bp + asize+ WSIZE);
		initFooter(bfooter);

		bheader->alloc =1;
		bfooter->alloc =1;
		bheader->block_size = asize+DSIZE;
		bfooter->block_size = asize+DSIZE;

		/*No Splinters, Hence Split the block*/
		size_t new_size = splinter_size;
		sf_free_header * nptr = (sf_free_header *)((char *)bp+ asize + DSIZE);
		/* Initialize free block header*/
		  initHeader(&nptr->header);
		  nptr->header.block_size = new_size - DSIZE;
		  nptr->next =NULL;
		  nptr->prev = NULL;

		  freelist_removal(bp);
		  freelist_insertion(nptr);
	}
	else{
		bfooter = (sf_footer *)((char *)bp + asize+ WSIZE +splinter_size);
		bheader->alloc =1;
		bfooter->alloc =1;
		bheader->block_size = asize+DSIZE + splinter_size;
		bfooter->block_size = asize+DSIZE + splinter_size;
		bheader->splinter =1;
		bfooter->splinter =1;
		bheader->splinter_size = splinter_size;
		freelist_removal(bp);
	}
}

static void freelist_insertion(sf_free_header *newblock){

	sf_free_header *cursor = freelist_head;

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
		//freelist_head->prev = NULL;
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

