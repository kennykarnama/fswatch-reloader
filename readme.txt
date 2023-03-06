## description

simple fswatch-reloader

## how to compile ?

dont forget to run `sudo ldconfig` in order to refresh sharedlib

gcc main.c -o fswatch-reloader -lyaml -lfswatch -lexplain

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
- libfswatch (1.17.1)
- libexplain