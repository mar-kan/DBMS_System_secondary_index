#ifndef SHT_H
#define SHT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Record.h"
#include "HT_info.h"
#include "SecondaryRecord.h"
#include "SHT_info.h"
#include "BF.h"


int SHT_CreateSecondaryIndex(char *sfileName, /* όνομααρχείου */char* attrName, /* όνομαπεδίου-κλειδιού */int attrLength, /* μήκοςπεδίου-κλειδιού */int buckets, /* αριθμόςκάδωνκατακερματισμού*/char* fileName /* όνομααρχείουπρωτεύοντοςευρετηρίου*/);
SHT_info* SHT_OpenSecondaryIndex(char *sfileName /* όνομα αρχείου */ );
int SHT_CloseSecondaryIndex( SHT_info* header_info );
int SHT_SecondaryInsertEntry( SHT_info header_info, /* επικεφαλίδα του αρχείου*/ SecondaryRecord record /* δομή πουπροσδιορίζει την εγγραφή */ );
int SHT_SecondaryGetAllEntries( SHT_info header_info_sht, /* επικεφαλίδα του αρχείου δευτερεύοντος ευρετηρίου*/ HT_info header_info_ht,/*επικεφαλίδατουαρχείουπρωτεύοντοςευρετηρίου*/ void *value /* τιμή τουπεδίου-κλειδιού προς αναζήτηση */);
int HashStatistics( char* filename /* όνομα του αρχείου που ενδιαφέρει */ );

long SHT_HashFunction(int key, long buckets);


#endif
