#ifndef DEADLOCK_H
#define DEADLOCK_H

#include "innie.h"

/* Returns 1 if deadlock detected and resolved, 0 otherwise */
int detect_deadlocks(Innie *innies, int count);

#endif
