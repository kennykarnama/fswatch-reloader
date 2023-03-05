#ifndef RELOADER_H
#define RELOADER_H

struct reloader_t {
    char *watch_dir;
    ArrayString *scripts;
};

#endif