#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/bf.h"
#include "../include/hash_file.h"
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

extern HT_Index *h;    // Declare Global Files Array
extern Buckets_Ind *b; // Declare Create Requests List
extern SHT_Index *sh;
FILE *fptr;

HT_ErrorCode HT_Init()
{
    BF_Init(LRU);

    fptr = fopen("Output1.txt", "w+");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    fclose(fptr);
    fptr = fopen("Hash_Stats.txt", "w+"); // Create files that will contain results
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    fclose(fptr);

    fptr = fopen("All_Entries.txt", "w+");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    fclose(fptr);
    fptr = fopen("Output.txt", "w+");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }

    h = malloc(MAX_OPEN_FILES * sizeof(HT_Index)); // Initiliaze Global Files Array
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        (h)[i].free = 1;
        (h)[i].globaldepth = -1;
    }

    b = malloc(sizeof(Buckets_Ind)); // And Create Requests List (head)
    b->next = NULL;
    b->prev = NULL;
    b->startingglobaldepth = -1;
    fprintf(fptr, "Initialized Structs\n");
    fclose(fptr);
    return HT_OK;
}

HT_ErrorCode HT_CreateIndex(const char *filename, int depth)
{

    fptr = fopen("Output.txt", "a");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    fprintf(fptr, "-- %s --\n", filename);

    if (depth > MAX_DEPTH)
    {
        fprintf(fptr, "Creation request with more than allowed max depth, create failed\n");
        return HT_ERROR;
    }
    CALL_BF(BF_CreateFile(filename));

    Buckets_Ind *t; //Add Create File Request to end of List
    t = b->next;
    Buckets_Ind *p;
    p = b;
    while (t != NULL)
    {
        p = t;
        t = t->next;
    }
    t = malloc(sizeof(Buckets_Ind));
    t->next = NULL;
    t->prev = p;
    p->next = t;
    strcpy(t->filename, filename);
    t->protevon = 1;
    t->startingglobaldepth = depth;
    fprintf(fptr, "Added Create File Request to List - FileName: %s, StartingGlobalDepth: %d\n", t->filename, depth);
    fclose(fptr);

    return HT_OK;
}

HT_ErrorCode HT_OpenIndex(const char *fileName, int *indexDesc)
{
    fptr = fopen("Output.txt", "a");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    Buckets_Ind *t;
    fprintf(fptr, "-- %s --\n", fileName);
    t = b->next;
    if (t == NULL)
    {
        fprintf(fptr, "There hasn't been a Create File Request yet\n");
        fclose(fptr);

        return HT_ERROR;
    }

    while (strcmp(t->filename, fileName) != 0)
    {

        t = t->next;
        if (t == NULL)
        {
            fprintf(fptr, "No Created Files match the Requested Filename\n");
            fclose(fptr);
            return HT_ERROR;
        }
    }
    int depth;
    if (strcmp(t->filename, fileName) == 0)
    {
        depth = t->startingglobaldepth;
    }

    int fd;
    *indexDesc = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) // Find if there are MAX_OPEN_FILES files opened, if not open this one
    {

        if (h[i].free == 1)
        {
            *indexDesc = i;
            h[i].free = 0;
            break;
        }
    }

    if (*indexDesc == -1)
    {
        fprintf(fptr, "Max number of allowed files has already been opened\n");
        fclose(fptr);
        return HT_ERROR;
    }
    CALL_BF(BF_OpenFile(fileName, &fd));
    h[*indexDesc].anagn_arx = fd;
    strcpy(h[*indexDesc].onoma_arxeiou,fileName);
    int blocksAllocated;
    BF_GetBlockCounter(fd, &blocksAllocated);
    if (blocksAllocated != 0) // No need for Allocation of Block_Evretiorou if the file has already opened
    {
        fprintf(fptr, "Opening a file that has been opened in the past\n");

        BF_Block *block;
        BF_Block_Init(&block);

        char *data[MAX_NUM_BLOCK_EVR];
        for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
        {
            CALL_BF(BF_GetBlock(h[*indexDesc].anagn_arx, i, block));
            data[i] = BF_Block_GetData(block);
        }

        int i, pos = 0, c = 0;
        int num_Blocks_Evr = 0;
        do
        {
            memcpy(&i, data[num_Blocks_Evr] + pos, sizeof(i));
            pos += 2 * sizeof(i);
            c++;
            if (pos == BF_BLOCK_SIZE)
            {
                pos = 0;
                num_Blocks_Evr++;
            }
        } while (i != -1 && num_Blocks_Evr < MAX_NUM_BLOCK_EVR);
        if (i == -1)
        {
            c -= 1;
        }
        fprintf(fptr, "There are already %d key-bucketID sets saved in Blocks Evretiriou, ", c);
        int j = 0;
        while (c > 1)
        {
            c = c >> 1;
            j++;
        }
        fprintf(fptr, "therefore we can deduce that its global depth is %d\n\n", j);
        h[*indexDesc].globaldepth = j; // Update relevant globaldepth in array of files
        BF_Block_Destroy(&block);
    }
    else
    {
        BF_Block *block;
        BF_Block_Init(&block);
        int j = 1;
        fprintf(fptr, "Opening this file for the first time\n");

        for (int i = 0; i < depth; i++)
        {
            j *= 2;
        }
        h[*indexDesc].globaldepth = depth;
        char *data[MAX_NUM_BLOCK_EVR], *data2;
        const int zero = 0;
        int pointed_bucket_id[j / 2];

        for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
        {
            CALL_BF(BF_AllocateBlock(fd, block)); // Allocate Block evretiriou
            data[i] = BF_Block_GetData(block);
        }
        for (int i = 0; i < j / 2; i++) // Allocate Bucket - BLocks and update relevant values
        {
            //fprintf(fptr, "Allocating Starting Blocks/Buckets\n");
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
        int num_Blocks_Evr = 0;
        for (int i = 0; i < j; i++) // Update relevant values for each key entry in Blocks Evretiriou
        {
            if (i % 2 == 0)
            {
                memcpy(data[num_Blocks_Evr] + pos, &i, sizeof(i));
                pos += sizeof(i);
                memcpy(data[num_Blocks_Evr] + pos, &pointed_bucket_id[i / 2], sizeof(pointed_bucket_id[i / 2]));
                pos += sizeof(pointed_bucket_id[i / 2]);
            }
            else
            {
                memcpy(data[num_Blocks_Evr] + pos, &i, sizeof(i));
                pos += sizeof(i);
                memcpy(data[num_Blocks_Evr] + pos, &pointed_bucket_id[(i - 1) / 2], sizeof(pointed_bucket_id[(i - 1) / 2]));
                pos += sizeof(pointed_bucket_id[(i - 1) / 2]);
            }
            if (pos == BF_BLOCK_SIZE)
            {
                pos = 0;
                num_Blocks_Evr++;
            }
        }
        if (num_Blocks_Evr != MAX_NUM_BLOCK_EVR) // Update endpoint of Block Evretiriou if not every Block evretiriou is full
        {
            int flag = -1;
            memcpy(data[num_Blocks_Evr] + pos, &flag, sizeof(flag));
            CALL_BF(BF_GetBlock(fd, num_Blocks_Evr, block));
            data[num_Blocks_Evr] = BF_Block_GetData(block);
            BF_Block_SetDirty(block);
        }

        for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
        {
            CALL_BF(BF_GetBlock(fd, i, block));
            data[i] = BF_Block_GetData(block);
        }

        pos = 0;
        int keyVal = 0, bucketID = 0;
        num_Blocks_Evr = 0;
        for (int i = 0; i < j; i++) // Print Values Block Evretiriou
        {
            memcpy(&keyVal, data[num_Blocks_Evr] + pos, sizeof(keyVal));
            pos += sizeof(keyVal);
            memcpy(&bucketID, data[num_Blocks_Evr] + pos, sizeof(bucketID));
            pos += sizeof(bucketID);
            fprintf(fptr, "KeyVal:%d => BucketID:%d\n", keyVal, bucketID);
            BF_Block_SetDirty(block);
            if (pos == BF_BLOCK_SIZE)
            {
                pos = 0;
                num_Blocks_Evr++;
            }
        }
        fprintf(fptr, "\n");
        BF_Block_Destroy(&block);
    }
    fclose(fptr);
    return HT_OK;
}

HT_ErrorCode HT_CloseFile(int indexDesc)
{

    fptr = fopen("Output.txt", "a");
    if (fptr == NULL)
    {
        fclose(fptr);
        return HT_ERROR;
    }
    fprintf(fptr, "-- %d --\n", indexDesc);

    if (h[indexDesc].free == 0)
    {
        BF_CloseFile(h[indexDesc].anagn_arx);
        strcpy(h[indexDesc].onoma_arxeiou,"dummy text");
        h[indexDesc].anagn_arx = -1;
        h[indexDesc].free = 1;
        h[indexDesc].globaldepth = -1;
        fprintf(fptr, "Closed file that was occupying the %d position of the file array.\n", indexDesc + 1);
    }
    else
    {
        fclose(fptr);
        return HT_ERROR;
    }

    fclose(fptr);
    return HT_OK;
}

HT_ErrorCode HT_InsertEntry(int indexDesc, Record record, int *tuppleid)
{

    fptr = fopen("Output.txt", "a");
    if (fptr == NULL)
    {
        return HT_ERROR;
    }
    fprintf(fptr, "-- %d --\n", indexDesc);

    if (h[indexDesc].free == 1)
    {
        fclose(fptr);
        return HT_ERROR;
    }
    else
    {
        char *data[MAX_NUM_BLOCK_EVR], *data2;
        int fd;
        fd = h[indexDesc].anagn_arx;
        BF_Block *block;
        BF_Block_Init(&block);
        for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
        {
            BF_GetBlock(fd, i, block);
            data[i] = BF_Block_GetData(block);
        }
        int numofblocks;
        BF_GetBlockCounter(h[indexDesc].anagn_arx, &numofblocks);
        int numKeys = 1;
        for (int i = 0; i < h[indexDesc].globaldepth; i++)
        {
            numKeys *= 2;
        } //numKeys = 2^d where d = global depth
        ssize_t pos = 0;
        int num_Blocks_Evr = 0;
        fprintf(fptr, "There have been %d blocks allocated, which means %d bucket-blocks\n", numofblocks, numofblocks - MAX_NUM_BLOCK_EVR);
        int x = hash_funct(record.id);
        int hash_val = x % numKeys;
        int hash_key_BE = 0;
        pos = -2 * sizeof(int);
        num_Blocks_Evr = 0;
        fprintf(fptr, "Record.id is %d, hashed value is = %d", record.id, hash_val);
        do
        {
            pos += 2 * sizeof(int);
            memcpy(&hash_key_BE, data[num_Blocks_Evr] + pos, sizeof(int)); // Find id of bucket that my hash value points to.
            if (pos == BF_BLOCK_SIZE)
            {
                pos = -2 * sizeof(int);
                num_Blocks_Evr++;
            }
        } while (hash_key_BE != hash_val);
        int BucketID; // BucketID contains id of bucket

        memcpy(&BucketID, data[num_Blocks_Evr] + pos + sizeof(int), sizeof(int));
        BF_Block *block2;
        BF_Block_Init(&block2);
        fprintf(fptr, ", so it belongs to bucket- block with ID = %d, ", BucketID);
        CALL_BF(BF_GetBlock(fd, BucketID, block2));
        data2 = BF_Block_GetData(block2);
        int numRecords;

        memcpy(&numRecords, data2, sizeof(int)); // Extract data about bucket - block
        int Local_Depth;
        memcpy(&Local_Depth, data2 + sizeof(int), sizeof(int));
        fprintf(fptr, "which already includes %d records and has local depth of = %d\n", numRecords, Local_Depth);


        if (numRecords < floor(MAX_BLOCK_ENTRIES))
        {
            *tuppleid = (BucketID + 1) * floor(MAX_BLOCK_ENTRIES) + numRecords;
            pos = 2 * sizeof(int) + numRecords * sizeof(Record);
            memcpy(data2 + pos, &record, sizeof(Record));
            numRecords += 1;
            memcpy(data2, &numRecords, sizeof(int)); // Update number of records in bucket - block
            BF_Block_SetDirty(block2);
            BF_UnpinBlock(block2);
            fprintf(fptr, "Successful eggrafi, with tuple id = %d\n", *tuppleid);
            BF_Block_Destroy(&block);
            BF_Block_Destroy(&block2);
        }
        else
        {
            fprintf(fptr, "\nMax block entries detected, checking for buddy\n");
            int Global_Depth = h[indexDesc].globaldepth;
            if (Local_Depth < Global_Depth) // CASE 1: LOCAL DEPTH < GLOBAL DEPTH
            {
                fprintf(fptr, "Buddy found!");
                int buddy_ID;
                pos = -2 * sizeof(int);
                num_Blocks_Evr = 0;
                if (hash_val % 2 == 0)
                {

                    do
                    {
                        pos += 2 * sizeof(int);
                        memcpy(&hash_key_BE, data[num_Blocks_Evr] + pos, sizeof(int)); // Locate buddy if hash_key is even
                        if (pos == BF_BLOCK_SIZE)
                        {
                            pos = -2 * sizeof(int);
                            num_Blocks_Evr++;
                        }
                    } while (hash_key_BE != hash_val + 1);
                }
                else
                {

                    do
                    {
                        pos += 2 * sizeof(int);
                        memcpy(&hash_key_BE, data[num_Blocks_Evr] + pos, sizeof(int)); // Locate buddy if hash_key is odd
                        if (pos == BF_BLOCK_SIZE)
                        {
                            pos = -2 * sizeof(int);
                            num_Blocks_Evr++;
                        }
                    } while (hash_key_BE != hash_val - 1);
                }
                fprintf(fptr, "=> Hash buddy of %d is %d\n", hash_val, hash_key_BE);
                memcpy(&buddy_ID, data[num_Blocks_Evr] + pos + sizeof(int), sizeof(int));
                fprintf(fptr, "\n(Before Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID =  %d\n", buddy_ID, BucketID);
                if (buddy_ID != BucketID)
                {
                    fprintf(fptr, "Logic Error!\n"); // Should never happen
                    fclose(fptr);
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
                Record r[MAX_BLOCK_ENTRIES];
                for (int i = 0; i < MAX_BLOCK_ENTRIES; i++)
                {
                    memcpy(&r[i], data2 + 2 * sizeof(int) + i * sizeof(Record), sizeof(Record));
                }
                BF_Block_SetDirty(block2);
                BF_UnpinBlock(block2);

                CALL_BF(BF_GetBlockCounter(h[indexDesc].anagn_arx, &buddy_ID));
                buddy_ID -= 1;
                memcpy(data[num_Blocks_Evr] + pos + sizeof(int), &buddy_ID, sizeof(int)); // Update where hash_key of buddy points to (newest bucket- block)
                fprintf(fptr, "(After Splitting) Buddy is pointing at bucket with ID = %d, while I am pointing at bucket with ID = %d\n", buddy_ID, BucketID);
                if (buddy_ID == BucketID)
                {
                    fprintf(fptr, "Logic Error!\n"); // Should never happen
                    fclose(fptr);
                    return HT_ERROR;
                }
                for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
                {
                    CALL_BF(BF_GetBlock(fd, num_Blocks_Evr, block2));
                    BF_Block_SetDirty(block2);
                }
                int tupples;
                fprintf(fptr, "\nBeginning rehashing\n");
                fclose(fptr);
                BF_Block_SetDirty(block);
                BF_Block_Destroy(&block);
                BF_Block_Destroy(&block2);
                int i,flag = 0;
                for (i = 0; i < MAX_OPEN_FILES; i++)
                {
                    if(strcmp(h[indexDesc].onoma_arxeiou,sh[i].onoma_protevontos_arxeiou) == 0 && sh[i].free == 0)
                    {
                        printf("Found my defterevon for rehash, its in postion (%s = %s), %d\n", h[indexDesc].onoma_arxeiou, sh[i].onoma_protevontos_arxeiou,i);
                        for (int j = 0; j < MAX_BLOCK_ENTRIES; j++)
                        { 
                            CALL_BF(HT_InsertEntry(indexDesc, r[j], &tupples)); //Tupples holds new tupleID for secondary record to be updated
                            UpdateRecordArray *u;
                            u = malloc(sizeof(UpdateRecordArray));
                            strcpy(u->city, r[j].city);
                            strcpy(u->name, r[j].name);
                            strcpy(u->surname, r[j].surname);
                            u->oldTupleId = (BucketID + 1) * floor(MAX_BLOCK_ENTRIES) + j;
                            u->newTupleId = tupples;
                            printf("Updating rec , %s %s, %s , %d => %d\n", u->name, u->surname, u->city, u->oldTupleId, u->newTupleId);
                            if(u->newTupleId == u->oldTupleId){
                                printf("Nevermind!\n");
                                free(u);
                                continue;
                            }
                            CALL_BF(SHT_SecondaryUpdateEntry(i, u));
                            printf("Done updating rec ;P\n");
                            free(u);
                        }
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0){
                    fprintf(fptr,"Relevant secondary file not opened!\n");
                    for (int i = 0; i < MAX_BLOCK_ENTRIES; i++)
                    {
                        CALL_BF(HT_InsertEntry(indexDesc, r[i], &tupples));
                    }
                }

                CALL_BF(HT_InsertEntry(indexDesc, record, &tupples));
                *tuppleid = tupples;
                fptr = fopen("Output.txt", "a");
                if (fptr == NULL)
                {
                    return HT_ERROR;
                }
                fprintf(fptr, "\nFinished rehashing\n");

            }
            else // CASE 2: LOCAL DEPTH = GLOBAL DEPTH
            {

                fprintf(fptr, "No buddy, trying array expansion:");
                if (h[indexDesc].globaldepth == MAX_DEPTH)
                {
                    fprintf(fptr, " Cannot expand anymore, sorry!\n"); // Record is not saved, but program does not finish because that doesnt mean all Bucket - Blocks are full!
                }
                else
                {
                    h[indexDesc].globaldepth += 1; // Increase global depth
                    num_Blocks_Evr = 0;
                    pos = 0;
                    int HK_ev1, HK_ev2[numKeys], bucketID[numKeys]; //in 3 old block id
                    fprintf(fptr, "\nOld global depth = %d => no. of existing hash keys = 2^d = %d\n", h[indexDesc].globaldepth - 1, numKeys);
                    for (int i = 0; i < numKeys; i++)
                    {

                        memcpy(&HK_ev1, data[num_Blocks_Evr] + pos, sizeof(int)); // For each existing Hash_key - Bucket ID pair, update Hash_key and create a new one as Well
                        HK_ev1 = HK_ev1 * 2;
                        memcpy(data[num_Blocks_Evr] + pos, &HK_ev1, sizeof(int));
                        HK_ev2[i] = HK_ev1 + 1;
                        fprintf(fptr, "Hash key evolution: from %d -> %d & %d\n", HK_ev1 / 2, HK_ev1, HK_ev1 + 1);
                        memcpy(&bucketID[i], data[num_Blocks_Evr] + pos + sizeof(int), sizeof(int));
                        pos += 2 * sizeof(int);
                        if (pos == BF_BLOCK_SIZE)
                        {
                            pos = 0;
                            num_Blocks_Evr++;
                        }
                    }
                    for (int i = 0; i < numKeys; i++) // All new Hash_key - Bucket ID pairs are added in the blocks_evretiriou
                    {
                        memcpy(data[num_Blocks_Evr] + pos, &HK_ev2[i], sizeof(int));
                        memcpy(data[num_Blocks_Evr] + pos + sizeof(int), &bucketID[i], sizeof(int));
                        pos += 2 * sizeof(int);
                        if (pos == BF_BLOCK_SIZE)
                        {
                            pos = 0;
                            num_Blocks_Evr++;
                        }
                    }

                    if (num_Blocks_Evr != MAX_NUM_BLOCK_EVR)
                    {
                        fprintf(fptr, "Updating new endpoint in block evr %d, pos %ld\n", num_Blocks_Evr, pos); // Update new block-evretiriou endpoint
                        int flag = -1;
                        memcpy(data[num_Blocks_Evr] + pos, &flag, sizeof(flag));
                        CALL_BF(BF_GetBlock(fd, num_Blocks_Evr, block));
                        data[num_Blocks_Evr] = BF_Block_GetData(block);
                        BF_Block_SetDirty(block);
                    }
                    BF_Block_SetDirty(block2);
                    BF_UnpinBlock(block2);
                    for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
                    {
                        BF_GetBlock(h[indexDesc].anagn_arx, i, block);
                        BF_Block_SetDirty(block);
                    }
                    BF_Block_Destroy(&block2);

                    pos = 0;
                    num_Blocks_Evr = 0;
                    int hi,bucketid, cc = 0;
                    Record Recs[1200];
                    int ss[1200],ss2[1200];
                    for (int i = 0; i < numKeys; i++) // All new Hash_key - Bucket ID pairs are added in the blocks_evretiriou
                    {
                        memcpy(&hi,data[num_Blocks_Evr] + pos, sizeof(int));
                        memcpy(&bucketid,data[num_Blocks_Evr] + pos + sizeof(int), sizeof(int));
                        BF_Block_Init(&block2);
                        BF_GetBlock( h[indexDesc].anagn_arx,bucketid,block2);
                        data2 = BF_Block_GetData(block2);
                        int rec;
                        memcpy(&rec,data2,sizeof(int));
                        ssize_t pos2 = 2*sizeof(int);
                        int k;
                        for(int j=0; j<rec; j++){
                            
                            memcpy(&Recs[cc], data2 + pos2, sizeof(Record));
                            ss[cc] = bucketid;
                            ss2[cc] = j;
                            cc++;
                            pos2+=sizeof(Record);
                        }
                        fprintf(fptr,"While examining Hash val %d => Bucket_ID %d : Read %d records for the big rehash, this moment %d records in total\n",hi,bucketid,rec,cc);
                        rec = 0;
                        memcpy(data2,&rec,sizeof(int));
                        memcpy(&rec, data2,sizeof(int));
                        fprintf(fptr, "Now there are %d records here\n", rec);
                        pos += 2 * sizeof(int);
                        if (pos == BF_BLOCK_SIZE)
                        {
                            pos = 0;
                            num_Blocks_Evr++;
                        }
                        BF_Block_SetDirty(block2);
                        BF_UnpinBlock(block2);
                        BF_Block_Destroy(&block2);

                    }
                    int tupples, flag = 0;
                    fclose(fptr);

                    for (int i = 0; i < MAX_OPEN_FILES; i++)
                    {
                        if (strcmp(h[indexDesc].onoma_arxeiou, sh[i].onoma_protevontos_arxeiou) == 0 && sh[i].free == 0)
                        {
                            fptr = fopen("Output.txt", "a");
                            fprintf(fptr,"Found my defterevon for BIG rehash, its in postion (%s = %s), %d\n", h[indexDesc].onoma_arxeiou, sh[i].onoma_protevontos_arxeiou, i);
                            fclose(fptr);
                            for (int j = 0; j < cc; j++)
                            {
                                CALL_BF(HT_InsertEntry(indexDesc, Recs[j], &tupples));
                                UpdateRecordArray *u;
                                u = malloc(sizeof(UpdateRecordArray));
                                strcpy(u->city, Recs[j].city);
                                strcpy(u->name, Recs[j].name);
                                strcpy(u->surname, Recs[j].surname);
                                u->oldTupleId = (ss[j] + 1) * floor(MAX_BLOCK_ENTRIES) + ss2[j];
                                u->newTupleId = tupples;
                                fptr = fopen("Output.txt", "a");

                                fprintf(fptr,"Updating rec , %s %s, %s , %d => %d\n", u->name, u->surname, u->city, u->oldTupleId, u->newTupleId);
                                fclose(fptr);

                                if (u->newTupleId == u->oldTupleId)
                                {
                                    printf("Nevermind!\n");
                                    free(u);
                                    continue;
                                }
                                CALL_BF(SHT_SecondaryUpdateEntry(i, u));
                                printf("Done updating rec :)\n");
                                free(u);
                            }
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 0)
                    {
                        fptr = fopen("Output.txt", "a");
                        fprintf(fptr,"Relevant secondary file not opened!\n");
                        fclose(fptr);
                        for (int i = 0; i < cc; i++)
                        {
                            CALL_BF(HT_InsertEntry(indexDesc, Recs[i], &tupples));
                        }
                    }
                    BF_Block_Destroy(&block);
                    CALL_BF(HT_InsertEntry(indexDesc, record,&tupples)); // Insert entry
                    *tuppleid = tupples;

                    fptr = fopen("Output.txt", "a");
                    if (fptr == NULL)
                    {
                        return HT_ERROR;
                    }
                }
            }
    
        }

    }
    
    fclose(fptr);
    return HT_OK;
}

HT_ErrorCode HT_PrintAllEntries(int indexDesc, int *id)
{
    fptr = fopen("All_Entries.txt", "a");
    if (fptr == NULL)
    {
        fclose(fptr);
        return HT_ERROR;
    }
    fprintf(fptr, "-- %d --\n", indexDesc);

    if (h[indexDesc].free == 1)
    {
        fclose(fptr);
        return HT_ERROR;
    }
    else
    {
        fprintf(fptr, "\n\n------------- PRINT ALL ENTRIES ------------- ");
        if (id != NULL)
        {
            fprintf(fptr, "Searching for record with id = %d\n", *id); // Search for record in its specific bucket
            char *data[MAX_NUM_BLOCK_EVR], *data2;
            int fd;
            fd = h[indexDesc].anagn_arx;
            BF_Block *block;
            BF_Block_Init(&block);
            int blocksAllocated;
            BF_GetBlockCounter(fd, &blocksAllocated);
            for (int i = 0; i < MAX_NUM_BLOCK_EVR; i++)
            {
                BF_GetBlock(fd, i, block);
                data[i] = BF_Block_GetData(block);
            }
            int numKeys = 1;
            for (int i = 0; i < h[indexDesc].globaldepth; i++)
            {
                numKeys *= 2;
            }
            int x = hash_funct(*id);
            int hash_val = x % numKeys;
            int hash_key_BE = 0;
            ssize_t pos = -2 * sizeof(int);
            int num_Blocks_Evr = 0;
            fprintf(fptr, "Record.id is %d, its hashed value is = %d\n", *id, hash_val);
            do
            {
                pos += 2 * sizeof(int);
                memcpy(&hash_key_BE, data[num_Blocks_Evr] + pos, sizeof(int));
                if (pos == BF_BLOCK_SIZE)
                {
                    pos = -2 * sizeof(int);
                    num_Blocks_Evr++;
                }
            } while (hash_key_BE != hash_val && hash_key_BE != -1);
            int BucketID; // BucketID contains id of bucket

            memcpy(&BucketID, data[num_Blocks_Evr] + pos + sizeof(int), sizeof(int));
            BF_Block *block2;
            BF_Block_Init(&block2);
            fprintf(fptr, "Looking in bucket with ID = %d \n", BucketID);
            CALL_BF(BF_GetBlock(fd, BucketID, block2));
            data2 = BF_Block_GetData(block2);
            int numRecords;
            int j = 0;
            memcpy(&numRecords, data2, sizeof(int));

            int found = 0;
            while (j < numRecords)
            {
                Record t;
                memcpy(&t, data2 + 2 * sizeof(int) + j * sizeof(Record), sizeof(Record));

                if (t.id == *id)
                {
                    fprintf(fptr, "Requested Record:\n\nID: %d, Name: %s, Surname: %s, City: %s\n\n", t.id, t.name, t.surname, t.city);
                    found = 1;
                }
                j++;
            }
            if (found == 0)
            {
                fprintf(fptr, "Record with ID=%d was not found\n", *id);
            }
            BF_UnpinBlock(block2);
            BF_Block_Destroy(&block);
            BF_Block_Destroy(&block2);
        }
        else
        {
            fprintf(fptr, "Printing all records\n"); // Search in all buckets
            char *data2;
            int fd;
            fd = h[indexDesc].anagn_arx;
            BF_Block *block;
            BF_Block_Init(&block);
            int blocksAllocated;
            int sum = 0;
            BF_GetBlockCounter(fd, &blocksAllocated);
            for (int i = MAX_NUM_BLOCK_EVR; i < blocksAllocated; i++)
            {
                fprintf(fptr, "-- Bucket no.%d ", i);
                BF_Block *block2;
                BF_Block_Init(&block2);
                BF_GetBlock(fd, i, block2);
                data2 = BF_Block_GetData(block2);
                int j = 0, numRecords;
                memcpy(&numRecords, data2, sizeof(int));
                fprintf(fptr, "with %d Records\n", numRecords);
                sum += numRecords;
                while (j < numRecords)
                {
                    Record t;
                    memcpy(&t, data2 + 2 * sizeof(int) + j * sizeof(Record), sizeof(Record));
                    fprintf(fptr, "Record no.%d\n", j + 1);
                    fprintf(fptr, "ID: %d, Name: %s, Surname: %s, City: %s\n\n", t.id, t.name, t.surname, t.city);
                    j++;
                }
                BF_UnpinBlock(block2);
                BF_Block_Destroy(&block2);
            }

            BF_Block_Destroy(&block);
        }
    }
    fclose(fptr);
    return HT_OK;
}

HT_ErrorCode HashStatistics(char *filename)
{
    fptr = fopen("Hash_Stats.txt", "a");
    if (fptr == NULL)
    {
        fclose(fptr);
        return HT_ERROR;
    }
    fprintf(fptr, "-- %s --\n", filename);
    fprintf(fptr, "\n-------------- HASH STATISTICS --------------\n");

    int fd;
    fclose(fptr);
    HT_OpenIndex(filename, &fd);
    fptr = fopen("Hash_Stats.txt", "a");
    char *data2;
    BF_Block *block;
    BF_Block_Init(&block);
    int blocksAllocated;
    BF_GetBlockCounter(h[fd].anagn_arx, &blocksAllocated);
    fprintf(fptr, "There have been %d blocks allocated\n", blocksAllocated);
    int max = -1, min = -1, sum = 0;
    for (int i = MAX_NUM_BLOCK_EVR; i < blocksAllocated; i++)
    {
        //fprintf(fptr, "In bucket with ID %d: ", i);
        BF_Block *block2;
        BF_Block_Init(&block2);
        BF_GetBlock(h[fd].anagn_arx, i, block2);
        data2 = BF_Block_GetData(block2);
        int eggrafesbucket;
        memcpy(&eggrafesbucket, data2, sizeof(int));
        //fprintf(fptr, "Detected %d eggrafes\n", eggrafesbucket);
        sum += eggrafesbucket;
        if (eggrafesbucket < min || min == -1)
            min = eggrafesbucket;
        if (eggrafesbucket > max)
            max = eggrafesbucket;

        BF_UnpinBlock(block2);
        BF_Block_Destroy(&block2);
    }
    fprintf(fptr, "\nMax Records: %d, Min Records: %d, Average Records (per Bucket): %.3f, Total Records: %d\n", max, min, (float)sum / (blocksAllocated - MAX_NUM_BLOCK_EVR), sum);
    fclose(fptr);
    BF_Block_Destroy(&block);

    HT_CloseFile(fd);
    return HT_OK;
}

int hash_funct(int x)
{
    return x;
}