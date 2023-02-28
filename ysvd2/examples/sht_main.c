#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../include/bf.h"
#include "../include/hash_file.h"
#include "../include/sht_file.h"

#define RECORDS_NUM 500 // you can change it if you want
#define GLOBAL_DEPT 2 // you can change it if you want
#define FILE_NAME1 "data1.db"
#define FILE_NAME2 "data2.db"
#define FILE_NAME3 "secondary_data1.db"
#define FILE_NAME4 "secondary_data2.db"

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
    "Stefanos",
    "Phillipos"

};

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
    "Halatsis",
    "Ntatsis",
    "Vavoulas"
    };

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
    "Miami",
    "Patras",
    "Reyjkavik",
    "Philadelphia",
    "Cairo"};

#define CALL_OR_DIE(call)     \
  {                           \
    HT_ErrorCode code = call; \
    if (code != HT_OK) {      \
      printf("Error\n");      \
      exit(code);             \
    }                         \
  }

unsigned long hash_funct2(const char *s)
{
    unsigned long hash = 5381;
    int c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
int hash_funct1(const char *s, const char *r)
{

    if (strcmp("surname", r) == 0)
    {
        for (int i = 0; i < 14; i++)
        {
            if (strcmp(surnames[i], s) == 0)
            {
                return i + 1;
            }
        }
        return -1;
    }
    else if (strcmp("name", r) == 0)
    {
        for (int i = 0; i < 14; i++)
        {
            if (strcmp(names[i], s) == 0)
            {
                return i + 1;
            }
        }
        return -1;
    }
    else if (strcmp("city", r) == 0)
    {
        for (int i = 0; i < 14; i++)
        {
            if (strcmp(cities[i], s) == 0)
            {
                printf("%s = %s\n", cities[i], s);
                return i + 1;
            }
        }
        printf("????? \n");

        return -1;
    }
    else
    {
        return -1;
    }
}

HT_Index *h;
SHT_Index *sh;
Buckets_Ind *b;
extern FILE *fptr;
extern FILE *fptr1;

int main() {
    int HELLO[RECORDS_NUM];
    printf("Running Program with:\n");
    printf("GLOBAL_DEPT = %d, RECORDS_NUM = %d, MAX_NUM_BLOCK_EVR = %d, MAX_BLOCK_ENTRIES = %d, MAX_DEPTH = %d\n", GLOBAL_DEPT, RECORDS_NUM, MAX_NUM_BLOCK_EVR, (int)floor(MAX_BLOCK_ENTRIES), MAX_DEPTH);
    CALL_OR_DIE(HT_Init());
    CALL_OR_DIE(SHT_Init());
    int indexDesc1, indexDesc2,indexDesc3,indexDesc4;
    CALL_OR_DIE(HT_CreateIndex(FILE_NAME2, GLOBAL_DEPT));
    CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME4, "city", 20, GLOBAL_DEPT, FILE_NAME2));
    CALL_OR_DIE(HT_CreateIndex(FILE_NAME1, GLOBAL_DEPT));
    CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME3, "city", 20, GLOBAL_DEPT, FILE_NAME1));

    printf("Creates complete\n");
    CALL_OR_DIE(HT_OpenIndex(FILE_NAME2, &indexDesc2));
    CALL_OR_DIE(HT_CloseFile(indexDesc2));
    CALL_OR_DIE(HT_OpenIndex(FILE_NAME2, &indexDesc2));
    CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME4, &indexDesc3));
    CALL_OR_DIE(HT_OpenIndex(FILE_NAME1, &indexDesc1));
    CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME3, &indexDesc4));
    printf(("Initiating...\n"));
    printf("%d %d\n",indexDesc2,indexDesc3);
    Record record;
    SecondaryRecord Srecord;
    srand(12345678);
    int r;
    printf("Inserting Entries...\n");

    for (int id = 0; id < RECORDS_NUM; ++id)
    {
        // create a record
        record.id = id;
        r = rand() % 14;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 14;
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 14;
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        strcpy(Srecord.index_key, record.city);
        CALL_OR_DIE(HT_InsertEntry(indexDesc2, record,&HELLO[id]));
        Srecord.tupleId = HELLO[id];
        CALL_OR_DIE(SHT_SecondaryInsertEntry(indexDesc3,Srecord))
        if( id == 19){
            CALL_OR_DIE(SHT_PrintAllEntries(indexDesc3, cities[1]));
        }
    }
    for (int id = 0; id < RECORDS_NUM; ++id)
    {
        // create a record
        record.id = id;
        r = rand() % 14;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 14;
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 14;
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        strcpy(Srecord.index_key, record.city);
        CALL_OR_DIE(HT_InsertEntry(indexDesc1, record, &HELLO[id]));
        Srecord.tupleId = HELLO[id];
        CALL_OR_DIE(SHT_SecondaryInsertEntry(indexDesc4, Srecord))
        if (id == 19)
        {
            CALL_OR_DIE(SHT_PrintAllEntries(indexDesc4, cities[1]));
        }
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
    printf("Dad\n");
for(int i=0; i<14; i++){
    CALL_OR_DIE(SHT_PrintAllEntries(indexDesc3, cities[i]));
}
    printf("Mom\n");
    CALL_OR_DIE(SHT_InnerJoin(indexDesc3,indexDesc4,"Munich");)
    printf("Uncle\n");

    CALL_OR_DIE(HT_CloseFile(indexDesc2));
    //CALL_OR_DIE(HT_CloseFile(indexDesc1));
    //CALL_OR_DIE(HashStatistics(FILE_NAME1));
    CALL_OR_DIE(HT_CloseFile(indexDesc1));
    //CALL_OR_DIE(SHT_CloseSecondaryIndex(indexDesc3));
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
