#include "arraylist.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */
static bool resize_al(arraylist_t* self){

    if(self ==NULL){
        errno = EINVAL;
        return false;
    }

    void *temp_data  = malloc(self->length*self->item_size);
    memcpy(temp_data, self->base, self->length*self->item_size);

    if(self->length==self->capacity)
        self->capacity = self->capacity*2;

    else if(self->length == (self->capacity)/2 -1){
        if(self->capacity/2 < INIT_SZ)
            return true;
        self->capacity = self->capacity/2;
    }

    if(self->base!=NULL)
        free(self->base);

    self->base = calloc(self->capacity ,self->item_size);
    memcpy(self->base, temp_data, self->length*self->item_size);

    if(temp_data!=NULL)
        free(temp_data);

    return true;
}

arraylist_t *new_al(size_t item_size){
    if(item_size==0){
        errno = EINVAL;
        return NULL;
    }
    arraylist_t *ret = malloc(sizeof(arraylist_t));
    ret->base = calloc(item_size , INIT_SZ);
    ret->capacity = INIT_SZ;
    ret->item_size = item_size;
    ret->length =0;
    ret->lock = false;
    return ret;
}

size_t insert_al(arraylist_t *self, void* data){

    if(data==NULL){
        errno = EINVAL;
        return UINT_MAX;
    }
    if(self ==NULL){
        errno = EINVAL;
        return UINT_MAX;
    }

    if(self->lock){
        while(!self->lock){
            /*Wait for it to unlock*/
        }
    }

    self->lock = true;
    if(self->length==self->capacity)
        resize_al(self);

    void * list = self->base;
    list =  (char *)list + self->item_size*self->length;
    memcpy(list , data,self->item_size);
    self->length++;

    self->lock = false;
    return self->length;
}

size_t get_data_al(arraylist_t *self, void *data){

     if(self ==NULL){
        errno = EINVAL;
        return UINT_MAX;
    }
    if(data==NULL)
        return 0;

    if(self->lock){
        while(!self->lock){
            /*Wait for it to unlock*/
        }
    }
    self->lock = true;
    void * list_data = self->base;
    for (int i = 0; i < self->length; i++){
        if(memcmp(list_data,data, self->item_size)==0){
            self->lock = false;
            return i;
        }
        list_data = (char *)list_data + self->item_size;
    }
    self->lock = false;
    errno = EINVAL;
    return UINT_MAX;
}

void *get_index_al(arraylist_t *self, size_t index){


    if(self ==NULL || self->length==0){
        errno =EINVAL;
        return NULL;
    }

    if(self->lock){
        while(!self->lock){
            /*Wait for it to unlock*/
        }
    }

    if(index >= self->length)
        index = self->length -1;
    self->lock = true;
    void * list = self->base;
    void * data = (char *)list + self->item_size*index;
    void * new_data = malloc(self->item_size);
    memcpy(new_data,data,self->item_size);
    self->lock = false;
    return new_data;
}

void *get_index_al_2(arraylist_t *self, size_t index){

    if(self ==NULL || self->length==0){
        errno =EINVAL;
        return NULL;
    }
    if(index >= self->length)
        index = self->length -1;

    if(self->lock){
        while(!self->lock){
            /*Wait for it to unlock*/
        }
    }

    self->lock =true;
    void * list = self->base;
    void * data = (char *)list + self->item_size*index;

    void * new_data = malloc(self->item_size);
    memcpy(new_data,data,self->item_size);
    self->lock =false;
    return data;
}

bool remove_data_al(arraylist_t *self, void *data){


   size_t index = get_data_al(self ,data);
   if (index == UINT_MAX){
        errno =EINVAL;
        return false;
   }
   if(self->lock){
        while(!self->lock){
            /*Wait for it to unlock*/
        }
    }
    self->lock = true;
   for(int i=index; i<self->length;i++){
        char * list_data = (char *)((char *)self->base + i*self->item_size);
        memcpy(list_data , (char *)(list_data+self->item_size), self->item_size);
   }
   self->length--;
   resize_al(self);
   self->lock=false;

    return true;
}

void *remove_index_al(arraylist_t *self, size_t index){


    void *data = get_index_al(self,index);
    if(data==NULL)
        return NULL;
    void * new_data = malloc(self->item_size);
    memcpy(new_data,data,self->item_size);
    remove_data_al(self, data);

    return new_data;
}

void delete_al(arraylist_t *self, void (*free_item_func)(void*)){

    if(self==NULL){
        errno =EINVAL;
        return;
    }
    if(free_item_func!=NULL)
    {
        while (self->length>0)
        {
           void * data =get_index_al_2(self,0);
           free_item_func(data);
        }
    }
    if(self->base !=NULL)
        free(self->base);
    if(self!=NULL)
        free(self);

    return;
}


