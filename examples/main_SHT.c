#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../include/HtList.h"
#include "../include/SHTList.h"
#include "../include/utilities.h"
#include "../include/HT.h"
#include "../include//SHT.h"

#define MAX_RECORDS 1500
#define FILENAMEHT "fileHT1"
#define FILENAMESHT "fileSHT1"


int main(int argc, char** argv)
{
    char filenameHT [8];
    char filenameSHT [9];

    strcpy(filenameHT, FILENAMEHT);
    strcpy(filenameSHT, FILENAMESHT);

    BF_Init();
    HtList * ht_files = malloc(sizeof(HtList)); //list that includes all primary-key files that have been created and opened
    SHtList * sht_files = malloc(sizeof(HtList)); //list that includes all secondary-key files that have been created and opened

    Record * record = malloc(sizeof(Record));
    SecondaryRecord * secondaryRecord = malloc(sizeof(SecondaryRecord));

    char input[50];
    if (argc > 1)   //input file given as argument
        strcpy(input, argv[1]);
    else
    {   //asks for input file
        printf("Please input file path\n");
        scanf("%s",input);
    }

    //opens input file with records
    FILE * fp = fopen(input, "r");
    if (!fp)
    {
        printf("Error in opening record file\n");
        exit(-1);
    }
    chdir("block_files");    //changes dir to create block files in their own dir


    /**creates and opens an empty ht file with its name as id**/
    printf("Creating HT file: %s\n", filenameHT);
    if (HT_CreateIndex(filenameHT, 'i', filenameHT, sizeof(filenameHT), 50) < 0)
        exit(EXIT_FAILURE);

    printf("Opening HT file\n\n");
    HT_info * info_ht;
    if (!(info_ht = HT_OpenIndex(filenameHT)))
        exit(EXIT_FAILURE);
    ListInsert(ht_files, info_ht);    //inserts info in list

    /**creates and opens an empty sht file with its name as id**/
    printf("Creating SHT file: %s\n", filenameSHT);
    if (SHT_CreateSecondaryIndex(filenameSHT, filenameSHT, sizeof(filenameSHT), 50, filenameHT) < 0)
        exit(EXIT_FAILURE);

    printf("Opening SHT file\n\n");
    SHT_info * info_sht;
    if (!(info_sht = SHT_OpenSecondaryIndex(filenameSHT)))
        exit(EXIT_FAILURE);
    SListInsert(sht_files, info_sht);    //inserts info in list

    //reads file line by line and inserts each record
    size_t size = 0;
    char * line = NULL;
    printf("Inserting all records\n");
    int count=0;
    while (getline(&line, &size, fp) != EOF)
    {
        //creates record with each line read
        createRecordFromLine(line, record);

        //when a file reaches its max records creates new file and opens that for use
        if (count == MAX_RECORDS)
        {
            //creates and opens 2 new block files
            filenameHT[7]++;
            filenameSHT[8]++;

            /** HT **/
            printf("Creating HT file: %s\n", filenameHT);
            if (HT_CreateIndex(filenameHT, 'i', filenameHT, sizeof(filenameHT), 50) < 0)
                exit(EXIT_FAILURE);

            printf("Opening HT file\n\n");
            if (!(info_ht = HT_OpenIndex(filenameHT)))
                exit(EXIT_FAILURE);
            ListInsert(ht_files, info_ht);    //inserts info in list

            /** SHT **/
            printf("Creating SHT file: %s\n", filenameSHT);
            if (SHT_CreateSecondaryIndex(filenameSHT, filenameSHT, sizeof(filenameSHT), 50, filenameHT) < 0)
                exit(EXIT_FAILURE);

            printf("Opening SHT file\n\n");
            if (!(info_sht = SHT_OpenSecondaryIndex(filenameSHT)))
                exit(EXIT_FAILURE);
            SListInsert(sht_files, info_sht);    //inserts info in list

            count=0;    //resets count for each file
        }

        /** insertion in HT file and creation of secondary record **/
        secondaryRecord->record = *record;
        secondaryRecord->blockId = HT_InsertEntry(*info_ht, *record);
        if (secondaryRecord->blockId == -1)
        {
            printf("Insertion in HT failed\n");
            break;
        }

        /** insertion in SHT file **/
        if (SHT_SecondaryInsertEntry(*info_sht, *secondaryRecord) < 0)
        {
            printf("Insertion in SHT failed\n");
            break;
        }

        count++;
    }

    //picking a random primary-key that exists in all input files
    srand(time(NULL));
    int value = rand() % 1000;  //gets random int that exists in all datasets
    char * key = malloc(4);
    sprintf(key, "%d", value);

    /** searching HT files with primary key **/
    printf("\nSearching primary key %s\n", key);
    HtNode * temp = ht_files->head;
    while (temp)
    {
        int blkCnt;
        if ((blkCnt = HT_GetAllEntries(*temp->info, key)) < 0)
            printf("Searching primary key failed\n");
        else
            printf("Read %d HT blocks\n", blkCnt);

        temp = temp->next;
    }

    //building secondary key with the same value
    char * sec_key = malloc(12);
    strcpy(sec_key, "surname_");
    strcat(sec_key, key);

    printf("\nSearching secondary key %s\n", sec_key);
    SHtNode *sTemp = sht_files->head;
    while (sTemp)
    {
        int blkCnt;

        //search for ht_info with this name
        temp = ht_files->head;
        while (temp)
        {
            if (strcmp(temp->info->attrName, sTemp->info->fileName) != 0)
                break;
            temp = temp->next;
        }
        if (!temp)
            break;

        if ((blkCnt = SHT_SecondaryGetAllEntries(*sTemp->info, *temp->info, sec_key)) < 0)
            printf("Searching secondary key failed\n");
        else
            printf("Read %d SHT blocks\n", blkCnt);

        sTemp = sTemp->next;
    }

    //prints hash statistics for every HT file
    temp = ht_files->head;
    while (temp)
    {
        if (HashStatistics(temp->info->attrName) < 0)
            printf("Error in calculating HT statistics\n");
        temp = temp->next;
    }

    //prints hash statistics for every SHT file
    sTemp = sht_files->head;
    while (sTemp)
    {
        if (HashStatistics(sTemp->info->attrName) < 0)
            printf("Error in calculating SHT statistics\n");
        sTemp = sTemp->next;
    }

    //closes and deletes all opened files
    printf("\nClosing HT files\n");
    temp = ht_files->head;
    while (temp)
    {
        if (HT_CloseIndex(temp->info) < 0)
            exit(EXIT_FAILURE);

        temp = temp->next;
    }

    printf("\nClosing SHT files\n");
    sTemp = sht_files->head;
    while (sTemp)
    {
        if (!sTemp->info->fileDesc)
            break;

        if (SHT_CloseSecondaryIndex(sTemp->info) < 0)
            exit(EXIT_FAILURE);

        sTemp = sTemp->next;
    }

    //frees all memory
    fclose(fp);
    ListDestroy(ht_files);
    SListDestroy(sht_files);
    free(line);
    free(record);
    free(secondaryRecord);
    free(ht_files);
    free(key);
    free(sec_key);
    free(sht_files);

    printf("Exiting\n");
    return 0;
}

