#ifndef HASH_FILE_H1
#define HASH_FILE_H1

#include "hash_file.h"

#define DEFTEREVON_EVR_MAX_DEPTH 4

typedef struct{
char index_key[20];
int tupleId;  /*Ακέραιος που προσδιορίζει το block και τη θέση μέσα στο block στην οποία έγινε η εισαγωγή της εγγραφής στο πρωτεύον ευρετήριο.*/ 
}SecondaryRecord;

typedef struct {  //μπορειτε να αλλαξετε τη δομη συμφωνα  με τις ανάγκες σας
	char surname[20];
	char city[20];
    char name[20];
	int oldTupleId; // η παλια θέση της εγγραφής πριν την εισαγωγή της νέας
	int newTupleId; // η νέα θέση της εγγραφής που μετακινήθηκε μετα την εισαγωγή της νέας εγγραφής 
	
} UpdateRecordArray;

typedef struct SHT_Index
{
    int globaldepth;
    int free;
    int anagn_arx;
    char onoma_protevontos_arxeiou[20];
    char attrName[10];
    int attrLength;
} SHT_Index;

HT_ErrorCode SHT_Init();

HT_ErrorCode SHT_CreateSecondaryIndex(
const char *sfileName, /* όνομα αρχείου */
char *attrName, /* όνομα πεδίου-κλειδιού */
int attrLength, /* μήκος πεδίου-κλειδιού */
int depth,/* το ολικό βάθος ευρετηρίου επεκτατού κατακερματισμού */
char *fileName /* όνομα αρχείου πρωτεύοντος ευρετηρίου*/);

HT_ErrorCode SHT_OpenSecondaryIndex(
const char *sfileName, /* όνομα αρχείου */
int *indexDesc /* θέση στον πίνακα με τα ανοιχτά αρχεία που επιστρέφεται */ );

HT_ErrorCode SHT_CloseSecondaryIndex(int indexDesc /* θέση στον πίνακα με τα ανοιχτά αρχεία */ );

HT_ErrorCode SHT_SecondaryInsertEntry (
int indexDesc, /* θέση στον πίνακα με τα ανοιχτά αρχεία */
SecondaryRecord record /* δομή που προσδιορίζει την εγγραφή */ );

HT_ErrorCode SHT_SecondaryUpdateEntry (
int indexDesc, /* θέση στον πίνακα με τα ανοιχτά αρχεία */
UpdateRecordArray *updateArray /* δομή που προσδιορίζει την παλιά εγγραφή */);

HT_ErrorCode SHT_PrintAllEntries(
int sindexDesc, /* θέση στον πίνακα με τα ανοιχτά αρχεία  του αρχείου δευτερεύοντος ευρετηρίου */
char *index_key /* τιμή του πεδίου-κλειδιού προς αναζήτηση */);

HT_ErrorCode SHT_HashStatistics(char *filename /* όνομα του αρχείου που ενδιαφέρει */);

HT_ErrorCode SHT_InnerJoin(
int sindexDesc1, /* θέση στον πίνακα με τα ανοιχτά αρχεία  του αρχείου δευτερεύοντος ευρετηρίου για το πρώτο αρχείο εισόδου */
int sindexDesc2, /* θέση στον πίνακα με τα ανοιχτά αρχεία του αρχείου δευτερεύοντος ευρετηρίου για το δεύτερο αρχείο εισόδου */
char *index_key  /* το κλειδι πανω στο οποιο θα γινει το join. Αν  NULL τοτε επιστρέφεί όλες τις πλειάδες*/);

int hash_funct1(const char *s, const char *r);
unsigned long hash_funct2(
    const char *s);
#endif // HASH_FILE_H
