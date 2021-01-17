#ifndef HT_H
#define HT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Record.h"
#include "HT_info.h"
#include "BF.h"


int HT_CreateIndex(char *fileName, /* όνομα αρχείου */char attrType, /* τύπος πεδίου-κλειδιού: 'c', 'i' */char* attrName, /* όνομα πεδίου-κλειδιού */int attrLength, /* μήκος πεδίου-κλειδιού */int buckets /* αριθμός κάδων κατακερματισμού*/);
HT_info* HT_OpenIndex(char *fileName /* όνομα αρχείου */ );
int HT_CloseIndex( HT_info* header_info );
int HT_InsertEntry( HT_info header_info, /* επικεφαλίδα του αρχείου*/ Record record /* δομή πουπροσδιορίζει την εγγραφή */ );
int HT_DeleteEntry( HT_info header_info, /* επικεφαλίδα του αρχείου*/ void *value /* τιμή τουπεδίου-κλειδιού προς διαγραφή */);
int HT_GetAllEntries( HT_info header_info, /* επικεφαλίδα του αρχείου */ void *value /* τιμή τουπεδίου-κλειδιού προς αναζήτηση */);

long HT_HashFunction(int key, long buckets);

#endif