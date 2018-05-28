ICM_ROOT_DIR := $(shell cd ..; pwd)
ICM_INCLUDE_DIR := ${ICM_ROOT_DIR}/include
ICM_SRC_DIR := ${ICM_ROOT_DIR}/src
ICM_CONF_DIR := ${ICM_ROOT_DIR}/conf
ICM_OBJ_DIR := ${ICM_ROOT_DIR}/obj
ICM_LIB_DIR := ${ICM_ROOT_DIR}/lib
ICM_LIBS := -licmoslib -licmlib -licmoslib

CROSS_CC := gcc
CROSS_AR := ar

CFLAGS := -I${ICM_INCLUDE_DIR} -I${ICM_SRC_DIR}

.PHONY: all
all: fp_test task_test

.PHONY: fp_test
fp_test: 
	$(CROSS_CC) -L ${ICM_LIB_DIR} ${CFLAGS} -o fp_test  fp_test.c  ${ICM_LIBS} -lpthread

.PHONY: task_test
task_test: 
	$(CROSS_CC) -L ${ICM_LIB_DIR} ${CFLAGS} -o task_test  task_test.c  ${ICM_LIBS} -lpthread

.PHONY: clean
clean:
	rm -f fp_test task_test
