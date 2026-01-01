#include <stdio.h>
#include <stdbool.h>
#include "innie.h"

/* DFS helper */
static bool dfs(Innie *innie, bool *visited, bool *rec_stack, int count) {
    int idx = innie->id;
    if (!visited[idx]) {
        visited[idx] = true;
        rec_stack[idx] = true;

        for (int i = 0; i < innie->waiting_count; i++) {
            Innie *dep = innie->waiting_for[i];
            int dep_idx = dep->id;

            if (!visited[dep_idx] && dfs(dep, visited, rec_stack, count))
                return true;
            else if (rec_stack[dep_idx])
                return true;
        }
    }
    rec_stack[idx] = false;
    return false;
}

/* Detect cycles and mark innies as DEADLOCKED */
void detect_deadlocks(Innie *innies, int count) {
    bool visited[count];
    bool rec_stack[count];

    for (int i = 0; i < count; i++) {
        visited[i] = false;
        rec_stack[i] = false;
    }

    for (int i = 0; i < count; i++) {
        if (dfs(&innies[i], visited, rec_stack, count)) {
            // mark all innies in rec_stack as DEADLOCKED
            for (int j = 0; j < count; j++) {
                if (rec_stack[j] && innies[j].state == INNIE_WAITING) {
                    innies[j].state = INNIE_DEADLOCKED;
                    innies[j].work_value = -1;
                    printf("[DEADLOCK] %s terminated due to circular dependency\n", innies[j].name);
                }
            }
        }
    }
}
