#!/bin/bash

gcc -Wall -I. -lpez test/test_all.c -o test/test_all && test/test_all
