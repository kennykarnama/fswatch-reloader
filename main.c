#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<yaml.h>
#include <libfswatch/c/libfswatch.h>

#include "arraystring.h" // automically resolve in local folder
#include "reloader.h"

void fsw_callback(fsw_cevent const *const events, const unsigned int event_num, void *data) {
    // cast to reloader_t
    struct reloader_t *reloader = (struct reloader_t*)data;
    if (*events->flags > PlatformSpecific && *events->flags <= MovedTo) {
        printf("got event: %s\n", fsw_get_event_flag_name(*events->flags));
        printf("scripts...\n");
        for (size_t i = 0; i < reloader->scripts->used; i++) {
            printf("value=%s\n", reloader->scripts->array[i]);
        }
    }

    return;
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("usage: fswatch-reloader <spec.yaml> <watched dir>");
    }
    
    FILE *file;

    yaml_parser_t parser;
    yaml_event_t event;

    fflush(stdout);

    file = fopen(argv[1], "rb");

    if (file == NULL) {
        printf("error opening file %s errno=%d errno_str=%s\n", argv[1], errno, strerror(errno));
        return -1;
    }

    int yaml_initialize_status = yaml_parser_initialize(&parser);

    if (yaml_initialize_status != 1) {
        printf("error initialize yaml_parser. code=%d problem=%s\n", parser.error, parser.problem);
        return -1;
    }

    yaml_parser_set_input_file(&parser, file);

    int done = 0;
    int count = 0;

    int pos = 0;
    
    int map_on = 0;

    struct reloader_t reloader;
    ArrayString initial;
    initArray(&initial, 10);

    reloader.scripts = &initial;

    while (!done) {
        int parse_status = yaml_parser_parse(&parser, &event);
        if (parse_status != 1) {
            printf("error yaml_parser.parse code=%d problem=%s\n", parser.error, parser.problem);
            return -1;
        }

        done = (event.type == YAML_STREAM_END_EVENT);
        printf("got event=%d\n", event.type);
        
        if (event.type == YAML_MAPPING_START_EVENT) {
            map_on = 1;
        }else if (event.type == YAML_MAPPING_END_EVENT) {
            map_on = 0;
            pos++;
        }else if (event.type == YAML_SCALAR_EVENT) {
            if (map_on && pos == 0) { // watchdir
                reloader.watch_dir = malloc(strlen(event.data.scalar.value) + 1); 
                strcpy(reloader.watch_dir, event.data.scalar.value);
            }else if (map_on && pos == 1 && strcmp(event.data.scalar.value, "scripts") != 0) {
               char *copy = malloc(strlen(event.data.scalar.value) + 1);
               strcpy(copy, event.data.scalar.value);
               insertArray(reloader.scripts, copy);
            }
            printf("scalar value=%s\n", event.data.scalar.value);
        }

        yaml_event_delete(&event);

        count ++;
    }

    yaml_parser_delete(&parser);

    fclose(file);

    printf("watchDir=%s\n", reloader.watch_dir);
    
    printf("set watchdir to %s\n", reloader.watch_dir);

    int watch_status;
    // trigger fswatch
    fsw_init_library();
    const FSW_HANDLE handle = fsw_init_session(system_default_monitor_type);
    watch_status = fsw_add_path(handle, reloader.watch_dir);
    if (watch_status != FSW_OK) {
        printf("error fsw.add_path watch_dir=%s error_code=%d", reloader.watch_dir, watch_status);
        return -1;
    }
    watch_status = fsw_set_callback(handle, fsw_callback, &reloader);
    if (watch_status != FSW_OK) {
        printf("error fsw.set_callback watch_dir=%s error_code=%d", reloader.watch_dir, watch_status);
        return -1;
    }
    fsw_set_verbose(1);
    
    watch_status = fsw_start_monitor(handle); // blocking call. TODO: start in different thread so we can gracefully shutdown if a SIGINT emitted. 
    if (watch_status != FSW_OK) {
        printf("error fsw.set_callback watch_dir=%s error_code=%d", reloader.watch_dir, watch_status);
        return -1;
    }

    free(reloader.watch_dir);
    freeArray(reloader.scripts);

    return 0;
}
