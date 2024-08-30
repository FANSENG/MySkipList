#!/bin/bash

g++ pressureTest.cpp -o pressureTest  --std=c++17 -pthread

./pressureTest

rm pressureTest