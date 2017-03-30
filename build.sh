#!/bin/bash

mkdir -p Build
cd Build
cmake -G Ninja ..
ninja