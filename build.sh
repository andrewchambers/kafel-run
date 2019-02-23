#! /bin/sh
set -eux

gcc -O2 -Wall -o kafel-run main.c $(pkg-config --libs --cflags kafel)
