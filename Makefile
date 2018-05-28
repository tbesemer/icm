ICM_ROOT_DIR := $(shell pwd)
ICM_INCLUDE_DIR := ${ICM_ROOT_DIR}/include
ICM_SRC_DIR := ${ICM_ROOT_DIR}/src
ICM_CONF_DIR := ${ICM_ROOT_DIR}/conf
ICM_OBJ_DIR := ${ICM_ROOT_DIR}/obj
ICM_LIB_DIR := ${ICM_ROOT_DIR}/lib
ICM_TEST_DIR := ${ICM_ROOT_DIR}/test

# Determine how we compile this.  We have three targets:
#
#  1. FreeRTOS on ARM, which implies Xilinx SDK right now.  This
#     Requires FREERTOS_INCLUDE_PATH to be set.
#  2. Posix on Ubuntu, using standard X896 POSIX Semaphores/Pthreads
#  3. FreeRTOS on x86 Ubuntu, using FreeRTOS-Sim.
#
ifeq ($(TARGET_ARCH),ARM_FREERTOS)
ifeq ($(FREERTOS_INCLUDE_PATH),)
$(error "FREERTOS_INCLUDE_PATH not set correctly")
endif
ICM_PORT_DIR := ${ICM_ROOT_DIR}/port/FreeRTOS
ICM_PORT_CMD := icmosal_cross
else ifeq ($(TARGET_ARCH),X86_LINUX)
ICM_PORT_DIR := ${ICM_ROOT_DIR}/port/linux
ICM_PORT_CMD := all
else ifeq ($(TARGET_ARCH),FREERTOS_LINUX)
ICM_PORT_DIR := ${ICM_ROOT_DIR}/port/FreeRTOS
ICM_PORT_CMD := all
else
$(error "TARGET_ARCH not set correctly")
endif

export CROSS_CC := ${FREERTOS_CROSS_PREFIX}gcc
export CROSS_AR := ${FREERTOS_CROSS_PREFIX}ar
export CROSS_RANLIB := ${FREERTOS_CROSS_PREFIX}ranlib

CFLAGS := -I${ICM_INCLUDE_DIR} -I${ICM_SRC_DIR} -Wall

.PHONY: all 
all: ${ICM_OBJ_DIR}/icm_core.o ${ICM_OBJ_DIR}/icm_pool.o ${ICM_OBJ_DIR}/icm_log.o ${ICM_OBJ_DIR}/icm_config.o  osal
	${CROSS_AR} -rvs ${ICM_LIB_DIR}/libicmlib.a ${ICM_OBJ_DIR}/icm_core.o ${ICM_OBJ_DIR}/icm_pool.o ${ICM_OBJ_DIR}/icm_log.o ${ICM_OBJ_DIR}/icm_config.o 
	${CROSS_RANLIB} ${ICM_LIB_DIR}/libicmlib.a 

.PHONY: test_code
test_code:
	make -C ${ICM_TEST_DIR}
	
${ICM_OBJ_DIR}/icm_core.o: ${ICM_SRC_DIR}/icm_core.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	$(CROSS_CC) ${CFLAGS} -c -o ${ICM_OBJ_DIR}/icm_core.o ${ICM_SRC_DIR}/icm_core.c

${ICM_OBJ_DIR}/icm_pool.o: ${ICM_SRC_DIR}/icm_pool.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	$(CROSS_CC) ${CFLAGS} -c -o ${ICM_OBJ_DIR}/icm_pool.o ${ICM_SRC_DIR}/icm_pool.c

${ICM_OBJ_DIR}/icm_log.o: ${ICM_SRC_DIR}/icm_log.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	$(CROSS_CC) ${CFLAGS} -c -o ${ICM_OBJ_DIR}/icm_log.o ${ICM_SRC_DIR}/icm_log.c

${ICM_OBJ_DIR}/icm_config.o: ${ICM_CONF_DIR}/icm_config.c ${ICM_SRC_DIR}/icm_private.h ${ICM_INCLUDE_DIR}/icm_pub.h
	$(CROSS_CC) ${CFLAGS} -c -o ${ICM_OBJ_DIR}/icm_config.o ${ICM_CONF_DIR}/icm_config.c

.PHONY: osal
osal:
	make -C ${ICM_PORT_DIR} ${ICM_PORT_CMD}

clean:
	rm -f ${ICM_OBJ_DIR}/* ${ICM_LIB_DIR}/*
	make -C ${ICM_PORT_DIR} clean
	make -C ${ICM_TEST_DIR} clean
