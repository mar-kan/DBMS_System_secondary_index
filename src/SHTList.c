#include "../include/SHTList.h"


int SListInsert(SHtList * list, SHT_info * info)
{
    if (!list)
    {
        list = malloc(sizeof(SHtList));
        list->head = malloc(sizeof(SHtList));
    }

    //creates a list node with given HP_info
    SHtNode * node = malloc(sizeof(SHtNode));
    node->info = info;
    node->next = NULL;
    //adds new node at the beginning of the list for speed
    if (list->head)
        node->next = list->head;
    list->head = node;

    return 0;
}


int SListRemove(SHtList * list, char * info_id)
{
    if (!list || !list->head)
        return -1;

    SHtNode * temp, * node = list->head;
    while (node)
    {
        if (strcmp(node->info->attrName, info_id) == 0)
        {
            temp->next = node->next;
            free(node);
            return 0;
        }
        temp = node;    //stores previous node
        node = node->next;
    }

    return 0;
}


int SListDestroy(SHtList * list)
{
    if (!list || !list->head)
        return -1;

    SHtNode * temp, * node;
    temp = list->head;
    while(temp)
    {
        node = temp;
        temp = temp->next;

        //free(node->info); //destroyed by SHT_CloseSecondaryIndex
        free(node);
    }
    return 0;
}