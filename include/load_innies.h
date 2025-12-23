#ifndef LOAD_INNIES_H
#define LOAD_INNIES_H

#include "./innie.h"

void load_innies_from_json(const char *filename, Innie **out_innies, int *out_count);

#endif