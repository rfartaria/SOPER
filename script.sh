#!/bin/bash

if diff <(sort $1) <(sort $2) &> /dev/null; then echo EQUAL; else echo DIFFERENT; fi

