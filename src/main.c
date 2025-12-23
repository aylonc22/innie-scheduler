#include <stdio.h>
#include "innie.h"

int main() {
    Innie innie1;
    init_innie(&innie1, 1);

    pthread_create(&innie1.thread, NULL, worker_routine, &innie1);
    pthread_join(innie1.thread, NULL);

    return 0;
}
