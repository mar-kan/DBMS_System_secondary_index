#ifndef SHTLIST_H
#define SHTLIST_H

#include "SHT_info.h"
#include <stdlib.h>
#include <string.h>

//list that contains pointers to secondary hash files' infos
typedef struct SHtNode
{
    SHT_info * info;
    struct SHtNode * next;
}
SHtNode;


typedef struct SHtList
{
    SHtNode * head;
}
SHtList;

int SListInsert(SHtList*, SHT_info*);
int SListRemove(SHtList*, char*);
int SListDestroy(SHtList*);


#endif
