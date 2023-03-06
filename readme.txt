## description

simple fswatch-reloader

## how to compile ?

dont forget to run `sudo ldconfig` in order to refresh sharedlib

gcc main.c -o fswatch-reloader -lyaml -lfswatch -lexplain

## How to use

fswatch-reloader <your yaml def>

## yaml def

-
 watch_dir: <dir>
-
 scripts:
   - ls <sequence of scripts to be executed>
   - whoami
-

## deps

- libyaml
- libfswatch (1.17.1)
- libexplain