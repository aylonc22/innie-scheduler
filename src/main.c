#include <stdio.h>
#include <stdlib.h>

#include "innie.h"
#include "registery.h"
#include "shift_stack.h"
#include "load_innies.h"

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

    load_innies_from_json(json_path, &innies, &innie_count);

    printf("Loaded %d innies from %s\n", innie_count, json_path);

    /* Spawn threads */
    for (int i = 0; i < innie_count; i++) {
        pthread_create(
            &innies[i].thread,
            NULL,
            innie_worker,
            &innies[i]
        );
    }

    /* Join threads */
    for (int i = 0; i < innie_count; i++) {
        pthread_join(innies[i].thread, NULL);
        shift_stack_destroy(innies[i].shifts);
    }

    free(innies);

    printf("All innies have completed their shifts.\n");
    return 0;
}
