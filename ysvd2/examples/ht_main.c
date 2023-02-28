#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "bf.h"
#include "hash_file.h"

#define RECORDS_NUM 2050 // you can change it if you want
#define GLOBAL_DEPT 2    // you can change it if you want
#define FILE_NAME1 "data1.db"
#define FILE_NAME2 "data2.db"

const char *names[] = {
    "Yannis",
    "Christofos",
    "Sofia",
    "Marianna",
    "Vagelis",
    "Maria",
    "Iosif",
    "Dionisis",
    "Konstantina",
    "Theofilos",
    "Giorgos",
    "Dimitris",
    "Stefanos"};

const char *surnames[] = {
    "Ioannidis",
    "Svingos",
    "Karvounari",
    "Rezkalla",
    "Nikolopoulos",
    "Berreta",
    "Koronis",
    "Gaitanis",
    "Oikonomou",
    "Mailis",
    "Michas",
    "Halatsis"};

const char *cities[] = {
    "Athens",
    "San Francisco",
    "Los Angeles",
    "Amsterdam",
    "London",
    "New York",
    "Tokyo",
    "Hong Kong",
    "Munich",
    "Miami"};

#define CALL_OR_DIE(call)         \
    {                             \
        HT_ErrorCode code = call; \
        if (code != HT_OK)        \
        {                         \
            printf("Error\n");    \
            exit(code);           \
        }                         \
    }

HT_Index *h;
Buckets_Ind *b;
extern FILE *fptr;
int main()
{

    printf("Running Program with:\n");
    printf("GLOBAL_DEPT = %d, RECORDS_NUM = %d, MAX_NUM_BLOCK_EVR = %d, MAX_BLOCK_ENTRIES = %d, MAX_DEPTH = %d\n", GLOBAL_DEPT, RECORDS_NUM, MAX_NUM_BLOCK_EVR, (int)floor(MAX_BLOCK_ENTRIES), MAX_DEPTH);
    CALL_OR_DIE(HT_Init());
    int indexDesc1, indexDesc2;
    //CALL_OR_DIE(HT_CreateIndex(FILE_NAME1, GLOBAL_DEPT+2));
    CALL_OR_DIE(HT_CreateIndex(FILE_NAME2, GLOBAL_DEPT));
    printf("Creates complete\n");
    CALL_OR_DIE(HT_OpenIndex(FILE_NAME2, &indexDesc2));

    //CALL_OR_DIE(HT_OpenIndex(FILE_NAME1, &indexDesc1));                     // Closing and Reopening file to check OpenIndex works properly

    CALL_OR_DIE(HT_CloseFile(indexDesc2));
    CALL_OR_DIE(HT_OpenIndex(FILE_NAME2, &indexDesc2));

    Record record;
    srand(time(NULL));
    int r;
    printf("Inserting Entries...\n");
    for (int id = 0; id < RECORDS_NUM; ++id)
    {
        // create a record
        record.id = id;
        r = rand() % 13;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 12;
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 10;
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        CALL_OR_DIE(HT_InsertEntry(indexDesc2, record));
    }

    /*    for (int id = RECORDS_NUM; id > 0; --id)
    {
        // create a record
        record.id = id;
        r = rand() % 13;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 12;
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 10;
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        CALL_OR_DIE(HT_InsertEntry(indexDesc1, record));
    }
*/

    int id = rand() % RECORDS_NUM;
    printf("ID requested is %d\n", id);
    CALL_OR_DIE(HT_PrintAllEntries(indexDesc2, &id));
    CALL_OR_DIE(HT_PrintAllEntries(indexDesc2, NULL));
    CALL_OR_DIE(HT_CloseFile(indexDesc2));
    //CALL_OR_DIE(HT_CloseFile(indexDesc1));
    //CALL_OR_DIE(HashStatistics(FILE_NAME1));

    CALL_OR_DIE(HashStatistics(FILE_NAME2));
    BF_Close();
    free(h);
    Buckets_Ind *t, *p;
    t = b;
    do
    {
        p = t;
        t = t->next;
        free(p);
    } while (t != NULL);
    printf("Exiting Succesfully! Results in .txt's\n");
}