#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>

#include "innie.h"
#include "registery.h"
#include "shift_stack.h"
#include "load_innies.h"
#include "instruction.h"
#include "execute.h"
#include "deadlock.h"

Registry registry;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s schedule.json\n", argv[0]);
        return 1;
    }

    const char *json_path = argv[1];

    registry_init(&registry);

    Innie *innies = NULL;
    int innie_count = 0;

    /* ---------- Phase 1: load innies (IDs + raw schedules) ---------- */
    load_innies_from_json(json_path, &innies, &innie_count);
    printf("Loaded %d innies from %s\n", innie_count, json_path);

    /* ---------- Allocate waiting_for arrays ---------- */
    for (int i = 0; i < innie_count; i++) {
        innies[i].waiting_for = malloc(sizeof(Innie*) * innie_count);
        innies[i].waiting_count = 0;
    }

    /* ---------- Phase 2: parse instructions (with full context) ---------- */
    for (int i = 0; i < innie_count; i++) {
        innies[i].instructions =
            parse_schedule(
                innies[i].schedule_src,
                &innies[i].instructions_count,
                innies,
                innie_count
            );

        free(innies[i].schedule_src);
        innies[i].schedule_src = NULL;
    }

    /* ---------- Phase 3: spawn threads ---------- */
    for (int i = 0; i < innie_count; i++) {
        pthread_create(
            &innies[i].thread,
            NULL,
            innie_worker,
            &innies[i]
        );
    }

    /* ---------- Phase 4: monitor & detect deadlocks ---------- */
    int all_done = 0;
    while (!all_done) {
        all_done = 1;
        for (int i = 0; i < innie_count; i++) {
            if (innies[i].state != INNIE_FINISHED &&
                innies[i].state != INNIE_DEADLOCKED) {
                all_done = 0;
                break;
            }
        }

        detect_deadlocks(innies, innie_count);
        sched_yield(); // let threads run
    }

    /* ---------- Phase 5: join & cleanup ---------- */
    for (int i = 0; i < innie_count; i++) {
        pthread_join(innies[i].thread, NULL);
        shift_stack_destroy(innies[i].shifts);
        free(innies[i].instructions);
        free(innies[i].waiting_for);
    }

    printf("All innies have completed their shifts.\n");
    for (int i = 0; i < innie_count; i++) {
        printf("Innie %s finished with work_value = %d\n",
            innies[i].name, innies[i].work_value);
    }

    free(innies);
    return 0;
}
