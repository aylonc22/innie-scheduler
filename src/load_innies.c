#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "innie.h"
#include "instruction.h"
#include "shift_stack.h"

void load_innies_from_json(const char *filename, Innie **out_innies, int *out_count){
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); exit(1); }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(data);
    if (!root) {
        fprintf(stderr, "JSON parse error\n");
        exit(1);
    }

    cJSON *innies_json = cJSON_GetObjectItem(root, "innies");
    int n = cJSON_GetArraySize(innies_json);

    Innie *innies = calloc(n, sizeof(Innie));

    for (int i = 0; i < n; i++) {
        cJSON *item = cJSON_GetArrayItem(innies_json, i);

        cJSON *id = cJSON_GetObjectItem(item, "id");
        cJSON *schedule = cJSON_GetObjectItem(item, "schedule");

        if (!cJSON_IsString(id) || !cJSON_IsString(schedule)) {
            fprintf(stderr, "Invalid innie JSON\n");
            exit(1);
        }

        innies[i].id = i;
        snprintf(innies[i].name, sizeof innies[i].name, "%s", id->valuestring);

        innies[i].pc = 0;
        innies[i].work_value = 0;
        innies[i].shifts = shift_stack_create();
        innies[i].waffled = 0;

        innies[i].schedule_src = strdup(schedule->valuestring);
    }

    free(data);
    cJSON_Delete(root);

    *out_innies = innies;
    *out_count = n;
}
