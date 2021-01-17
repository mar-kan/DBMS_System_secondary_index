#ifndef SHT_INFO_H
#define SHT_INFO_H


typedef struct
{
    int fileDesc; /* αναγνωριστικόςαριθμόςανοίγματοςαρχείουαπότοεπίπεδο block */
    char* attrName; /* τοόνοματουπεδίουπουείναικλειδίγιατοσυγκεκριμένοαρχείο */
    int attrLength; /* το μέγεθοςτουπεδίουπουείναικλειδίγιατοσυγκεκριμένοαρχείο */
    long int numBuckets; /* τοπλήθοςτων “κάδων” τουαρχείουκατακερματισμού */
    char *fileName; /* όνομααρχείου μετοπρωτεύονευρετήριοστο id */

} SHT_info;


#endif
