#!/bin/bash

clang++ -o a.out source/main.cpp -std=c++17 `curl-config --libs`
