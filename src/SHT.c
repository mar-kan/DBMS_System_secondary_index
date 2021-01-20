#include "../include/SHT.h"


int SHT_CreateSecondaryIndex(char *sfileName, /* όνομααρχείου */char* attrName, /* όνομαπεδίου-κλειδιού */int attrLength, /* μήκοςπεδίου-κλειδιού */int buckets, /* αριθμόςκάδωνκατακερματισμού*/char* fileName /* όνομααρχείουπρωτεύοντοςευρετηρίου*/)
{
    int fd;
    void* block = NULL;
    int blkCnt = 0;
    char shash_id = 'S';

    //creates and opens new block file
    if (BF_CreateFile(sfileName) < 0)
    {
        BF_PrintError("Error creating BF file");
        return -1;
    }
    if ((fd = BF_OpenFile(sfileName)) < 0)
    {
        BF_PrintError("Error opening BF file");
        return -1;
    }

    //allocates its 1st block
    if (BF_AllocateBlock(fd) < 0)
    {
        BF_PrintError("Error allocating BF file");
        return -1;
    }

    if (BF_ReadBlock(fd, blkCnt, &block) < 0)
    {
        BF_PrintError("Error reading BF file");
        return -1;
    }

    int filename_size = sizeof(fileName);
    //writes secondary hash id to block, number of max buckets of file, sizeof(filename), filename
    memcpy(block,&shash_id,sizeof(char));
    memcpy(block+sizeof(char), &buckets, sizeof(int));
    memcpy(block+sizeof(char)+sizeof(int), &filename_size, sizeof(int));
    memcpy(block+sizeof(char)+sizeof(int)*2, fileName, filename_size);

    if (BF_WriteBlock(fd, blkCnt) < 0)
    {
        BF_PrintError("Error writing BF file");
        return -1;
    }

    //closes file
    if (BF_CloseFile(fd) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }
    return 0;
}


SHT_info * SHT_OpenSecondaryIndex(char *sfileName /* όνομα αρχείου */ )
{
    int fd;
    void * block;
    if ((fd = BF_OpenFile(sfileName)) < 0)
    {
        BF_PrintError("Error opening block file");
        return NULL;
    }

    //reads data
    if (BF_ReadBlock(fd, 0, &block) < 0) //reads file's block 0 and stores its data to block
    {
        BF_PrintError("Error reading block file");
        return NULL;
    }

    //gets id of file
    char file_id;
    memcpy (&file_id, block, sizeof(char));

    //checks if opened file is SHT file
    if (file_id != 'S')
    {
        printf("File is not an SHT file\n");
        return NULL;
    }

    //gets num of buckets of file
    int buckets;
    memcpy (&buckets, block+sizeof(char), sizeof(int));

    //gets ht filename
    int filename_size;
    memcpy(&filename_size, block+sizeof(char)+sizeof(int), sizeof(int));
    char * filename = (char*)malloc(filename_size);
    memcpy(filename, block+sizeof(char)+sizeof(int)*2, filename_size);

    //closes file
    if (BF_CloseFile(fd) < 0)
    {
        BF_PrintError("Error closing block file");
        return NULL;
    }

    //creates file's HT_info
    SHT_info * info = malloc(sizeof(SHT_info));

    info->fileDesc = fd;
    info->attrName = (char*)malloc(sizeof(sfileName));
    strcpy(info->attrName, sfileName);
    info->attrLength = sizeof (sfileName);
    info->numBuckets = buckets;   //keeps max num of buckets of file
    info->fileName = (char*)malloc(sizeof(filename));
    strcpy(info->fileName, filename);

    free(filename);
    return info;
}


int SHT_CloseSecondaryIndex(SHT_info* header_info)
{
    if ((BF_OpenFile(header_info->attrName)) < 0)
    {
        BF_PrintError("Error opening block file");
        return -1;
    }

    if (BF_CloseFile(header_info->fileDesc) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }

    //frees SHT_info memory
    free(header_info->fileName);
    free(header_info->attrName);
    free(header_info);
    return 0;
}


int SHT_SecondaryInsertEntry(SHT_info header_info, /* επικεφαλίδα του αρχείου*/ SecondaryRecord record /* δομή πουπροσδιορίζει την εγγραφή */)
{
    void* block = NULL;
    int blkCnt;
    int recordNum = 0;
    int block_hash_num = 0;
    int new_block = 0;
    int max_buckets;

    if ((BF_OpenFile(header_info.attrName)) < 0)
    {
        BF_PrintError("Error opening block file");
        return -1;
    }

    //reads 1st block
    if (BF_ReadBlock(header_info.fileDesc, 0, &block) < 0) //reads file's block 0 and stores its data to block
    {
        BF_PrintError("Error reading block file");
        return -1;
    }

    //gets max num of buckets for file
    memcpy(&max_buckets, block+sizeof(char), sizeof(int));

    //calculates hash value of new entry's key
    long hash_value = SHT_HashFunction(record.record.surname, max_buckets);

    //gets last block
    if((blkCnt = BF_GetBlockCounter(header_info.fileDesc)) < 0)
    {
        BF_PrintError("Error getting block counter");
        return -1;
    }
    blkCnt--;

    if(!blkCnt)   //only 1st info block
    {
        //allocates a new block
        if (BF_AllocateBlock(header_info.fileDesc) < 0)
        {
            BF_PrintError("Error allocating block");
            return -1;
        }
        blkCnt++;
        new_block = 1;
    }

    //reads last block and gets its hash value and number of records
    if (BF_ReadBlock(header_info.fileDesc, blkCnt, &block) < 0)
    {
        BF_PrintError("Error in reading block");
        return -1;
    }

    int i = blkCnt-1;
    if (!new_block)
    {
        //checks all blocks for a hash value that equals the new entry's one
        for (i=1; i<blkCnt; i++)        //skips info block 0
        {
            memcpy(&block_hash_num, block, sizeof(int));
            memcpy (&recordNum, block + sizeof(int), sizeof(int));

            if (block_hash_num == hash_value) //a block with this hash value already exists
                break;
        }

        if (i == blkCnt)    //there are no files with this hash value
        {
            //allocates a new block
            if (BF_AllocateBlock(header_info.fileDesc) < 0)
            {
                BF_PrintError("Error allocating block");
                return -1;
            }
            blkCnt++;
            new_block = 1;

            //sets values for new block
            recordNum = 0;
            block_hash_num = (int)hash_value;

            //reads newly allocated block
            if (BF_ReadBlock(header_info.fileDesc, blkCnt, &block) < 0)
            {
                BF_PrintError("Error in reading block");
                return -1;
            }
        }

        //checks if there is enough space for the new record
        if (sizeof(int)*2 + recordNum*sizeof(SecondaryRecord) + sizeof(SecondaryRecord) > BLOCK_SIZE)   //no space
        {
            //allocates a new block
            if (BF_AllocateBlock(header_info.fileDesc) < 0)
            {
                BF_PrintError("Error allocating block");
                return -1;
            }
            blkCnt++;
            new_block = 1;

            //sets values for new block
            recordNum=0;
            block_hash_num = (int)hash_value;

            //passes new block's number at the end of this block
            memcpy(block+sizeof(int)*2 + recordNum*sizeof(SecondaryRecord), &blkCnt, sizeof(int));
            if (BF_WriteBlock(header_info.fileDesc, i) < 0)
            {
                BF_PrintError("Error writing block");
                return -1;
            }

            //reads newly allocated block
            if (BF_ReadBlock(header_info.fileDesc, blkCnt, &block) < 0)
            {
                BF_PrintError("Error reading block");
                return -1;
            }
        }
    }

    //writes hash value if block is new
    if (new_block)
        memcpy(block, &hash_value, sizeof(int));

    //updates recordNum
    recordNum++;
    memcpy(block+sizeof(int), &recordNum, sizeof(int));

    //inserts record at the end of the block
    memcpy(block+sizeof(int)*2+ sizeof(void*)+recordNum*sizeof(SecondaryRecord), &record, sizeof(SecondaryRecord));

    //writes block to block file
    if (BF_WriteBlock(header_info.fileDesc, i) < 0)
    {
        BF_PrintError("Error writing block");
        return -1;
    }

    //closes file
    if (BF_CloseFile(header_info.fileDesc) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }
    return i;   //returns block where record was inserted
}


int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, /* επικεφαλίδα του αρχείου δευτερεύοντος ευρετηρίου*/ HT_info header_info_ht,/*επικεφαλίδατουαρχείουπρωτεύοντοςευρετηρίου*/ void *value /* τιμή τουπεδίου-κλειδιού προς αναζήτηση */)         //searches for a record with id = key. if found prints it
{
    void* block = NULL;
    int blkCnt;
    int recordNum = 0;
    int hash_value;
    int i, j;
    int flag = 0;

    //opens requested file
    if ((BF_OpenFile(header_info_sht.attrName)) < 0)
    {
        BF_PrintError("Error opening block file");
        return -1;
    }

    //calculates value's hash value
    long hash_num = SHT_HashFunction(value, header_info_sht.numBuckets);

    //gets block count
    if((blkCnt = BF_GetBlockCounter(header_info_sht.fileDesc)) < 0)
    {
        BF_PrintError("Error getting block counter");
        return -1;
    }

    //searches each block of the file for hash value = value's hash_num
    for (i=0; i<blkCnt; i++)
    {
        //reads each block
        if (BF_ReadBlock(header_info_sht.fileDesc, i, &block) < 0)
        {
            BF_PrintError("Error in reading block");
            return -1;
        }

        //checks file id and skips 1st id block
        char file_id;
        if (i==0)
        {
            memcpy (&file_id, block, sizeof(char));

            //checks if opened file is hash file
            if (file_id != 'S')
            {
                printf("File is not a hash file\n");
                return -1;
            }
            continue;
        }

        //gets block's hash value
        memcpy (&hash_value, block, sizeof(int));

        if (hash_value != hash_num)
            continue;   //skip

        //gets block's record num
        memcpy (&recordNum, block, sizeof(int));

        //checks each record
        for (j=0; j<=recordNum; j++)
        {
            //gets current secondary record and compares its key to value
            SecondaryRecord * rec = malloc(sizeof(SecondaryRecord));
            memcpy(rec, block+sizeof(int)*2 + j*sizeof(SecondaryRecord), sizeof(SecondaryRecord));

            //converts record id to string
            char * id = (char*)malloc(sizeof(int));

            if (strcmp(rec->record.surname, (char*)value) == 0)  //secondary record found
            {
                //prints record
                printf("Found Record with secondary key: %s\n"
                       "ID: %d\n"
                       "Full name: %s %s\n"
                       "Address: %s\n", (char*)value, rec->record.id, rec->record.name, rec->record.surname, rec->record.address);

                flag = 1;
            }
            free(id);
            free(rec);
        }
    }

    if (!flag)
        printf("Secondary record key %s not found\n", (char*)value);

    //closes file
    if (BF_CloseFile(header_info_sht.fileDesc) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }

    return blkCnt;  //returns num of blocks read
}


long SHT_HashFunction(char * key, long buckets)  //hash function
{
    size_t hash = 0;
    while (*key)
        hash = (hash * 10) + *key++ - '0';

    return (long)hash % buckets;
}


int HashStatistics(char * filename)         //prints hash file's statistics
{
    int fd;
    int blkCnt;
    int recordNum;
    void * block;
    int min=5000, max=0, sum=0, overflow_sum=0;

    //opens requested file
    if ((fd = BF_OpenFile(filename)) < 0)
    {
        BF_PrintError("Error opening block file");
        return -1;
    }
    //gets block count
    if((blkCnt = BF_GetBlockCounter(fd)) < 0)
    {
        BF_PrintError("Error getting block counter");
        return -1;
    }

    //traverses all blocks of the file and calculates statistics
    for (int i=0; i<blkCnt; i++)
    {
        //reads each block
        if (BF_ReadBlock(fd, i, &block) < 0)
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
            if (file_id != 'S')
            {
                printf("File is not a hash file\n");
                return -1;
            }
            continue;
        }

        //gets num of records
        memcpy (&recordNum, block, sizeof(int));

        //updates statistics if necessary
        if (recordNum < min)
            min = recordNum;
        if (recordNum > max)
            max = recordNum;
        sum++;
    }
    //closes file
    if (BF_CloseFile(fd) < 0)
    {
        BF_PrintError("Error closing block file");
        return -1;
    }

    printf("\nHash statistics for file %s:\n", filename);
    printf("File has %d blocks\n", blkCnt);
    printf("Records per bucket:\n Minimum: %d\n Maximum: %d\n Average: %d\n", min, max, sum /blkCnt);
    printf("Buckets with overflow: %d\n", overflow_sum);

    return 0;
}