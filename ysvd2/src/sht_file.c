#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/bf.h"
#include <unistd.h>
#include "../include/sht_file.h"
#define MAX_OPEN_FILES 20

#define CALL_BF(call)             \
    {                             \
        BF_ErrorCode code = call; \
        if (code != BF_OK)        \
        {                         \
            BF_PrintError(code);  \
            return HT_ERROR;      \
        }                         \
    }

#ifndef HASH_FILE_H
#define HASH_FILE_H

typedef enum HT_ErrorCode
{
    HT_OK,
    HT_ERROR
} HT_ErrorCode; 

typedef struct Record
{
    int id;
    char name[15];
    char surname[20];
    char city[20];
} Record;

typedef struct
{
    char index - key[20];
    int tupleId; /*Ακέραιος που προσδιορίζει το block και τη θέση μέσα στο block στην οποία έγινε η εισαγωγή της εγγραφής στο πρωτεύον ευρετήριο.*/
} SecondaryRecord;
#endif                // HASH_FILE_H
extern SHT_Index *sh; // Declare Global Files Array
extern HT_Index *h;
extern Buckets_Ind *b; // Declare Create Requests List
FILE *fptr1;
extern const char *names;
extern const char *surnames;
extern const char *cities;


HT_ErrorCode SHT_Init()
{
    sh = malloc(MAX_OPEN_FILES * sizeof(SHT_Index)); // Initiliaze Global Files Array
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        (sh)[i].free = 1;
        (sh)[i].globaldepth = -1;
    }
    return HT_OK;
}

HT_ErrorCode SHT_CreateSecondaryIndex(const char *sfileName, char *attrName, int attrLength, int depth, char *fileName)
{

    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        fclose(fptr1);
        return HT_ERROR;
    }
    fprintf(fptr1, "-- %s --\n", sfileName);

    if (depth > DEFTEREVON_EVR_MAX_DEPTH)
    {
        fprintf(fptr1, "Creation request with more than allowed max depth, create failed\n");
        fclose(fptr1);
        return HT_ERROR;
    }
    Buckets_Ind *t; //Add Create File Request to end of List
    int flag = 1;
    t = b->next;
    Buckets_Ind *p;
    p = b;
    while (t != NULL)
    {
        p = t;
        if (strcmp(t->filename, fileName) == 0 && t->protevon == 1)
        {
            fprintf(fptr1, "Found relevant protevon\n");
            flag = 0;
        }
        t = t->next;
    }

    if (flag)
    {
        printf("OK2\n");

        fprintf(fptr1, "Defterevon creation request without relevant protevon!\n");
        fclose(fptr1);
        return HT_ERROR;
    }
    CALL_BF(BF_CreateFile(sfileName));
    t = malloc(sizeof(Buckets_Ind));
    t->next = NULL;
    t->prev = p;
    p->next = t;
    strcpy(t->filename, sfileName);
    t->startingglobaldepth = depth;
    t->protevon = 0;
    t->attrLength = attrLength;
    strcpy(t->protevon_filename, fileName);
    strcpy(t->attrName, attrName);
    fprintf(fptr1, "Added Create Secondary File Request to List - FileName: %s, StartingGlobalDepth: %d, Protevon_Filename: %s\n", t->filename, depth, t->protevon_filename);
    fclose(fptr1);
    return HT_OK;
}

HT_ErrorCode SHT_OpenSecondaryIndex(const char *sfileName, int *indexDesc)
{

    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        return HT_ERROR;
    }
    Buckets_Ind *t;
    fprintf(fptr1, "-- %s --\n", sfileName);
    t = b->next;
    if (t == NULL)
    {
        fprintf(fptr1, "There hasn't been a Create File Request yet\n");
        fclose(fptr1);

        return HT_ERROR;
    }

    while (strcmp(t->filename, sfileName) != 0)
    {

        t = t->next;
        if (t == NULL)
        {
            fprintf(fptr1, "No Created Files match the Requested Filename\n");
            fclose(fptr1);
            return HT_ERROR;
        }
    }

    int depth;
    int attrLength;
    char attrName[10];
    char protevon1[20];
    if (strcmp(t->filename, sfileName) == 0 && t->protevon == 0)
    {
        depth = t->startingglobaldepth;
        attrLength = t->attrLength;
        strcpy(attrName, t->attrName);
        printf("Bad Bad Good\n");
    }
    else
    {
        printf("Good Good\n");
        return HT_ERROR;
    }

    int fd;
    *indexDesc = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) // Find if there are MAX_OPEN_FILES files opened, if not open this one
    {

        if (sh[i].free == 1)
        {
            *indexDesc = i;
            sh[i].free = 0;
            break;
        }
    }
    printf("Good Good?\n");

    if (*indexDesc == -1)
    {
        fprintf(fptr1, "Max number of allowed files has already been opened\n");
        fclose(fptr1);
        return HT_ERROR;
    }
    fprintf(fptr1,"Stin i-osti thesi anoixa =%d\n",*indexDesc);
    CALL_BF(BF_OpenFile(sfileName, &fd));
    sh[*indexDesc].anagn_arx = fd;
    strcpy(sh[*indexDesc].attrName, attrName);
    sh[*indexDesc].attrLength = attrLength;
    strcpy(sh[*indexDesc].onoma_protevontos_arxeiou,t->protevon_filename);
    int blocksAllocated;
    BF_GetBlockCounter(fd, &blocksAllocated);

    if (blocksAllocated != 0) // No need for Allocation of Block_Evretiorou if the file has already opened
    {
        fprintf(fptr1, "Opening a secondary file that has been opened in the past\n");

        BF_Block *block;
        BF_Block_Init(&block);

        char *data;
        CALL_BF(BF_GetBlock(sh[*indexDesc].anagn_arx, 0, block)); //// !!!!
        data = BF_Block_GetData(block);

        int i, pos = 0, c = 0;
        do
        {
            memcpy(&i, data + pos, sizeof(i));
            pos += 2 * sizeof(i);
            c++;

        } while (i != -1);
        if (i == -1)
        {
            c -= 1;
        }
        fprintf(fptr1, "There are already %d key-bucketID sets saved in Blocks Evretiriou, ", c);
        int j = 0;
        while (c > 1)
        {
            c = c >> 1;
            j++;
        }
        fprintf(fptr1, "therefore we can deduce that its global depth is %d\n\n", j);
        sh[*indexDesc].globaldepth = j; // Update relevant globaldepth in array of files
        BF_Block_Destroy(&block);
    }
    else
    {
        BF_Block *block;
        BF_Block_Init(&block);
        int j = 1;
        fprintf(fptr1, "Opening this secondary file for the first time\n");

        for (int i = 0; i < depth; i++)
        {
            j *= 2;
        }
        sh[*indexDesc].globaldepth = depth;
        char *data, *data2;
        const int zero = 0;
        int pointed_bucket_id[j / 2];

        CALL_BF(BF_AllocateBlock(fd, block)); // Allocate Block evretiriou
        data = BF_Block_GetData(block);

        for (int i = 0; i < j / 2; i++) // Allocate Bucket - BLocks and update relevant values
        {
            //fprintf(fptr1, "Allocating Starting Blocks/Buckets\n");
            CALL_BF(BF_AllocateBlock(fd, block));
            data2 = BF_Block_GetData(block);
            memcpy(data2, &zero, sizeof(zero)); //Initialize Block/Bucket with numRecords = 0
            int ldepth = depth - 1;
            memcpy(data2 + sizeof(int), &ldepth, sizeof(int)); //And Local Depth = Global Depth - 1
            BF_GetBlockCounter(fd, &blocksAllocated);
            pointed_bucket_id[i] = blocksAllocated - 1;
            BF_Block_SetDirty(block);
            BF_UnpinBlock(block);
        }

        int pos = 0;
        for (int i = 0; i < j; i++) // Update relevant values for each key entry in Blocks Evretiriou
        {
            if (i % 2 == 0)
            {
                memcpy(data + pos, &i, sizeof(i));
                pos += sizeof(i);
                memcpy(data + pos, &pointed_bucket_id[i / 2], sizeof(pointed_bucket_id[i / 2]));
                pos += sizeof(pointed_bucket_id[i / 2]);
            }
            else
            {
                memcpy(data + pos, &i, sizeof(i));
                pos += sizeof(i);
                memcpy(data + pos, &pointed_bucket_id[(i - 1) / 2], sizeof(pointed_bucket_id[(i - 1) / 2]));
                pos += sizeof(pointed_bucket_id[(i - 1) / 2]);
            }
        }

        int flag = -1;
        memcpy(data + pos, &flag, sizeof(flag));
        CALL_BF(BF_GetBlock(fd, 0, block));
        data = BF_Block_GetData(block);
        BF_Block_SetDirty(block);

        CALL_BF(BF_GetBlock(fd, 0, block));
        data = BF_Block_GetData(block);

        pos = 0;
        int keyVal = 0, bucketID = 0;
        for (int i = 0; i < j; i++) // Print Values Block Evretiriou
        {
            memcpy(&keyVal, data + pos, sizeof(keyVal));
            pos += sizeof(keyVal);
            memcpy(&bucketID, data + pos, sizeof(bucketID));
            pos += sizeof(bucketID);
            fprintf(fptr1, "KeyVal:%d => BucketID:%d\n", keyVal, bucketID);
            BF_Block_SetDirty(block);
            if (pos == BF_BLOCK_SIZE)
            {
                pos = 0;
            }
        }
        memcpy(&keyVal, data + pos, sizeof(keyVal));
        fprintf(fptr1, "Here should be flag => %d\n", keyVal);

        fprintf(fptr1, "\n");
        BF_UnpinBlock(block);
        BF_Block_Destroy(&block);
    }
    fclose(fptr1);
    return HT_OK;
}

HT_ErrorCode SHT_CloseSecondaryIndex(int indexDesc)
{
    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        fclose(fptr1);
        return HT_ERROR;
    }
    fprintf(fptr1, "-- %d --\n", indexDesc);

    if (sh[indexDesc].free == 0)
    {
        BF_CloseFile(sh[indexDesc].anagn_arx);
        sh[indexDesc].anagn_arx = -1;
        sh[indexDesc].free = 1;
        sh[indexDesc].globaldepth = -1;
        fprintf(fptr1, "Closed file that was occupying the %d position of the file array.\n", indexDesc + 1);
    }
    else
    {
        fclose(fptr1);
        return HT_ERROR;
    }

    fclose(fptr1);
    return HT_OK;
}

HT_ErrorCode SHT_SecondaryInsertEntry(int indexDesc, SecondaryRecord record)
{
    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        return HT_ERROR;
    }
    fprintf(fptr1, "-- %d --\n", indexDesc);

    if (sh[indexDesc].free == 1)
    {
        fclose(fptr1);
        return HT_ERROR;
    }
    else
    {
        char *data, *data2;
        int fd;
        
        fd = sh[indexDesc].anagn_arx;

        printf("  FD is %d\n", fd);
        BF_Block *block;
        BF_Block_Init(&block);

        CALL_BF(BF_GetBlock(fd, 0, block));
        data = BF_Block_GetData(block);

        int numofblocks;
        BF_GetBlockCounter(sh[indexDesc].anagn_arx, &numofblocks);
        int numKeys = 1;
        for (int i = 0; i < sh[indexDesc].globaldepth; i++)
        {
            numKeys *= 2;
        } //numKeys = 2^d where d = global depth
        ssize_t pos = 0;
        int num_Blocks_Evr = 0;
        fprintf(fptr1, "There have been %d keys, %d blocks allocated, which means %d bucket-blocks\n", numKeys, numofblocks, numofblocks - 1);
        int val;
        unsigned long int x;
        int hash_val;
        if (strcmp("city", sh[indexDesc].attrName) == 0)
        {
            val = 0;
            if (numofblocks == 17)
            {
                printf("City hash\n");
                hash_val = hash_funct1(record.index_key, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(record.index_key);
                hash_val = x % numKeys;
            }
        }
        else if (strcmp("surname", sh[indexDesc].attrName) == 0)
        {
            val = 1;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(record.index_key, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(record.index_key);
                hash_val = x % numKeys;
            }
        }
        else if ((strcmp("name", sh[indexDesc].attrName) == 0))
        {
            val = 2;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(record.index_key, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(record.index_key);
                hash_val = x % numKeys;
            }
        }
        else
        {
            fclose(fptr1);
            return HT_ERROR;
        }
        if (hash_val == -1)
        {
            printf("ERRORRRR (number of blocks = %d), %s!!!!!\n",numofblocks,record.index_key);
        }
        int hash_key_BE = 0;
        pos = -2 * sizeof(int);
        num_Blocks_Evr = 0;
        fprintf(fptr1, "Record.indexkey is %s, its tuple id = %d, hashed value is = %d", record.index_key, record.tupleId ,hash_val);
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE, data + pos, sizeof(int)); // Find id of bucket that my hash value points to.

        } while (hash_key_BE != hash_val);
        int BucketID; // BucketID contains id of bucket

        memcpy(&BucketID, data + pos + sizeof(int), sizeof(int));
        BF_Block *block2;
        BF_Block_Init(&block2);
        fprintf(fptr1, ", so it belongs to bucket- block with ID = %d, ", BucketID);
        CALL_BF(BF_GetBlock(fd, BucketID, block2));
        data2 = BF_Block_GetData(block2);
        int numRecords;

        memcpy(&numRecords, data2, sizeof(int)); // Extract data about bucket - block
        int Local_Depth;
        memcpy(&Local_Depth, data2 + sizeof(int), sizeof(int));
        fprintf(fptr1, "which already includes %d records and has local depth of = %d\n", numRecords, Local_Depth);
        if (numofblocks < 17)
        {
            fprintf(fptr1, "%d blocks\n", numofblocks);
            int MAX_BLOCK_ENTRIES_SECONDARY = (BF_BLOCK_SIZE - 2 * sizeof(int)) / (sizeof(int) + sh[indexDesc].attrLength * sizeof(char));
            if (numRecords < floor(MAX_BLOCK_ENTRIES_SECONDARY))
            {
                pos = 2 * sizeof(int) + numRecords * (sizeof(int) + sh[indexDesc].attrLength * sizeof(char));
                memcpy(data2 + pos, &record.index_key, sh[indexDesc].attrLength * sizeof(char));
                memcpy(data2 + pos + sh[indexDesc].attrLength * sizeof(char), &record.tupleId, sizeof(int));
                numRecords += 1;
                memcpy(data2, &numRecords, sizeof(int)); // Update number of records in bucket - block
                char t[25];
                int hi;
                memcpy(t,data2 + pos, sh[indexDesc].attrLength * sizeof(char));
                memcpy(&hi,data2 + pos + sh[indexDesc].attrLength * sizeof(char),  sizeof(int));

                BF_Block_SetDirty(block2);
                BF_UnpinBlock(block2);
                fprintf(fptr1, "Successful eggrafi, %s %d\n",t,hi);
            }
            else
            {
                fprintf(fptr1, "\nMax block entries detected, checking for buddy\n");
                int Global_Depth = sh[indexDesc].globaldepth;
                if (Local_Depth < Global_Depth) // CASE 1: LOCAL DEPTH < GLOBAL DEPTH
                {
                    fprintf(fptr1, "Buddy found!");
                    if(Global_Depth - Local_Depth == 1){
                        int buddy_ID;
                        pos = -2 * sizeof(int);
                        num_Blocks_Evr = 0;
                        if (hash_val % 2 == 0)
                        {

                            do
                            {
                                pos += 2 * sizeof(int);
                                memcpy(&hash_key_BE, data + pos, sizeof(int)); // Locate buddy if hash_key is even

                            } while (hash_key_BE != hash_val + 1);
                        }
                        else
                        {

                            do
                            {
                                pos += 2 * sizeof(int);
                                memcpy(&hash_key_BE, data + pos, sizeof(int)); // Locate buddy if hash_key is odd
                            } while (hash_key_BE != hash_val - 1);
                        }
                        fprintf(fptr1, "=> Hash buddy of %d is %d\n", hash_val, hash_key_BE);
                        memcpy(&buddy_ID, data + pos + sizeof(int), sizeof(int));
                        fprintf(fptr1, "\n(Before Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID =  %d\n", buddy_ID, BucketID);
                        if (buddy_ID != BucketID)
                        {
                            fprintf(fptr1, "Logic Error!\n"); // Should never happen
                            fclose(fptr1);
                            return HT_ERROR;
                        }
                        CALL_BF(BF_AllocateBlock(fd, block2));
                        char *data3;
                        data3 = BF_Block_GetData(block2);
                        int zero = 0;
                        memcpy(data3, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in new Bucket - block
                        Local_Depth += 1;
                        memcpy(data3 + sizeof(int), &Local_Depth, sizeof(int));
                        BF_Block_SetDirty(block2);
                        BF_UnpinBlock(block2);
                        CALL_BF(BF_GetBlock(fd, BucketID, block2));
                        data2 = BF_Block_GetData(block2);
                        memcpy(data2, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in old Bucket - block
                        memcpy(data2 + sizeof(int), &Local_Depth, sizeof(int));
                        SecondaryRecord r[MAX_BLOCK_ENTRIES_SECONDARY];
                        for (int i = 0; i < MAX_BLOCK_ENTRIES_SECONDARY; i++)
                        {
                            memcpy(&r[i], data2 + 2 * sizeof(int) + i * sizeof(SecondaryRecord), sizeof(SecondaryRecord));
                        }
                        BF_Block_SetDirty(block2);
                        BF_UnpinBlock(block2);

                        CALL_BF(BF_GetBlockCounter(sh[indexDesc].anagn_arx, &buddy_ID));
                        buddy_ID -= 1;
                        memcpy(data + pos + sizeof(int), &buddy_ID, sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)
                        fprintf(fptr1, "(After Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID = %d\n", buddy_ID, BucketID);
                        if (buddy_ID == BucketID)
                        {
                            fprintf(fptr1, "Logic Error!\n"); // Should never happen
                            fclose(fptr1);
                            return HT_ERROR;
                        }

                        CALL_BF(BF_GetBlock(fd, 0, block2));
                        BF_Block_SetDirty(block2);

                        fprintf(fptr1, "\nBeginning rehashing\n");
                        fclose(fptr1);
                        for (int i = 0; i < MAX_BLOCK_ENTRIES_SECONDARY; i++)
                        {

                            CALL_BF(SHT_SecondaryInsertEntry(indexDesc, r[i]));
                        }

                        CALL_BF(SHT_SecondaryInsertEntry(indexDesc, record));
                        fptr1 = fopen("Output1.txt", "a");
                        if (fptr1 == NULL)
                        {
                            return HT_ERROR;
                        }
                        fprintf(fptr1, "\nFinished rehashing\n");
                    }
                    else{
                        fprintf(fptr1,"We in this (searching for %d)\n",BucketID);; 
                        int cc = 0, l = 0;
                        int competitors[numKeys];
                        int hash_key_BED;
                        int bucketsIDS[numKeys];
                        pos = -2 * sizeof(int);
                        do{
                            pos += 2 * sizeof(int);
                            memcpy(&bucketsIDS[cc], data + pos + sizeof(int), sizeof(int)); // Locate buddy if hash_key is even
                            memcpy(&competitors[cc], data + pos, sizeof(int));              // Locate buddy if hash_key is even
                            if (bucketsIDS[cc] == BucketID)
                            {
                                cc++;
                            }
                            l++;
                        }while (l < numKeys);
                            printf("We in thissss\n");
                        ;
                        fprintf(fptr1, "%d guys for the same girl\n", cc);

                        int buddy_ID;
                        num_Blocks_Evr = 0;

                        for (int i = 0; i < cc; i++)
                        {
                            fprintf(fptr1, "\n(Before Splitting) Buddy no %d points to bucketID = %d\n", i, bucketsIDS[i]);
                            if (i != cc - 1)
                            {
                                if (bucketsIDS[i] != bucketsIDS[i + 1])
                                {
                                    fprintf(fptr1, "Oh god why!\n");
                                    fclose(fptr1);
                                    return HT_ERROR;
                                }
                            }
                        }
                        printf("We gucci\n");
                        CALL_BF(BF_AllocateBlock(fd, block2));
                        char *data3;
                        data3 = BF_Block_GetData(block2);
                        int zero = 0;
                        memcpy(data3, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in new Bucket - block
                        Local_Depth += 1;
                        memcpy(data3 + sizeof(int), &Local_Depth, sizeof(int));
                        BF_Block_SetDirty(block2);
                        BF_UnpinBlock(block2);
                        CALL_BF(BF_GetBlock(fd, bucketsIDS[0], block2));
                        data2 = BF_Block_GetData(block2);
                        memcpy(data2, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in old Bucket - block
                        memcpy(data2 + sizeof(int), &Local_Depth, sizeof(int));
                        SecondaryRecord r[MAX_BLOCK_ENTRIES_SECONDARY];
                        for (int i = 0; i < MAX_BLOCK_ENTRIES_SECONDARY; i++)
                        {
                            memcpy(&r[i], data2 + 2 * sizeof(int) + i * sizeof(SecondaryRecord), sizeof(SecondaryRecord));
                        }
                        BF_Block_SetDirty(block2);
                        BF_UnpinBlock(block2);
                        CALL_BF(BF_GetBlockCounter(sh[indexDesc].anagn_arx, &buddy_ID));
                        buddy_ID -= 1;
                        for (int i = 0; i < cc; i++)
                        {

                            // Last i elements are already in place
                            for (int j = 0; j < cc - i; j++)
                            {
                                if (competitors[j] > competitors[j + 1])
                                {
                                    int temp = competitors[j];
                                    competitors[j] = competitors[j + 1];
                                    competitors[j + 1] = temp;
                                }
                            }
                        }
                        for (int i = 0; i < cc; i++)
                        {
                            printf("%d. %d\n", i + 1, competitors[i]);
                        }
                        for (int i = 0; i < cc; i++)
                        {
                            if (i >= cc / 2)
                            {
                                pos = 0;
                                for (int j = 0; j < numKeys; j++)
                                {
                                    memcpy(&val, data + pos, sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)
                                    if (val == competitors[i])
                                    {
                                        memcpy(data + pos +sizeof(int), &buddy_ID, sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)
                                    }
                                    pos += 2 * sizeof(int);
                                }
                            }
                        }
                        int val2;
                        pos = 0;
                        for (int j = 0; j < numKeys; j++)
                        {
                            memcpy(&val, data + pos, sizeof(int));                // Update where hash_key of buddy points to (newest bucket- block)
                            memcpy(&val2, data + pos + sizeof(int), sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)

                            fprintf(fptr1, "(After Splitting) Hash_key = %d => bucket ID = %d\n", val, val2);

                            pos += 2 * sizeof(int);
                        }

                        CALL_BF(BF_GetBlock(fd, 0, block2));

                        fprintf(fptr1, "\nBeginning rehashing\n");
                        fclose(fptr1);
                        for (int i = 0; i < MAX_BLOCK_ENTRIES_SECONDARY; i++)
                        {

                            CALL_BF(SHT_SecondaryInsertEntry(indexDesc, r[i]));
                        }

                        CALL_BF(SHT_SecondaryInsertEntry(indexDesc, record));
                        fptr1 = fopen("Output1.txt", "a");
                        if (fptr1 == NULL)
                        {
                            return HT_ERROR;
                        }
                        fprintf(fptr1, "\nFinished rehashing\n");
                        ;
                    }
                }
                else // CASE 2: LOCAL DEPTH = GLOBAL DEPTH
                {

                    fprintf(fptr1, "No buddy, trying array expansion:");
                    if (sh[indexDesc].globaldepth == DEFTEREVON_EVR_MAX_DEPTH)
                    {
                        fprintf(fptr1, " Cannot expand anymore, sorry!\n"); // Record is not saved, but program does not finish because that doesnt mean all Bucket - Blocks are full!
                        return HT_ERROR;
                    }
                    else
                    {
                        num_Blocks_Evr = 0;
                        sh[indexDesc].globaldepth += 1; // Increase global depth
                        pos = 0;
                        int HK_ev1, HK_ev2[numKeys], bucketID[numKeys]; //in 3 old block id
                        fprintf(fptr1, "\nOld global depth = %d => no. of existing hash keys = 2^d = %d\n", sh[indexDesc].globaldepth - 1, numKeys);
                        if (sh[indexDesc].globaldepth == DEFTEREVON_EVR_MAX_DEPTH)
                        {
                            fprintf(fptr1, "Danger zone!\n");
                            for (int i = 0; i < numKeys; i++)
                            {
                                memcpy(&HK_ev1, data + pos, sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                HK_ev1 = HK_ev1 * 2;
                                memcpy(data + pos, &HK_ev1, sizeof(int));
                                HK_ev2[i] = HK_ev1 + 1;
                                fprintf(fptr1, "Hash key evolution: from %d -> %d & %d\n", HK_ev1 / 2, HK_ev1, HK_ev1 + 1);
                                memcpy(&bucketID[i], data + pos + sizeof(int), sizeof(int));
                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }
                            for (int i = 0; i < numKeys; i++) // All new Hash_key - Bucket ID pairs are added in the blocks_evretiriou
                            {
                                memcpy(data + pos, &HK_ev2[i], sizeof(int));
                                memcpy(data + pos + sizeof(int), &bucketID[i], sizeof(int));
                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }

                            if (num_Blocks_Evr != 1)
                            {
                                fprintf(fptr1, "Updating new endpoint in block evr %d, pos %ld\n", num_Blocks_Evr, pos); // Update new block-evretiriou endpoint
                                int flag = -1;
                                memcpy(data + pos, &flag, sizeof(flag));
                                CALL_BF(BF_GetBlock(fd, num_Blocks_Evr, block));
                                data = BF_Block_GetData(block);
                                BF_Block_SetDirty(block);
                            }
                            BF_Block_SetDirty(block2);
                            BF_UnpinBlock(block2);
                            BF_GetBlock(sh[indexDesc].anagn_arx, 0, block);
                            BF_Block_SetDirty(block);
                            pos = 0;
                            for (int i = 0; i < 2 * numKeys; i++)
                            {
                                int f, f2;
                                memcpy(&f, data + pos, sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                memcpy(&f2, data + pos + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                fprintf(fptr1, "Hash_val = %d, points to bucket with ID = %d\n", f, f2);

                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }

                            SecondaryRecord SRS[336];
                            int j;
                            int k, srecordcounter = 0, buddy_ID;
                            do
                            {
                                CALL_BF(BF_GetBlock(fd, 0, block));
                                data = BF_Block_GetData(block);

                                ssize_t pos1 = 0;
                                for (int i = 0; i < 2 * numKeys; i++)
                                {
                                    memcpy(&k, data + pos1 + sizeof(int), sizeof(int)); // got my bucket ID
                                    memcpy(&hash_val, data + pos1, sizeof(int));        // got my hash val
                                    BucketID = k;
                                    CALL_BF(BF_GetBlock(fd, BucketID, block2));
                                    data2 = BF_Block_GetData(block2);
                                    int LDL;
                                    memcpy(&LDL, data2 + sizeof(int), sizeof(int));
                                    fprintf(fptr1, "Examining bucket with ID = %d, hash_val = %d, Local Depth = %d\n", k, hash_val, LDL);

                                    if (LDL == sh[indexDesc].globaldepth - 1)
                                    {
                                        
                                        fprintf(fptr1, "RN LDL is %d,", LDL);

                                        int noofrec;
                                        memcpy(&noofrec, data2, sizeof(int));
                                        fprintf(fptr1, " also it has %d records", noofrec);

                                        for (int i = 0; i < noofrec; i++)
                                        {
                                            memcpy(&SRS[srecordcounter].index_key, data2 + 2 * sizeof(int) + i * (sizeof(int) + sizeof(char) * sh[indexDesc].attrLength), sizeof(char) * sh[indexDesc].attrLength);
                                            memcpy(&SRS[srecordcounter].tupleId, data2 + 2 * sizeof(int) + i * (sizeof(int) + sizeof(char) * sh[indexDesc].attrLength) + sizeof(char) * sh[indexDesc].attrLength, sizeof(int));
                                            srecordcounter++;
                                        }

                                        pos = -2 * sizeof(int);
                                        num_Blocks_Evr = 0;
                                        if (hash_val % 2 == 0)
                                        {

                                            do
                                            {
                                                pos += 2 * sizeof(int);
                                                memcpy(&hash_key_BE, data + pos, sizeof(int)); // Locate buddy if hash_key is even

                                            } while (hash_key_BE != hash_val + 1);
                                        }
                                        else
                                        {

                                            do
                                            {
                                                pos += 2 * sizeof(int);
                                                memcpy(&hash_key_BE, data + pos, sizeof(int)); // Locate buddy if hash_key is odd
                                            } while (hash_key_BE != hash_val - 1);
                                        }
                                        fprintf(fptr1, "=> Hash buddy of %d is %d\n", hash_val, hash_key_BE);
                                        memcpy(&buddy_ID, data + pos + sizeof(int), sizeof(int));
                                        fprintf(fptr1, "\n(Before Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID =  %d\n", buddy_ID, BucketID);
                                        if (buddy_ID != BucketID)
                                        {
                                            fprintf(fptr1, "Logic Error!\n"); // Should never happen
                                            fclose(fptr1);
                                            return HT_ERROR;
                                        }
                                        CALL_BF(BF_AllocateBlock(fd, block2));
                                        char *data3;
                                        data3 = BF_Block_GetData(block2);
                                        int zero = 0;
                                        memcpy(data3, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in new Bucket - block
                                        LDL += 1;
                                        memcpy(data3 + sizeof(int), &LDL, sizeof(int));
                                        BF_Block_SetDirty(block2);
                                        BF_UnpinBlock(block2);
                                        CALL_BF(BF_GetBlock(fd, BucketID, block2));
                                        data2 = BF_Block_GetData(block2);
                                        memcpy(data2, &zero, sizeof(int)); // Set NumOfRecords = 0, Local Depth+=1 in old Bucket - block
                                        memcpy(data2 + sizeof(int), &LDL, sizeof(int));
                                        BF_Block_SetDirty(block2);
                                        BF_UnpinBlock(block2);

                                        CALL_BF(BF_GetBlockCounter(sh[indexDesc].anagn_arx, &buddy_ID));
                                        fprintf(fptr1, "Now there are %d buckets\n", buddy_ID);
                                        buddy_ID -= 1;
                                        memcpy(data + pos + sizeof(int), &buddy_ID, sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)
                                        fprintf(fptr1, "(After Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID = %d, with Local Depth = %d\n", buddy_ID, BucketID, LDL);
                                        if (buddy_ID == BucketID)
                                        {
                                            fprintf(fptr1, "Logic Error!\n"); // Should never happen
                                            fclose(fptr1);
                                            return HT_ERROR;
                                        }
                                        pos = 0;
                                        for (int i = 0; i < 2 * numKeys; i++)
                                        {
                                            int f, f2;
                                            memcpy(&f, data + pos, sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                            memcpy(&f2, data + pos + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                            fprintf(fptr1, "Hash_val = %d, points to bucket with ID = %d\n", f, f2);

                                            pos += 2 * sizeof(int);
                                            if (pos == BF_BLOCK_SIZE)
                                            {
                                                pos = 0;
                                                num_Blocks_Evr++;
                                            }
                                        }
                                    }
                                    else if (LDL < sh[indexDesc].globaldepth - 1)
                                    {

                                        int noofboys = 1;
                                        int n = sh[indexDesc].globaldepth - LDL;
                                        while (n > 0)
                                        {
                                            noofboys *= 2;
                                            n--;
                                        }
                                        fprintf(fptr1, "%d IDs same bucket, bucket ID searched for = %d\n", noofboys, BucketID);

                                        int competitors[noofboys], v;
                                        ssize_t pos2 = 0, cc = 0;
                                        for (int i = 0; i < 2 * numKeys; i++)
                                        {
                                            memcpy(&competitors[cc], data + pos2, sizeof(int));
                                            memcpy(&v, data + pos2 + sizeof(int), sizeof(int));
                                            if (v == BucketID)
                                            {
                                                cc++;
                                            }
                                            pos2 += 2 * sizeof(int);
                                        }
                                        fprintf(fptr1, "Found %ld buckets competing for same ID\n", cc);

                                        CALL_BF(BF_AllocateBlock(fd, block2));
                                        char *data3;
                                        data3 = BF_Block_GetData(block2);
                                        LDL += 1;
                                        memcpy(data3 + sizeof(int), &LDL, sizeof(int));
                                        BF_Block_SetDirty(block2);
                                        BF_UnpinBlock(block2);
                                        CALL_BF(BF_GetBlock(fd, BucketID, block2));
                                        data2 = BF_Block_GetData(block2);
                                        memcpy(data2 + sizeof(int), &LDL, sizeof(int));
                                        BF_Block_SetDirty(block2);
                                        BF_UnpinBlock(block2);
                                        BF_GetBlockCounter(sh[indexDesc].anagn_arx, &numofblocks);
                                        numofblocks -= 1;
                                        //quickSort(competitors,0,cc);
                                        for (int i = 0; i < cc; i++)
                                        {

                                            // Last i elements are already in place
                                            for (int j = 0; j < cc - i; j++)
                                            {
                                                if (competitors[j] > competitors[j + 1])
                                                {
                                                    int temp = competitors[j];
                                                    competitors[j] = competitors[j + 1];
                                                    competitors[j + 1] = temp;
                                                }
                                            }
                                        }
                                        for (int i = 0; i < cc; i++)
                                        {
                                            printf("%d. %d\n", i + 1, competitors[i]);
                                        }
                                        for (int i = 0; i < noofboys; i++)
                                        {
                                            if (i >= noofboys / 2)
                                            {
                                                pos = 0;
                                                for (int j = 0; j < 2 * numKeys; j++)
                                                {
                                                    int f, f2;
                                                    memcpy(&f, data + pos, sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                                    memcpy(&f2, data + pos + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                                    if (f == competitors[i])
                                                    {
                                                        memcpy(data + pos + sizeof(int), &numofblocks, sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                                        BF_Block_SetDirty(block);
                                                        break;
                                                    }
                                                    pos += 2 * sizeof(int);
                                                    if (pos == BF_BLOCK_SIZE)
                                                    {
                                                        pos = 0;
                                                        num_Blocks_Evr++;
                                                    }
                                                }
                                            }
                                        }
                                        BF_Block_SetDirty(block);

                                        pos = 0;
                                        for (int i = 0; i < 2 * numKeys; i++)
                                        {
                                            int f, f2;
                                            memcpy(&f, data + pos, sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                            memcpy(&f2, data + pos + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                            fprintf(fptr1, "!Hash_val = %d, points to bucket with ID = %d\n", f, f2);

                                            pos += 2 * sizeof(int);
                                            if (pos == BF_BLOCK_SIZE)
                                            {
                                                pos = 0;
                                                num_Blocks_Evr++;
                                            }
                                        }
                                        BF_Block_SetDirty(block);
                                        break;
                                    }
                                    pos1 += 2 * sizeof(int);
                                }

                                BF_GetBlockCounter(sh[indexDesc].anagn_arx, &j);
                                pos = 0;
                                fprintf(fptr1, "%d blocks rn\n", j);

                            } while (j < 17);
                            fprintf(fptr1, "There are %d records to take care of.\n", srecordcounter);

                            fprintf(fptr1, "Finished the big split, time for big rehash\n");
                            pos = 0;
                            for (int i = 0; i < 2 * numKeys; i++)
                            {
                                int f, f2;
                                memcpy(&f, data + pos, sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                memcpy(&f2, data + pos + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                fprintf(fptr1, "!Hash_val = %d, points to bucket with ID = %d, ", f, f2);
                                CALL_BF(BF_GetBlock(fd, f2, block2));
                                data2 = BF_Block_GetData(block2);
                                int l, nr;
                                memcpy(&nr, data2 , sizeof(int));                // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                memcpy(&l, data2 + sizeof(int), sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well

                                fprintf(fptr1, "=> no. of Rec: %d, LD: %d\n ", nr, l);

                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }
                            for (int i = 0; i < srecordcounter; i++)
                            {
                                //fprintf ("Finished the big split, time for big rehash\n");
                                fprintf(fptr1,"%s, %d\n",SRS[i].index_key,SRS[i].tupleId);
                                fclose(fptr1);
                                CALL_BF(SHT_SecondaryInsertEntry(indexDesc, SRS[i])); // Insert entry
                                fptr1 = fopen("Output1.txt", "a");
                            }
                            
                            fprintf(fptr1, "Finished the big rehash\n");

                            fclose(fptr1);
                            CALL_BF(SHT_SecondaryInsertEntry(indexDesc, record)); // Insert entry
                            fptr1 = fopen("Output1.txt", "a");
                            if (fptr1 == NULL)
                            {
                                return HT_ERROR;
                            }
                        }
                        else
                        {
                            SecondaryRecord Recs [336];
                            int cc = 0;
                            pos = 0;
                            for (int i = 0; i < numKeys; i++)
                            {

                                memcpy(&HK_ev1, data + pos, sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                                HK_ev1 = HK_ev1 * 2;
                                memcpy(data + pos, &HK_ev1, sizeof(int));
                                HK_ev2[i] = HK_ev1 + 1;
                                fprintf(fptr1, "Hash key evolution: from %d -> %d & %d\n", HK_ev1 / 2, HK_ev1, HK_ev1 + 1);
                                memcpy(&bucketID[i], data + pos + sizeof(int), sizeof(int));
                                BF_Block *block3;
                                BF_Block_Init(&block3);
                                BF_GetBlock(fd,bucketID[i],block3);
                                data2 = BF_Block_GetData(block3);
                                int rec;
                                memcpy(&rec, data2, sizeof(int));
                                fprintf(fptr1, "After upd: HashK = %d => BucketID = %d\n", HK_ev1, bucketID[i]);

                                ssize_t pos2 = 2 * sizeof(int);
                                int k;
                                for (int j = 0; j < rec; j++)
                                {

                                    memcpy(&Recs[cc], data2 + pos2, sizeof(SecondaryRecord));
                                    cc++;
                                    pos2 += sizeof(SecondaryRecord);
                                }
                                fprintf(fptr1, "While examining Hash val %d => Bucket_ID %d : Read %d records for the big rehash, this moment %d records in total\n", HK_ev1, bucketID[i], rec, cc);
                                rec = 0;
                                memcpy(data2, &rec, sizeof(int));
                                memcpy(&rec, data2, sizeof(int));
                                fprintf(fptr1, "Now there are %d records here\n", rec);
                                BF_Block_SetDirty(block3);
                                BF_UnpinBlock(block3);
                                BF_Block_Destroy(&block3);
                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }
                            for (int i = 0; i < numKeys; i++) // All new Hash_key - Bucket ID pairs are added in the blocks_evretiriou
                            {
                                memcpy(data + pos, &HK_ev2[i], sizeof(int));
                                memcpy(data + pos + sizeof(int), &bucketID[i], sizeof(int));
                                fprintf(fptr1, "After upd: HashK = %d => BucketID = %d\n", HK_ev2[i], bucketID[i]);

                                pos += 2 * sizeof(int);
                                if (pos == BF_BLOCK_SIZE)
                                {
                                    pos = 0;
                                    num_Blocks_Evr++;
                                }
                            }

                            if (num_Blocks_Evr != 1)
                            {
                                fprintf(fptr1, "Updating new endpoint in block evr %d, pos %ld\n", num_Blocks_Evr, pos); // Update new block-evretiriou endpoint
                                int flag = -1;
                                memcpy(data + pos, &flag, sizeof(flag));
                                CALL_BF(BF_GetBlock(fd, num_Blocks_Evr, block));
                                data = BF_Block_GetData(block);
                                BF_Block_SetDirty(block);
                            }
                            BF_Block_SetDirty(block2);
                            BF_UnpinBlock(block2);
                            BF_GetBlock(sh[indexDesc].anagn_arx, 0, block);
                            BF_Block_SetDirty(block);

                            fclose(fptr1);
                            for(int i = 0; i< cc; i++) {
                                CALL_BF(SHT_SecondaryInsertEntry(indexDesc, Recs[i])); // Insert entry
                            }
                            CALL_BF(SHT_SecondaryInsertEntry(indexDesc, record)); // Insert entry
                            fptr1 = fopen("Output1.txt", "a");
                            if (fptr1 == NULL)
                            {
                                return HT_ERROR;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            fprintf(fptr1, "%d blockz\n", numofblocks);
            int MAX_BLOCK_ENTRIES_SECONDARY = (BF_BLOCK_SIZE - 2 * sizeof(int)) / (sizeof(int));
            if (numRecords < floor(MAX_BLOCK_ENTRIES_SECONDARY))
            {
                pos = 2 * sizeof(int) + numRecords * sizeof(int);
                memcpy(data2 + pos, &record.tupleId, sizeof(int));
                numRecords += 1;
                memcpy(data2, &numRecords, sizeof(int)); // Update number of records in bucket - block
                BF_Block_SetDirty(block2);
                BF_UnpinBlock(block2);
                fprintf(fptr1, "Successful eggrafi\n");
            }
            else
            {
                fprintf(fptr1, "Cannot enter secondary bucket\n");
                fclose(fptr1);
                return HT_ERROR;
            }
        }
        BF_Block_SetDirty(block);
        BF_Block_SetDirty(block2);
        BF_UnpinBlock(block);
        BF_UnpinBlock(block2);

        BF_Block_Destroy(&block2);
        BF_Block_Destroy(&block);
        fprintf(fptr1,"Thank you for having me!\n");
        fclose(fptr1);
        return HT_OK;
    }
}

HT_ErrorCode SHT_SecondaryUpdateEntry(int indexDesc, UpdateRecordArray *updateArray)
{
    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        printf("Cannot open outputty!\n");
        return HT_ERROR;
    }
    printf("!\n");

    fprintf(fptr1, "-- %d --\n", indexDesc);
    if (sh[indexDesc].free == 1)
    {
        printf("Wrong open outputty!\n");

        fclose(fptr1);
        return HT_ERROR;
    }
    else
    {
        fprintf(fptr1, "%s %s %s }} %d\n", updateArray->name, updateArray->surname, updateArray->city, updateArray->oldTupleId);

        fprintf(fptr1,":)\n");
        printf("Good open outputty!\n");

        char *data, *data2;
        int fd;
        fd = sh[indexDesc].anagn_arx;
        printf(" Updated ID = %d, FD is %d, LD = %d\n",indexDesc, fd,sh[indexDesc].globaldepth);
        BF_Block *block;
        BF_Block_Init(&block);

        CALL_BF(BF_GetBlock(fd, 0, block));
        data = BF_Block_GetData(block);

        int numofblocks;
        BF_GetBlockCounter(sh[indexDesc].anagn_arx, &numofblocks);
        int numKeys = 1;
        for (int i = 0; i < sh[indexDesc].globaldepth; i++)
        {
            numKeys *= 2;
        } //numKeys = 2^d where d = global depth
        ssize_t pos = 0;
        int num_Blocks_Evr = 0;
        fprintf(fptr1, "(SecondaryUpdateEntry) There have been %d keys , %d blocks allocated, which means %d bucket-blocks\n", numKeys, numofblocks, numofblocks - 1);
        printf("(SecondaryUpdateEntry) There have been %d keys , %d blocks allocated, which means %d bucket-blocks\n", numKeys, numofblocks, numofblocks - 1);

        printf("<.>\n");

        int val;
        unsigned long int x;
        int hash_val;

        if (strcmp("city", sh[indexDesc].attrName) == 0)
        {
            fprintf(fptr1, "Examining values: %s, %d => %d\n", updateArray->city, updateArray->oldTupleId, updateArray->newTupleId);

            val = 0;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(updateArray->city, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(updateArray->city);
                hash_val = x % numKeys;
            }
        }
        else if (strcmp("surname", sh[indexDesc].attrName) == 0)
        {
            fprintf(fptr1, "Examining values: %s %d ==> %d\n", updateArray->surname, updateArray->oldTupleId, updateArray->newTupleId);

            val = 1;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(updateArray->surname, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(updateArray->surname);
                hash_val = x % numKeys;
            }
        }
        else if ((strcmp("name", sh[indexDesc].attrName) == 0))
        {
            fprintf(fptr1, "Examining values: %s %d ==> %d\n", updateArray->name, updateArray->oldTupleId, updateArray->newTupleId);

            val = 2;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(updateArray->name, sh[indexDesc].attrName);
            }
            else
            {
                x = hash_funct2(updateArray->name);
                hash_val = x % numKeys;
            }
        }
        else
        {
            fclose(fptr1);
            printf("?");
            return HT_ERROR;
        }
        printf("<.>\n");

        if (hash_val == -1){
            printf("ERRORRRR!!!!!\n");
        }
        int hash_key_BE = 0;
        pos = -2 * sizeof(int);
        int BucketID; // BucketID contains id of bucket

        num_Blocks_Evr = 0;
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE, data + pos, sizeof(int)); // Find id of bucket that my hash value points to.
            memcpy(&BucketID, data + pos + sizeof(int), sizeof(int));

            fprintf(fptr1,"Hash value = %d, proposed hash val = %d, bid = %d\n",hash_val,hash_key_BE,BucketID);
            if(hash_val == hash_key_BE){
                memcpy(&hash_key_BE, data + pos + 2*sizeof(int), sizeof(int)); // Find id of bucket that my hash value points to.
                fprintf(fptr1,"Found: %d (if -1 we should be at the last one)\n ",hash_key_BE);
                break;
            }

        } while (hash_key_BE != hash_val);

        BF_Block *block2;
        BF_Block_Init(&block2);
        CALL_BF(BF_GetBlock(fd, BucketID, block2));
        data2 = BF_Block_GetData(block2);

        int numRecords;

        memcpy(&numRecords, data2, sizeof(int)); // Extract data about bucket - block
        int Local_Depth;
        memcpy(&Local_Depth, data2 + sizeof(int), sizeof(int));
        fprintf(fptr1, "which already includes %d records and has local depth of = %d, each ik %d  searching for %s chars\n", numRecords, Local_Depth,sh[indexDesc].attrLength,sh[indexDesc].attrName);
        if (numofblocks < 17)
        {
            pos = 2 * sizeof(int);
            for (int i = 0; i < numRecords; i++)
            {
                char test[25];
                int t;
                memcpy(test, data2 + pos, sh[indexDesc].attrLength * sizeof(char));
                memcpy(&t, data2 + pos + sh[indexDesc].attrLength * sizeof(char), sizeof(int));
                fprintf(fptr1, "%s , (%d) = (%d)?\n", test, t, updateArray->oldTupleId);
                fprintf(fptr1, "Difference is %d - %d = %d\n", updateArray->oldTupleId, t, updateArray->oldTupleId - t);
                int otd = updateArray->oldTupleId;

                if (updateArray->oldTupleId - t == 0)
                {
                    fprintf(fptr1, "yes we are friends %d %d\n", updateArray->oldTupleId, t);
                }
                else{
                    fprintf(fptr1, "no we arent friends %d %d\n",updateArray->oldTupleId, t);
                }

                if (strcmp("city", sh[indexDesc].attrName) == 0)
                {
                    fprintf(fptr1, "City?\n");

                    if (strcmp(test, updateArray->city) == 0 && ((updateArray->oldTupleId - t) == 0))
                    {
                        fprintf(fptr1, "Updating old value %d with new value of %d\n", t, updateArray->newTupleId);
                        memcpy(data2 + pos + sh[indexDesc].attrLength * sizeof(char), &updateArray->newTupleId, sizeof(int));

                        break;
                    }
                }
                else if (strcmp("surname", sh[indexDesc].attrName) == 0)
                {
                    fprintf(fptr1, "Surname?\n");

                    if (strcmp(test, updateArray->surname) == 0 && updateArray->oldTupleId == t)
                    {
                        
                        fprintf(fptr1, "Updating old value %d with new value of %d\n", t, updateArray->newTupleId);
                        memcpy(data2 + pos + sh[indexDesc].attrLength * sizeof(char), &updateArray->newTupleId, sizeof(int));

                        break;
                    }
                }
                else if ((strcmp("name", sh[indexDesc].attrName) == 0))
                {
                    fprintf(fptr1, "Name?\n");

                    if (strcmp(test, updateArray->name) == 0 && updateArray->oldTupleId == t)
                    {
                        fprintf(fptr1, "Updating old value %d with new value of %d\n", t, updateArray->newTupleId);
                        memcpy(data2 + pos + sh[indexDesc].attrLength * sizeof(char), &updateArray->newTupleId, sizeof(int));
                        break;
                    }
                }
                else{
                    fprintf(fptr1,"WHAT?\n");
                    return HT_ERROR;
                }
                pos += sizeof(int) + sh[indexDesc].attrLength * sizeof(char);
                if(i == numRecords - 1){
                    fprintf(fptr1,"Did not find what I was looking for!\n");
                    return HT_ERROR;
                }
            }
            BF_Block_SetDirty(block2);
            BF_UnpinBlock(block2);
            BF_Block_SetDirty(block);

            fprintf(fptr1, "Successful eggrafi\n");
            BF_UnpinBlock(block);
            BF_UnpinBlock(block2);

            BF_Block_Destroy(&block2);
            BF_Block_Destroy(&block);
        }
        else
        {
            pos = 2 * sizeof(int);
            int test;

            for (int i = 0; i < numRecords; i++)
            {
                memcpy(&test, data2 + pos, sizeof(int));
                if (test == updateArray->oldTupleId)
                {
                    fprintf(fptr1, "Updating old value %d with new value of %d\n", test, updateArray->newTupleId);
                    break;
                }
                pos += sizeof(int);
            }
            if (test != updateArray->oldTupleId)
            {
                fprintf(fptr1, "LOGIC ERROR!\n");
                fclose(fptr1);
                printf("stop this rn");
                return HT_ERROR;
            }
            memcpy(data2 + pos, &updateArray->newTupleId, sizeof(int));
            BF_Block_SetDirty(block2);
            BF_Block_SetDirty(block);

            BF_UnpinBlock(block2);
            fprintf(fptr1, "Successful eggrafi\n");
            BF_UnpinBlock(block);
            BF_UnpinBlock(block2);

            BF_Block_Destroy(&block2);
            BF_Block_Destroy(&block);
        }

        fprintf(fptr1, "Thank you for having me (u)!\n");
        fclose(fptr1);
        return HT_OK;
    }
}

HT_ErrorCode SHT_PrintAllEntries(int sindexDesc, char *index_key)
{
    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        return HT_ERROR;
    }
    fprintf(fptr1, "\n\n-- PRINT ALL ENTRIES BITCHES %d (for entry = %s) --\n\n", sindexDesc,index_key);;

    if (sh[sindexDesc].free == 1)
    {
        fclose(fptr1);
        return HT_ERROR;
    }
    else
    {
        char *data, *data2;
        int fd;
        fd = sh[sindexDesc].anagn_arx;
        BF_Block *block;
        BF_Block_Init(&block);

        CALL_BF(BF_GetBlock(fd, 0, block));
        data = BF_Block_GetData(block);

        int numofblocks;
        BF_GetBlockCounter(sh[sindexDesc].anagn_arx, &numofblocks);
        int numKeys = 1;
        for (int i = 0; i < sh[sindexDesc].globaldepth; i++)
        {
            numKeys *= 2;
        } //numKeys = 2^d where d = global depth
        ssize_t pos = 0;
        int num_Blocks_Evr = 0;
        fprintf(fptr1, "There have been %d keys, %d blocks allocated, which means %d bucket-blocks\n", numKeys, numofblocks, numofblocks - 1);
        int val;
        unsigned long int x;
        int hash_val = x % numKeys;
        if (strcmp("city", sh[sindexDesc].attrName) == 0)
        {
            val = 0;
            if (numofblocks == 17)
            {
                printf("City hash\n");
                hash_val = hash_funct1(index_key, sh[sindexDesc].attrName);
            }
            else
            {
                x = hash_funct2(index_key);
                hash_val = x % numKeys;
            }
        }
        else if (strcmp("surname", sh[sindexDesc].attrName) == 0)
        {
            val = 1;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(index_key, sh[sindexDesc].attrName);
            }
            else
            {
                x = hash_funct2(index_key);
                hash_val = x % numKeys;
            }
        }
        else if ((strcmp("name", sh[sindexDesc].attrName) == 0))
        {
            val = 2;
            if (numofblocks == 17)
            {
                hash_val = hash_funct1(index_key, sh[sindexDesc].attrName);
            }
            else
            {
                x = hash_funct2(index_key);
                hash_val = x % numKeys;
            }
        }
        else
        {
            fclose(fptr1);
            return HT_ERROR;
        }
        if (hash_val == -1)
        {
            printf("ERRORRRR!!!!!\n");
        }
        int hash_key_BE = 0;
        pos = -2 * sizeof(int);
        num_Blocks_Evr = 0;
        fprintf(fptr1, "Record.id is %s, hashed value is = %d", index_key, hash_val);
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE, data + pos, sizeof(int)); // Find id of bucket that my hash value points to.

        } while (hash_key_BE != hash_val);
        int BucketID; // BucketID contains id of bucket

        memcpy(&BucketID, data + pos + sizeof(int), sizeof(int));
        BF_Block *block2;
        BF_Block_Init(&block2);
        fprintf(fptr1, ", so it belongs to bucket- block with ID = %d, ", BucketID);
        CALL_BF(BF_GetBlock(fd, BucketID, block2));
        data2 = BF_Block_GetData(block2);
        int numRecords;

        memcpy(&numRecords, data2, sizeof(int)); // Extract data about bucket - block
        int Local_Depth;
        memcpy(&Local_Depth, data2 + sizeof(int), sizeof(int));
        fprintf(fptr1, "which already includes %d records and has local depth of = %d\n", numRecords, Local_Depth);
        if (numofblocks < 17)
        {
            fprintf(fptr1,"IN THIS\n");
            fprintf(fptr1, "%d blocks\n", numofblocks);

            int MAX_BLOCK_ENTRIES_SECONDARY = (BF_BLOCK_SIZE - 2 * sizeof(int)) / (sizeof(int) + sh[sindexDesc].attrLength * sizeof(char));
            char ik[20];
            int TID;
            for(int i=0; i<numRecords; i++){

                pos = 2 * sizeof(int) + i * (sizeof(int) + sh[sindexDesc].attrLength * sizeof(char));
                memcpy(&ik,data2 + pos, sh[sindexDesc].attrLength * sizeof(char));
                memcpy(&TID, data2 + pos + sh[sindexDesc].attrLength * sizeof(char),  sizeof(int));
                fprintf(fptr1,"Examining %s, %d:",ik,TID);
                if(strcmp(ik,index_key)== 0){
                    for(int j=0; j<MAX_OPEN_FILES; j++){
                        if(h[j].free == 0 && strcmp(h[j].onoma_arxeiou, sh[sindexDesc].onoma_protevontos_arxeiou) == 0){
                            int position = 0,blono;
                            for (int k = 0; k < floor(MAX_BLOCK_ENTRIES); k++){
                                if((TID - k)%8 == 0){
                                    position = k;
                                    blono = (TID-k)/8 - 1;
                                    break;
                                }
                            }
                            fprintf(fptr1, "Entry with index key = %s, tupleID = %d, which means block no. %d and pos %d:",ik,TID,blono,position+1);

                            BF_Block *block3;
                            char *data3;
                            BF_Block_Init(&block3);
                            CALL_BF(BF_GetBlock(h[j].anagn_arx, blono, block3));
                            data3 = BF_Block_GetData(block3);
                            Record r;
                            memcpy(&r,data3+2*sizeof(int)+position*sizeof(Record),sizeof(Record));
                            fprintf(fptr1, "ID: %d, Name: %s, Surname: %s, City: %s\n\n", r.id, r.name, r.surname, r.city);
                            BF_UnpinBlock(block3);

                            BF_Block_Destroy(&block3);
                        }
                    }
                }
                else{
                    fprintf(fptr1,"\n");
                }

            }            
        }
        else{
            fprintf(fptr1, "IN THIS\n");
            fprintf(fptr1, "%d blocks\n", numofblocks);
            int MAX_BLOCK_ENTRIES_SECONDARY = (BF_BLOCK_SIZE - 2 * sizeof(int)) / sizeof(int);

            int TID;
            for (int i = 0; i < numRecords; i++)
            {

                pos = 2 * sizeof(int) + i * (sizeof(int));
                memcpy(&TID, data2 + pos, sizeof(int));
                    for (int j = 0; j < MAX_OPEN_FILES; j++)
                    {
                        
                        if (h[j].free == 0 && strcmp(h[j].onoma_arxeiou, sh[sindexDesc].onoma_protevontos_arxeiou) == 0)
                        {
                            fprintf(fptr1, "Found relevant protevon open\n");

                            int position = 0, blono;
                            for (int k = 0; k < floor(MAX_BLOCK_ENTRIES); k++)
                            {
                                if ((TID - k) % 8 == 0)
                                {
                                    position = k;
                                    blono = (TID - k) / 8 - 1;
                                    break;
                                }
                            }
                            fprintf(fptr1, "Entry with index key = %s, tupleID = %d, which means block no. %d and pos %d:", index_key, TID, blono, position + 1);

                            BF_Block *block3;
                            char *data3;
                            BF_Block_Init(&block3);
                            CALL_BF(BF_GetBlock(h[j].anagn_arx, blono, block3));
                            data3 = BF_Block_GetData(block3);
                            Record r;
                            memcpy(&r, data3 + 2 * sizeof(int) + position * sizeof(Record), sizeof(Record));
                            fprintf(fptr1, "ID: %d, Name: %s, Surname: %s, City: %s\n\n", r.id, r.name, r.surname, r.city);
                            BF_UnpinBlock(block3);
                            BF_Block_Destroy(&block3);
                        }
                        if(h[j].free == 0){
                            fprintf(fptr1," Examined filename %s, base:%s\n",h[j].onoma_arxeiou,sh[sindexDesc].onoma_protevontos_arxeiou);
                        }
                    }
                

            }




        }
        BF_Block_Destroy(&block2);
        BF_Block_Destroy(&block);
    }
    fclose(fptr1);
    return HT_OK;
}
HT_ErrorCode SHT_HashStatistics(char *filename)
{
    //insert code here
    return HT_OK;
}

HT_ErrorCode SHT_InnerJoin(int sindexDesc1, int sindexDesc2, char *index_key)
{
    fptr1 = fopen("Output1.txt", "a");
    if (fptr1 == NULL)
    {
        return HT_ERROR;
    }
    fprintf(fptr1, "\n\n-- Zefxi bitches %d %d (for entry = %s) --\n\n", sindexDesc1, sindexDesc2, index_key);
    if (sh[sindexDesc1].free == 1 || sh[sindexDesc2].free == 1)
    {
        fclose(fptr1);
        return HT_ERROR;
    }
    else
    {
        char *data[4];
        int fd[2];
        if (strcmp(sh[sindexDesc1].attrName, sh[sindexDesc2].attrName) != 0)
        {
            fclose(fptr1);
            printf("Different zefxi attrs.");
            return HT_ERROR;
        }
        fd[0] = sh[sindexDesc1].anagn_arx;
        fd[1] = sh[sindexDesc2].anagn_arx;

        BF_Block *block[4];
        for (int i = 0; i < 4; i++)
        {
            BF_Block_Init(&block[i]);
        }
        for (int i = 0; i < 2; i++)
        {
            CALL_BF(BF_GetBlock(fd[i], 0, block[i]));
            data[i] = BF_Block_GetData(block[i]);
        }

        int numofblocks[2];

        BF_GetBlockCounter(sh[sindexDesc1].anagn_arx, &numofblocks[0]);
        BF_GetBlockCounter(sh[sindexDesc2].anagn_arx, &numofblocks[1]);

        int numKeys[2] = {1, 1};
        for (int i = 0; i < sh[sindexDesc1].globaldepth; i++)
        {
            numKeys[0] *= 2;
        }
        for (int i = 0; i < sh[sindexDesc2].globaldepth; i++)
        {
            numKeys[1] *= 2;
        }

        //numKeys = 2^d where d = global depth
        ssize_t pos = 0;
        int num_Blocks_Evr = 0;
        for (int i = 0; i < 2; i++)
        {
            fprintf(fptr1, "(%d) There have been %d keys, %d blocks allocated, which means %d bucket-blocks\n", i, numKeys[i], numofblocks[i], numofblocks[i] - 1);
        }
        int val;
        unsigned long int x[2];
        int hash_val[2];
        if (strcmp("city", sh[sindexDesc1].attrName) == 0)
        {
            if (numofblocks[0] == 17)
            {
                printf("City hash\n");
                hash_val[0] = hash_funct1(index_key, sh[sindexDesc1].attrName);
            }
            else
            {
                x[0] = hash_funct2(index_key);
                hash_val[0] = x[0] % numKeys[0];
            }
        }
        else if (strcmp("surname", sh[sindexDesc1].attrName) == 0)
        {
            if (numofblocks[0] == 17)
            {
                hash_val[0] = hash_funct1(index_key, sh[sindexDesc1].attrName);
            }
            else
            {
                x[0] = hash_funct2(index_key);
                hash_val[0] = x[0] % numKeys[0];
            }
        }
        else if ((strcmp("name", sh[sindexDesc1].attrName) == 0))
        {
            if (numofblocks[0] == 17)
            {
                hash_val[0] = hash_funct1(index_key, sh[sindexDesc1].attrName);
            }
            else
            {
                x[0] = hash_funct2(index_key);
                hash_val[0] = x[0] % numKeys[0];
            }
        }
        else
        {
            fclose(fptr1);
            return HT_ERROR;
        }

        if (strcmp("city", sh[sindexDesc2].attrName) == 0)
        {
            if (numofblocks[1] == 17)
            {
                printf("City hash\n");
                hash_val[1] = hash_funct1(index_key, sh[sindexDesc2].attrName);
            }
            else
            {
                x[1] = hash_funct2(index_key);
                hash_val[1] = x[1] % numKeys[1];
            }
        }
        else if (strcmp("surname", sh[sindexDesc2].attrName) == 0)
        {
            if (numofblocks[1] == 17)
            {
                hash_val[1] = hash_funct1(index_key, sh[sindexDesc2].attrName);
            }
            else
            {
                x[1] = hash_funct2(index_key);
                hash_val[1] = x[1] % numKeys[1];
            }
        }
        else if ((strcmp("name", sh[sindexDesc2].attrName) == 0))
        {
            if (numofblocks[1] == 17)
            {
                hash_val[1] = hash_funct1(index_key, sh[sindexDesc2].attrName);
            }
            else
            {
                x[1] = hash_funct2(index_key);
                hash_val[1] = x[1] % numKeys[1];
            }
        }
        else
        {
            fclose(fptr1);
            return HT_ERROR;
        }

        int hash_key_BE[2] = {0, 0};
        int BucketID[2]; // BucketID contains id of bucket

        pos = -2 * sizeof(int);
        num_Blocks_Evr = 0;
        
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE[0], data[0] + pos, sizeof(int)); // Find id of bucket that my hash value points to.

        } while (hash_key_BE[0] != hash_val[0]);

        memcpy(&BucketID[0], data[0] + pos + sizeof(int), sizeof(int));

        pos = -2 * sizeof(int);
        num_Blocks_Evr = 0;
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE[1], data[1] + pos, sizeof(int)); // Find id of bucket that my hash value points to.

        } while (hash_key_BE[1] != hash_val[1]);
        memcpy(&BucketID[1], data[1] + pos + sizeof(int), sizeof(int));
        for(int i=0;i<2; i++){
            fprintf(fptr1, "Record.id is %s, hashed value is = %d, bucketID is %d\n", index_key, hash_val[i],BucketID[i]);
        }

        CALL_BF(BF_GetBlock(fd[0], BucketID[0], block[2]));
        CALL_BF(BF_GetBlock(fd[1], BucketID[1], block[3]));

        data[2] = BF_Block_GetData(block[2]);
        data[3] = BF_Block_GetData(block[3]);

        int numRecords[2];
        int Local_Depth[2];

        for (int i = 0; i < 2; i++)
        {
            memcpy(&numRecords[i], data[i + 2], sizeof(int)); // Extract data about bucket - block
            memcpy(&Local_Depth[i], data[i + 2] + sizeof(int), sizeof(int));
            fprintf(fptr1, "Has LD = %d, numRec = %d\n", Local_Depth[i],numRecords[i]);
        }
        Record R[2];
        if (numofblocks[0] == numofblocks[1])
        {
            if (numofblocks[0] < 17)
            {
                printf("BB");

                // check for both
                ssize_t pos1 = 2 * sizeof(int);
                SecondaryRecord Sr[2];

                for (int i = 0; i < numRecords[0]; i++)
                {
                    memcpy(&Sr[0], data[2] + pos1, sizeof(SecondaryRecord));
                    pos1 += sizeof(SecondaryRecord);
                    ssize_t pos2 = 2 * sizeof(int);
                    for (int j = 0; j < numRecords[1]; j++)
                    {
                        memcpy(&Sr[1], data[3] + pos2, sizeof(SecondaryRecord));
                        pos2 += sizeof(SecondaryRecord);
                        if (strcmp(Sr[0].index_key, Sr[1].index_key) == 0 && strcmp(Sr[0].index_key, index_key) == 0)
                        {
                            int TID;
                            TID = Sr[0].tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc1].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[0], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }
                            TID = Sr[1].tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc2].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[1], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }

                            fprintf(fptr1, "%s %s %s %d || %s %s %s %d\n", R[0].name, R[0].surname, R[0].city, R[0].id, R[1].name, R[1].surname, R[1].city, R[1].id);
                        }
                    }
                }
            }
            else
            {
                printf("AA");

                fprintf(fptr1,"%d * %d = %d\n",numRecords[0],numRecords[1],numRecords[0]*numRecords[1]);
                ssize_t pos1 = 2 * sizeof(int);
                int Sr[2];

                for (int i = 0; i < numRecords[0]; i++)
                {
                    memcpy(&Sr[0], data[2] + pos1, sizeof(int));
                    pos1 += sizeof(int);
                    ssize_t pos2 = 2 * sizeof(int);
                    for (int j = 0; j < numRecords[1]; j++)
                    {
                        memcpy(&Sr[1], data[3] + pos2, sizeof(int));
                        pos2 += sizeof(int);

                        int TID;
                        TID = Sr[0];
                        for (int k = 0; k < MAX_OPEN_FILES; k++)
                        {
                            if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc1].onoma_protevontos_arxeiou) == 0)
                            {
                                int position1 = 0, blono1;
                                for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                {
                                    if ((TID - l) % 8 == 0)
                                    {
                                        position1 = l;
                                        blono1 = (TID - l) / 8 - 1;
                                        break;
                                    }
                                }

                                BF_Block *block3;
                                char *data3;
                                BF_Block_Init(&block3);
                                CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                data3 = BF_Block_GetData(block3);
                                memcpy(&R[0], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                BF_UnpinBlock(block3);
                                BF_Block_Destroy(&block3);
                            }
                        }
                        TID = Sr[1];
                        for (int k = 0; k < MAX_OPEN_FILES; k++)
                        {
                            if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc2].onoma_protevontos_arxeiou) == 0)
                            {
                                int position1 = 0, blono1;
                                for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                {
                                    if ((TID - l) % 8 == 0)
                                    {
                                        position1 = l;
                                        blono1 = (TID - l) / 8 - 1;
                                        break;
                                    }
                                }

                                BF_Block *block3;
                                char *data3;
                                BF_Block_Init(&block3);
                                CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                data3 = BF_Block_GetData(block3);
                                memcpy(&R[1], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                BF_UnpinBlock(block3);
                                BF_Block_Destroy(&block3);
                            }
                        }

                        fprintf(fptr1, "%s %s %s %d || %s %s %s %d\n", R[0].name, R[0].surname, R[0].city, R[0].id, R[1].name, R[1].surname, R[1].city, R[1].id);
                    }
                }
            }
        }
        else
        {
            if (numofblocks[0] == 17)
            {
                printf("AB");

                // check for both
                ssize_t pos1 = 2 * sizeof(int);
                SecondaryRecord Sr2;
                int Sr;

                for (int i = 0; i < numRecords[0]; i++)
                {
                    memcpy(&Sr, data[2] + pos1, sizeof(int));
                    pos1 += sizeof(int);
                    ssize_t pos2 = 2 * sizeof(int);
                    for (int j = 0; j < numRecords[1]; j++)
                    {
                        memcpy(&Sr2, data[3] + pos2, sizeof(SecondaryRecord));
                        pos2 += sizeof(SecondaryRecord);
                        if ( strcmp(Sr2.index_key, index_key) == 0)
                        {
                            int TID;
                            TID = Sr;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc1].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[0], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }
                            TID = Sr2.tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc2].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[1], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }

                            fprintf(fptr1, "%s %s %s %d || %s %s %s %d\n", R[0].name, R[0].surname, R[0].city, R[0].id, R[1].name, R[1].surname, R[1].city, R[1].id);
                        }
                    }
                }
            }
            else if (numofblocks[1] == 17)
            {
                printf("BA\n");
                // check for both
                ssize_t pos1 = 2 * sizeof(int);
                SecondaryRecord Sr;
                int Sr2;

                for (int i = 0; i < numRecords[0]; i++)
                {
                    memcpy(&Sr, data[2] + pos1, sizeof(SecondaryRecord));
                    pos1 += sizeof(SecondaryRecord);
                    ssize_t pos2 = 2 * sizeof(int);
                    for (int j = 0; j < numRecords[1]; j++)
                    {
                        memcpy(&Sr2, data[3] + pos2, sizeof(int));
                        pos2 += sizeof(int);
                        if (strcmp(Sr.index_key, index_key) == 0)
                        {
                            int TID;
                            TID = Sr.tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc1].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[0], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }
                            TID = Sr2;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc2].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[1], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }

                            fprintf(fptr1, "%s %s %s %d || %s %s %s %d\n", R[0].name, R[0].surname, R[0].city, R[0].id, R[1].name, R[1].surname, R[1].city, R[1].id);
                        }
                    }
                }
            }
            else
            {
                printf("BB");

                // check for both
                ssize_t pos1 = 2 * sizeof(int);
                SecondaryRecord Sr[2];

                for (int i = 0; i < numRecords[0]; i++)
                {
                    memcpy(&Sr[0], data[2] + pos1, sizeof(SecondaryRecord));
                    pos1 += sizeof(SecondaryRecord);
                    ssize_t pos2 = 2 * sizeof(int);
                    for (int j = 0; j < numRecords[1]; j++)
                    {
                        memcpy(&Sr[1], data[3] + pos2, sizeof(SecondaryRecord));
                        pos2 += sizeof(SecondaryRecord);
                        if (strcmp(Sr[0].index_key, Sr[1].index_key) == 0 && strcmp(Sr[0].index_key, index_key) == 0)
                        {
                            int TID;
                            TID = Sr[0].tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc1].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[0], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }
                            TID = Sr[1].tupleId;
                            for (int k = 0; k < MAX_OPEN_FILES; k++)
                            {
                                if (h[k].free == 0 && strcmp(h[k].onoma_arxeiou, sh[sindexDesc2].onoma_protevontos_arxeiou) == 0)
                                {
                                    int position1 = 0, blono1;
                                    for (int l = 0; l < floor(MAX_BLOCK_ENTRIES); l++)
                                    {
                                        if ((TID - l) % 8 == 0)
                                        {
                                            position1 = l;
                                            blono1 = (TID - l) / 8 - 1;
                                            break;
                                        }
                                    }

                                    BF_Block *block3;
                                    char *data3;
                                    BF_Block_Init(&block3);
                                    CALL_BF(BF_GetBlock(h[k].anagn_arx, blono1, block3));
                                    data3 = BF_Block_GetData(block3);
                                    memcpy(&R[1], data3 + 2 * sizeof(int) + position1 * sizeof(Record), sizeof(Record));
                                    BF_UnpinBlock(block3);
                                    BF_Block_Destroy(&block3);
                                }
                            }

                            fprintf(fptr1, "%s %s %s %d || %s %s %s %d\n", R[0].name, R[0].surname, R[0].city, R[0].id, R[1].name, R[1].surname, R[1].city, R[1].id);
                        }
                    }
                }
            }
        }
        fclose(fptr1);
        return HT_OK;
    }
}