#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdio.h>
#include <stdbool.h>
#include "arraylist.h"

/******************************************
 *                  ITEMS                 *
 ******************************************/
arraylist_t *global_list;

typedef struct {
    char* name;
    int32_t id;
    double gpa;
}student_t;

typedef struct{
    int i;
    float f;
    long double ld;
    char c1:4;
    char c2:4;
    short s;
    void *some_data;
}test_item_t;

/******************************************
 *              HELPER FUNCS              *
 ******************************************/
void test_item_t_free_func(void *argptr){
    test_item_t* ptr = (test_item_t*) argptr;
    if(!ptr)
        free(ptr->some_data);
    else
        cr_log_warn("%s\n", "Pointer was NULL");
}

void setup(void) {
    cr_log_warn("Setting up test");
    global_list = new_al(sizeof(test_item_t));
}

void teardown(void) {
    cr_log_error("Tearing down");
    delete_al(global_list, test_item_t_free_func);
}

bool compareStudents(student_t *s1, student_t *s2){
    if(strcmp(s1->name,s2->name)!=0)
        return false;
    if(s1->id != s2->id)
        return false;
    if(s1->gpa!= s2->gpa)
        return false;
    return true;
}

/******************************************
 *                  TESTS                 *
 ******************************************/
Test(al_suite, 0_creation, .timeout=2){

    arraylist_t *locallist = new_al(sizeof(student_t));
    cr_assert_not_null(locallist, "List returned was NULL");
    cr_assert(locallist->length==0,"length is incorrect!");
    cr_assert(locallist->capacity==INIT_SZ,"Capacity is incorrect!");
    cr_assert(locallist->item_size==sizeof(student_t),"item_size is incorrect!");

}

Test(al_suite, 1_deletion, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");

    delete_al(locallist, test_item_t_free_func);

    cr_assert(true, "Delete completed without crashing");
}

Test(al_suite, 2_insertion, .timeout=2, .init=setup, .fini=teardown){

    arraylist_t *locallist = new_al(sizeof(student_t));
    student_t * newStudent = malloc(sizeof(student_t));
    student_t * newStudent2 = malloc(sizeof(student_t));
    student_t * newStudent3 = malloc(sizeof(student_t));
    /*ONLY use this for testing*/
    student_t * testStudent = malloc(sizeof(student_t));

    newStudent->name ="Yello";
    newStudent->gpa = 3.0;
    newStudent->id = 3;
    insert_al(locallist,newStudent);

    newStudent->name = "TEST";


    testStudent = get_index_al(locallist,0);
    cr_assert(!compareStudents(testStudent,newStudent),"Student1 is same");
    newStudent->name ="Yello";
    testStudent->name = "TESTS";

    newStudent2 = get_index_al(locallist,0);
    //printf("%s\n",newStudent2->name);

    /*Test for just one student*/
    newStudent2->name ="Yello2";
    newStudent2->gpa = 3.2;
    newStudent2->id = 32;

    insert_al(locallist,newStudent2);

    newStudent3->name ="Yello3";
    newStudent3->gpa = 3.3;
    newStudent3->id = 33;
    insert_al(locallist,newStudent3);


    /*Test with student3 for getting from data*/
    size_t index = get_data_al(locallist,newStudent);
    cr_assert(index==0,"Index should be 0");
    index = get_data_al(locallist,newStudent2);
    cr_assert(index==1,"Index should be 1");
    index = get_data_al(locallist,newStudent3);
    cr_assert(index==2,"Index should be 2");
    cr_assert(locallist->length==3,"length is incorrect! not 3");


    /*Test for getting from index*/
    testStudent = (student_t *) get_index_al(locallist,0);
    cr_assert(compareStudents(testStudent,newStudent),"Student1 is not same");
    testStudent = (student_t *) get_index_al(locallist,1);
    cr_assert(compareStudents(testStudent,newStudent2),"Student2 is not same");
    testStudent = (student_t *) get_index_al(locallist,23);
    cr_assert(compareStudents(testStudent,newStudent3),"Student3 is not same");



}

Test(al_suite, 3_removal, .timeout=2, .init=setup, .fini=teardown){

    arraylist_t *locallist = new_al(sizeof(student_t));
    student_t * newStudent = malloc(sizeof(student_t));
    student_t * newStudent2 = malloc(sizeof(student_t));
    student_t * newStudent3 = malloc(sizeof(student_t));
    student_t * newStudent4 = malloc(sizeof(student_t));
    /*ONLY use this for testing*/
    student_t * testStudent = malloc(sizeof(student_t));

    newStudent->name ="Yello";
    newStudent->gpa = 3.0;
    newStudent->id = 3;
    insert_al(locallist,newStudent);

    newStudent2->name ="Yello2";
    newStudent2->gpa = 3.2;
    newStudent2->id = 32;

    insert_al(locallist,newStudent2);


    newStudent3->name ="Yello3";
    newStudent3->gpa = 3.3;
    newStudent3->id = 33;
    insert_al(locallist,newStudent3);

    newStudent4->name ="Yello4";
    newStudent4->gpa = 3.4;
    newStudent4->id = 34;
    insert_al(locallist,newStudent4);


    testStudent->name ="Yello2";
    testStudent->gpa = 3.2;
    testStudent->id = 32;

    /*Test for remove from data*/
    cr_assert(remove_data_al(locallist,newStudent),"Item wasn't removed");
    cr_assert(locallist->length==3,"length is incorrect! not 3");

    testStudent = (student_t *) get_index_al(locallist,0);
    cr_assert(compareStudents(testStudent,newStudent2),"Student2 is not moved");
    testStudent = (student_t *) get_index_al(locallist,1);
    cr_assert(compareStudents(testStudent,newStudent3),"Student3 is not moved");
    testStudent = (student_t *) get_index_al(locallist,2);
    cr_assert(compareStudents(testStudent,newStudent4),"Student4 is not moved");

    remove_data_al(locallist,newStudent3);
    remove_data_al(locallist,newStudent2);
    remove_data_al(locallist,newStudent4);

    testStudent = get_index_al(locallist,3);
    //printf("%s\n", testStudent->name);
    if(testStudent!=NULL)
    printf("%s\n",testStudent->name);

   // // cr_assert(remove_data_al(locallist,newStudent),"Item wasn't removed");
   //  cr_assert(locallist->length==1,"length is incorrect! not 1");
   //  testStudent = (student_t *) get_index_al(locallist,0);
   //  cr_assert(compareStudents(testStudent,newStudent3),"Student3 is not moved");


}

Test(al_suite, index_removal, .timeout=2, .init=setup, .fini=teardown){

    arraylist_t *locallist = new_al(sizeof(student_t));
    student_t * newStudent = malloc(sizeof(student_t));
    student_t * newStudent2 = malloc(sizeof(student_t));
    student_t * newStudent3 = malloc(sizeof(student_t));
    /*ONLY use this for testing*/
    student_t * testStudent = malloc(sizeof(student_t));

    newStudent->name ="Yello";
    newStudent->gpa = 3.0;
    newStudent->id = 3;
    insert_al(locallist,newStudent);

    newStudent2->name ="Yello2";
    newStudent2->gpa = 3.2;
    newStudent2->id = 32;

    insert_al(locallist,newStudent2);


    newStudent3->name ="Yello3";
    newStudent3->gpa = 3.3;
    newStudent3->id = 33;
    insert_al(locallist,newStudent3);

    /*Test for remove index*/
    testStudent = remove_index_al(locallist,0);
    cr_assert(locallist->length==2,"length is incorrect! not 2");
    cr_assert(compareStudents(testStudent,newStudent),"Student is not correct");

    testStudent->name = "TEST";
    testStudent = get_index_al(locallist,0);
    //cr_assert(locallist->length==2,"A copy is not returned");

    testStudent = (student_t *) get_index_al(locallist,0);
    cr_assert(compareStudents(testStudent,newStudent2),"Student2 is not moved");
    testStudent = (student_t *) get_index_al(locallist,1);
    cr_assert(compareStudents(testStudent,newStudent3),"Student3 is not moved");


    testStudent = remove_index_al(locallist,1);
    cr_assert(locallist->length==1,"length is incorrect! not 1");
    cr_assert(compareStudents(testStudent,newStudent3),"Student3 is not correct");
}
