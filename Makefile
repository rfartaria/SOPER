SHELL=/bin/bash

OBJ_DIR=obj
SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
TESTS_DIR=tests

CC=gcc
CFLAGS = -O0 -g -fPIC -Wall -I$(INC_DIR)

OBJECTS=$(addprefix $(OBJ_DIR)/, broker.o control.o exchange.o file.o investor.o main.o memory.o prodcons.o scheduler.o time.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(BIN_DIR)/socurrency

$(BIN_DIR)/socurrency: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(OBJ_DIR)/so.o -lrt -lpthread -o $(BIN_DIR)/socurrency

$(OBJ_DIR)/broker.o: $(SRC_DIR)/broker.c $(addprefix $(INC_DIR)/, main.h memory.h broker.h)
$(OBJ_DIR)/control.o: $(SRC_DIR)/control.c $(addprefix $(INC_DIR)/, main.h so.h memory.h prodcons.h control.h)
$(OBJ_DIR)/exchange.o: $(SRC_DIR)/exchange.c $(addprefix $(INC_DIR)/, exchange.h main.h memory.h sotime.h)
$(OBJ_DIR)/file.o: $(SRC_DIR)/file.c $(addprefix $(INC_DIR)/, main.h so.h memory.h scheduler.h file.h prodcons.h sotime.h)
$(OBJ_DIR)/investor.o: $(SRC_DIR)/investor.c $(addprefix $(INC_DIR)/, main.h memory.h sotime.h investor.h file.h)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(addprefix $(INC_DIR)/, main.h investor.h memory.h prodcons.h control.h file.h sotime.h so.h exchange.h broker.h)
$(OBJ_DIR)/memory.o: $(SRC_DIR)/memory.c $(addprefix $(INC_DIR)/, main.h so.h memory.h prodcons.h control.h scheduler.h file.h sotime.h)
$(OBJ_DIR)/prodcons.o: $(SRC_DIR)/prodcons.c $(addprefix $(INC_DIR)/, main.h so.h control.h prodcons.h)
$(OBJ_DIR)/scheduler.o: $(SRC_DIR)/scheduler.c $(addprefix $(INC_DIR)/, main.h so.h scheduler.h)
$(OBJ_DIR)/time.o: $(SRC_DIR)/time.c $(addprefix $(INC_DIR)/, main.h so.h sotime.h)

clean: clean_shm
	rm -vf $(BIN_DIR)/*
	rm -vf $(OBJECTS)
	rm -vf $(TESTS_DIR)/out/*
	rm -vf $(TESTS_DIR)/log/*

clean_shm:
	rm -vf /dev/shm/*_$$(id -u)

all_scenarios: scenario1 scenario2 scenario3 scenario4

test_dirs:
	if test ! -d $(TESTS_DIR)/out; then mkdir -v $(TESTS_DIR)/out; fi
	if test ! -d $(TESTS_DIR)/log; then mkdir -v $(TESTS_DIR)/log; fi

scenario%:
	./bin/socurrency $(TESTS_DIR)/in/$@ $(TESTS_DIR)/out/$@ -l $(TESTS_DIR)/log/$@.log -t 1000

scenario1: test_dirs $(BIN_DIR)/socurrency
scenario2: test_dirs $(BIN_DIR)/socurrency
scenario3: test_dirs $(BIN_DIR)/socurrency
scenario4: test_dirs $(BIN_DIR)/socurrency

base_test: scenario1
	@echo
	@echo Test lines out of order
	@bash script.sh tests/out/scenario1 <(awk 'BEGIN{i=0}{l[i++]=$$0}END{for (j=0;j<i;j++) print l[i-j-1]}' tests/out/scenario1)
	@echo
	@echo Test changes in file
	@bash script.sh tests/out/scenario1 <(sed 's/001/0010/' tests/out/scenario1)

# these tests require that the provided executable is moved into the project main directory and renamed socurrency.ref
test_scenario%:
	rm -vf $(TESTS_DIR)/out/$(subst test_,,$@).ref $(TESTS_DIR)/out/$(subst test_,,$@)
	./socurrency.ref $(TESTS_DIR)/in/$(subst test_,,$@) $(TESTS_DIR)/out/$(subst test_,,$@).ref -l $(TESTS_DIR)/log/$(subst test_,,$@).log.ref -t 1000
	make clean_shm
	$(BIN_DIR)/socurrency $(TESTS_DIR)/in/$(subst test_,,$@) $(TESTS_DIR)/out/$(subst test_,,$@) -l $(TESTS_DIR)/log/$(subst test_,,$@).log -t 1000
	bash script.sh $(TESTS_DIR)/out/$(subst test_,,$@) $(TESTS_DIR)/out/$(subst test_,,$@).ref

test_scenario1: test_dirs $(BIN_DIR)/socurrency
test_scenario2: test_dirs $(BIN_DIR)/socurrency
test_scenario3: test_dirs $(BIN_DIR)/socurrency
test_scenario4: test_dirs $(BIN_DIR)/socurrency

