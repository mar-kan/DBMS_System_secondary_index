#include <stdio.h>
#include "../include/utilities.h"


int createRecordFromLine(char * line, Record * record)
{
    int iteration=0;
    char * temp = (char*)malloc(sizeof(line));

    int i=1;                                    //ignores 1st char of line because its a bracket ({)
    while (line[i] != '}')  //last character
    {
        //stores each value from line in temp
        int j=0;
        while (line[i] != ',')
        {
            if (line[i] == '"') //ignores quotation marks
            {
                i++;
                continue;
            }
            if (line[i] == '}')
                break;

            temp[j++] = line[i++];
        }
        temp[j+1] = '\0';

        //initializes each variable of record
        switch (iteration)
        {
            case 0:
                record->id = atoi(temp);
                break;
            case 1:
                strcpy(record->name, temp);
                break;
            case 2:
                strcpy(record->surname, temp);
                break;
            case 3:
                strcpy(record->address, temp);
                break;
            default:
                break;
        }
        if (line[i] == '}')
            break;

        iteration++;
        i++;
    }

    free(temp);
    return 0;
}


int printHT(HT_info header_info)
{
    void* block = NULL;
    int blkCnt;
    int recordNum;

    //opens requested file
    if ((BF_OpenFile(header_info.attrName)) < 0)
    {
        BF_PrintError("Error opening block file");
        return -1;
    }
    //gets block count
    if((blkCnt = BF_GetBlockCounter(header_info.fileDesc)) < 0)
    {
        BF_PrintError("Error getting block counter");
        return -1;
    }

    //searches each block of the file for record with id = value
    for (int i=0; i<blkCnt; i++)
    {
        //reads each block
        if (BF_ReadBlock(header_info.fileDesc, i, &block) < 0)
        {
            BF_PrintError("Error in reading block");
            return -1;
        }

        //checks file id and skips 1st id block
        char file_id;
        if (i==0)
        {
            memcpy (&file_id, block, sizeof(char));

            //checks if opened file is heap file
            if (file_id != 'T')
            {
                printf("File is not a ash file\n");
                return -1;
            }
            continue;
        }

        //gets num of records
        memcpy (&recordNum, block, sizeof(int));

        //checks each record
        for (int j=0; j<recordNum; j++)
        {
            //gets current record and compares its key to value
            Record * rec = malloc(sizeof(Record));
            memcpy(rec, block+sizeof(int)*2 + j*sizeof(Record), sizeof(Record));

            printf("%d , %s , %s , %s \n", rec->id, rec->name, rec->surname, rec->address);
            free(rec);
        }
    }
    //closes file
    if (BF_CloseFile(header_info.fileDesc) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }
}