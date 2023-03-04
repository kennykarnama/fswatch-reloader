#include<stdio.h>
#include<stdlib.h>
#include<yaml.h>
#include<errno.h>

typedef struct {
  char **array;
  size_t used;
  size_t size;
} ArrayString;

void initArray(ArrayString *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(char*));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(ArrayString *a, char *element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(char*));
  }
  a->array[a->used++] = element;
}

void freeArray(ArrayString *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

struct reloader_t {
    char *watch_dir;
    ArrayString *scripts;
};



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
    printf("scripts...\n");
    for (size_t i = 0; i < reloader.scripts->used; i++) {
        printf("value=%s\n", reloader.scripts->array[i]);
    }

    free(reloader.watch_dir);
    freeArray(reloader.scripts);

    return 0;
}
