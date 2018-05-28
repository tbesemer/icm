ICM_ROOT_DIR := $(shell cd ../../; pwd)
ICM_INCLUDE_DIR := ${ICM_ROOT_DIR}/include
ICM_SRC_DIR := ${ICM_ROOT_DIR}/src
ICM_OBJ_DIR := ${ICM_ROOT_DIR}/obj
ICM_LIB_DIR := ${ICM_ROOT_DIR}/lib

CROSS_CC := gcc
CROSS_AR := ar
CROSS_RANLIB := ranlib

CFLAGS := -I${ICM_INCLUDE_DIR} -I${ICM_SRC_DIR} -Wall

.PHONY: all
all:	${ICM_LIB_DIR}/libicmoslib.a

${ICM_LIB_DIR}/libicmoslib.a: icm_osal.c ${ICM_SRC_DIR}/icm_core.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	${CROSS_CC} -c -o ${ICM_OBJ_DIR}/icm_osal.o icm_osal.c ${CFLAGS}
	${CROSS_AR} -rvs ${ICM_LIB_DIR}/libicmoslib.a ${ICM_OBJ_DIR}/icm_osal.o
	${CROSS_RANLIB} ${ICM_LIB_DIR}/libicmoslib.a 

.PHONY: clean
clean:
	rm -f ${ICM_OBJ_DIR}/icm_osal.o ${ICM_LIB_DIR}/libicmoslib.a
