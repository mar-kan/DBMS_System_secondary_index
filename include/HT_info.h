#ifndef HT_INFO_H
#define HT_INFO_H

typedef struct
{
    int fileDesc; /*αναγνωριστικός αριθμός ανοίγματος αρχείου από το επίπεδο block */
    char attrType; /*οτύποςτουπεδίουπουείναικλειδίγιατοσυγκεκριμένοαρχείο,'c'ή'i'*/
    char* attrName; /*τοόνοματουπεδίουπουείναικλειδίγιατοσυγκεκριμένοαρχείο*/
    int attrLength; /* το μέγεθος του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο */
    long int numBuckets; /* το πλήθος των “κάδων” του αρχείου κατακερματισμού */
}
HT_info;

#endif