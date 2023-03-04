## description

simple fswatch-reloader

## how to compile ?

gcc main.c -o fswatch-reloader -lyaml

## yaml def

-
 watch_dir: .
-
 scripts:
   - ls
   - whoami
-

## deps

- libyaml