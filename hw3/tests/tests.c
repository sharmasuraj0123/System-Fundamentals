#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(sizeof(int));
  *x = 4;
  cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
  void *pointer = sf_malloc(sizeof(short));
  sf_free(pointer);
  pointer = (char*)pointer - 8;
  sf_header *sfHeader = (sf_header *) pointer;
  cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
  sf_footer *sfFooter = (sf_footer *) ((char*)pointer + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, SplinterSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini){
  void* x = sf_malloc(32);
  void* y = sf_malloc(32);
  (void)y;

  sf_free(x);

  x = sf_malloc(16);

  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->splinter == 1, "Splinter bit in header is not 1!");
  cr_assert(sfHeader->splinter_size == 16, "Splinter size is not 16");

  sf_footer *sfFooter = (sf_footer *)((char*)sfHeader + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->splinter == 1, "Splinter bit in header is not 1!");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  memset(x, 0, 0);
  cr_assert(freelist_head->next == NULL);
  cr_assert(freelist_head->prev == NULL);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  int *y = sf_malloc(4);
  memset(y, 0, 0);
  sf_free(x);

  //just simply checking there are more than two things in list
  //and that they point to each other
  cr_assert(freelist_head->next != NULL);
  cr_assert(freelist_head->next->prev != NULL);
}

//#
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//#
Test(sf_memsuite, Padding_present_or_not, .init = sf_mem_init, .fini = sf_mem_fini) {

 void* x = sf_malloc(28);

  sf_header *sfHeader = (sf_header *)((char*)x - 8);

  cr_assert(sfHeader->padding_size == 4, "Padding Size in header is not 4!");

  x = sf_malloc(32);
  sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->padding_size == 0, "Padding Size in header is not 4!");

}

Test(sf_memsuite, Realloc_blocks_splits_or_not, .init = sf_mem_init, .fini = sf_mem_fini)
{
  void *x =sf_malloc(240);
  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  sf_realloc(x,4);
  sf_header *new_sfHeader = (sf_header *)((char*)x - 8);
  sf_footer *sfFooter = (sf_footer *)((char*)sfHeader + (new_sfHeader->block_size << 4) - 8);

  cr_assert(new_sfHeader==sfHeader, "The new block isn't in the same position");
  cr_assert(new_sfHeader->block_size==2,"The new Block size doesn't match");
  cr_assert(sfFooter->block_size == new_sfHeader->block_size, "Block Size doesn't match in header and footer");
  cr_assert(sfFooter->alloc==1,"The Allocation bit of footer is not 1!");
  cr_assert(new_sfHeader->alloc==1,"The Allocation bit of header is not 1!");
}

Test(sf_memsuite, info_function, .init = sf_mem_init, .fini = sf_mem_fini)
{

  info * testInfo = sf_malloc(sizeof(info));
   void *val = sf_malloc(9);
    void *val2 = sf_malloc(90);
    void *val3 = sf_malloc(98);
    void *val4 = sf_malloc(90);
    void *val5 = sf_malloc(96);
    void *val6 = sf_malloc(944);
    void *val7 = sf_malloc(19);
    void *val8 = sf_malloc(94);


    double peakMemoryUtilization = 1488.0/4096.0;
        sf_info(testInfo);

        cr_assert(testInfo->allocatedBlocks ==9, "The number of Allocated Blocks is not 9");
        cr_assert(testInfo->splinterBlocks==0, "Number of Splinter Blocks is not 0");
        cr_assert(testInfo->padding==48, "Padding Bytes is not 48");
        cr_assert(testInfo->splintering==0, "Splintering Bytes is not 0");
        cr_assert(testInfo->coalesces==0, "Number of coalescings is not 0");
        cr_assert(testInfo->peakMemoryUtilization==peakMemoryUtilization, " The Peak Memory Utilization is not %lf" ,peakMemoryUtilization);

        sf_free(val);
        sf_free(val2);
        sf_free(val3);
        sf_free(val4);
        sf_free(val5);
        sf_free(val6);
        sf_free(val7);
        sf_free(val8);

        sf_info(testInfo);
        cr_assert(testInfo->allocatedBlocks ==1, "The number of Allocated Blocks is not 1");
        cr_assert(testInfo->splinterBlocks==0, "Number of Splinter Blocks is not 0");
        cr_assert(testInfo->padding==0, "Padding Bytes is not 0");
        cr_assert(testInfo->splintering==0, "Splintering Bytes is not 0");
        cr_assert(testInfo->coalesces==7, "Number of coalescings is not 7");
        cr_assert(testInfo->peakMemoryUtilization==peakMemoryUtilization, " The Peak Memory Utilization is not %lf" ,peakMemoryUtilization);

        val = sf_malloc(16);
        val2 = sf_malloc(96);
        val3 = sf_malloc(256);
        sf_realloc(val2,80);
        sf_free(val3);

        sf_info(testInfo);
        cr_assert(testInfo->allocatedBlocks ==3, "The number of Allocated Blocks is not 4");
        cr_assert(testInfo->splinterBlocks==1, "Number of Splinter Blocks is not 1");
        cr_assert(testInfo->padding==0, "Padding Bytes is not 0");
        cr_assert(testInfo->splintering==16, "Splintering Bytes is not 16");
        cr_assert(testInfo->coalesces==8, "Number of coalescings is not 7");
        cr_assert(testInfo->peakMemoryUtilization==peakMemoryUtilization, " The Peak Memory Utilization is not %lf" ,peakMemoryUtilization);

}

