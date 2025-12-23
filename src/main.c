#include <stdio.h>
#include <stdlib.h>
#include "../include/innie.h"
#include "../include/registery.h"
#include "../include/shift_stack.h"

Registry registry;

int main(void) {
    registry_init(&registry);

    const int N = 4;
    Innie innies[N];

    for (int i = 0; i < N; i++) {
        innies[i].id = i;
        innies[i].pc = 0;
        innies[i].work_value = 0;
        innies[i].shifts = shift_stack_create();

        pthread_create(&innies[i].thread, NULL, innie_worker, &innies[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(innies[i].thread, NULL);
        shift_stack_destroy(innies[i].shifts);
    }

    printf("All innies have completed their shifts.\n");
    return 0;
}
