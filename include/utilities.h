#include "Record.h"
#include <stdlib.h>
#include <string.h>
#include "HT_info.h"
#include "BF.h"

#ifndef UTILITIES_H
#define UTILITIES_H

int createRecordFromLine(char *, Record*);

int printHT(HT_info info);
#endif
