#ifndef HASH_FILE_H
#define HASH_FILE_H
#include "bf.h"
#define MAX_DEPTH 8
#define MAX_NUM_BLOCK_EVR 4
#define MAX_BLOCK_ENTRIES (BF_BLOCK_SIZE - 2 * sizeof(int)) / sizeof(Record)

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

typedef struct HT_Index
{
    int globaldepth;
    int free;
    int anagn_arx;
    char onoma_arxeiou[30];
} HT_Index;

typedef struct Buckets_Ind
{
    char filename[30];
    char protevon_filename[30];
    int protevon;
    int startingglobaldepth;
    char attrName[10];
    int attrLength;
    struct Buckets_Ind *prev;
    struct Buckets_Ind *next;
} Buckets_Ind;

/*
* Η συνάρτηση HT_Init χρησιμοποιείται για την αρχικοποίηση κάποιον δομών που μπορεί να χρειαστείτε. 
* Σε περίπτωση που εκτελεστεί επιτυχώς, επιστρέφεται HT_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
*/
HT_ErrorCode
HT_Init();

/*
* Η συνάρτηση HT_CreateIndex χρησιμοποιείται για τη δημιουργία και κατάλληλη αρχικοποίηση ενός άδειου αρχείου κατακερματισμού με όνομα fileName. 
* Στην περίπτωση που το αρχείο υπάρχει ήδη, τότε επιστρέφεται ένας κωδικός λάθους. 
* Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HΤ_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
*/
HT_ErrorCode HT_CreateIndex(
    const char *fileName, /* όνομααρχείου */
    int depth);

/*
* Η ρουτίνα αυτή ανοίγει το αρχείο με όνομα fileName. 
* Εάν το αρχείο ανοιχτεί κανονικά, η ρουτίνα επιστρέφει HT_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
*/
HT_ErrorCode HT_OpenIndex(
    const char *fileName, /* όνομα αρχείου */
    int *indexDesc        /* θέση στον πίνακα με τα ανοιχτά αρχεία  που επιστρέφεται */
);

/*
* Η ρουτίνα αυτή κλείνει το αρχείο του οποίου οι πληροφορίες βρίσκονται στην θέση indexDesc του πίνακα ανοιχτών αρχείων.
* Επίσης σβήνει την καταχώρηση που αντιστοιχεί στο αρχείο αυτό στον πίνακα ανοιχτών αρχείων. 
* Η συνάρτηση επιστρέφει ΗΤ_OK εάν το αρχείο κλείσει επιτυχώς, ενώ σε διαφορετική σε περίπτωση κωδικός λάθους.
*/
HT_ErrorCode HT_CloseFile(
    int indexDesc /* θέση στον πίνακα με τα ανοιχτά αρχεία */
);

/*
* Η συνάρτηση HT_InsertEntry χρησιμοποιείται για την εισαγωγή μίας εγγραφής στο αρχείο κατακερματισμού. 
* Οι πληροφορίες που αφορούν το αρχείο βρίσκονται στον πίνακα ανοιχτών αρχείων, ενώ η εγγραφή προς εισαγωγή προσδιορίζεται από τη δομή record. 
* Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HT_OK, ενώ σε διαφορετική περίπτωση κάποιος κωδικός λάθους.
*/
HT_ErrorCode HT_InsertEntry(
    int indexDesc, /* θέση στον πίνακα με τα ανοιχτά αρχεία */
    Record record,  /* δομή που προσδιορίζει την εγγραφή */
    int *tuppleid
);

/*
* Η συνάρτηση HΤ_PrintAllEntries χρησιμοποιείται για την εκτύπωση όλων των εγγραφών που το record.id έχει τιμή id. 
* Αν το id είναι NULL τότε θα εκτυπώνει όλες τις εγγραφές του αρχείου κατακερματισμού. 
* Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HP_OK, ενώ σε διαφορετική περίπτωση κάποιος κωδικός λάθους.
*/
HT_ErrorCode HT_PrintAllEntries(
    int indexDesc, /* θέση στον πίνακα με τα ανοιχτά αρχεία */
    int *id        /* τιμή του πεδίου κλειδιού προς αναζήτηση */
);

HT_ErrorCode HashStatistics(
    char *filename /* όνομα του αρχείου που ενδιαφέρει */
);

int hash_funct(int x);

#endif // HASH_FILE_H