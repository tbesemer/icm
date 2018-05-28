ICM_ROOT_DIR := $(shell cd ../../; pwd)
ICM_OSAL_DIR := $(shell pwd)
FREE_RTOS_BASE := ${ICM_OSAL_DIR}/FreeRTOS_x86
ICM_INCLUDE_DIR := ${ICM_ROOT_DIR}/include
ICM_SRC_DIR := ${ICM_ROOT_DIR}/src
ICM_OBJ_DIR := ${ICM_ROOT_DIR}/obj
ICM_LIB_DIR := ${ICM_ROOT_DIR}/lib
FREE_RTOS_ROOT := ${ICM_OSAL_DIR}/FreeRTOS-Sim
FREE_RTOS_KERNEL := ${FREE_RTOS_ROOT}/Source
FREE_RTOS_ROOT_OBJ := ${ICM_OSAL_DIR}/FreeRTOS_x86/obj/freertosx86.o

CROSS_CC ?= gcc
CROSS_AR ?= ar
CROSS_RANLIB ?= ranlib

FREERTOS_INCLUDE_PATH ?= -I${FREE_RTOS_ROOT} -I${FREE_RTOS_KERNEL}/include -I${FREE_RTOS_KERNEL}/portable/GCC/POSIX -I${FREE_RTOS_BASE}

CFLAGS ?= -D__GCC_POSIX__ -I${ICM_INCLUDE_DIR} -I${ICM_SRC_DIR} ${FREERTOS_INCLUDE_PATH} -Wall

.PHONY: all
all:	icmosal_x86

.PHONY: icmosal_x86
icmosal_x86: freertos_x86_base ${ICM_LIB_DIR}/libicmoslib.a

.PHONY: icmosal_cross
icmosal_cross: 
	${CROSS_CC} -c -o ${ICM_OBJ_DIR}/icm_osal.o icm_osal.c ${CFLAGS}
	${CROSS_AR} -rvs ${ICM_LIB_DIR}/libicmoslib.a ${ICM_OBJ_DIR}/icm_osal.o 
	${CROSS_RANLIB} ${ICM_LIB_DIR}/libicmoslib.a 

${ICM_LIB_DIR}/libicmoslib.a: icm_osal.c ${ICM_SRC_DIR}/icm_core.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	${CROSS_CC} -c -o ${ICM_OBJ_DIR}/icm_osal.o icm_osal.c ${CFLAGS}
	${CROSS_AR} -rvs ${ICM_LIB_DIR}/libicmoslib.a ${ICM_OBJ_DIR}/icm_osal.o ${FREE_RTOS_ROOT_OBJ}
	${CROSS_RANLIB} ${ICM_LIB_DIR}/libicmoslib.a 

.PHONY: freertos_x86_base
freertos_x86_base:
	make -C ${FREE_RTOS_BASE} 

.PHONY: clean
clean:
	rm -f ${ICM_OBJ_DIR}/icm_osal.o ${ICM_LIB_DIR}/libicmoslib.a
	make -C ${FREE_RTOS_BASE} clean
