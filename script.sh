#!/bin/bash

if diff <(sort $1) <(sort $2) &> /dev/null; then echo IGUAIS; else echo DIFERENTES; fi

