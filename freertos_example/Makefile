ICM_ROOT_DIR := $(shell cd ..; pwd)
ICM_INCLUDE_DIR := ${ICM_ROOT_DIR}/include
ICM_SRC_DIR := ${ICM_ROOT_DIR}/src
ICM_CONF_DIR := ${ICM_ROOT_DIR}/conf
ICM_OBJ_DIR := ${ICM_ROOT_DIR}/obj
ICM_LIB_DIR := ${ICM_ROOT_DIR}/lib
ICM_LIBS := -licmoslib -licmlib -licmoslib

export CROSS_CC := ${FREERTOS_CROSS_PREFIX}gcc
export CROSS_AR := ${FREERTOS_CROSS_PREFIX}ar
export CROSS_RANLIB := ${FREERTOS_CROSS_PREFIX}ranlib

CFLAGS := -mcpu=cortex-a9 -mfloat-abi=hard -mfpu=vfpv3 -I${ICM_INCLUDE_DIR} -I${ICM_SRC_DIR} 

ifeq ($(TARGET_ARCH),ARM_FREERTOS)
ifeq ($(FREERTOS_INCLUDE_PATH),)
$(error "FREERTOS_INCLUDE_PATH not set correctly")
endif
ICM_PORT_DIR := ${ICM_ROOT_DIR}/port/FreeRTOS
ICM_PORT_CMD := icmosal_cross
else
$(error "TARGET_ARCH not set to ARM_FREERTOS, source ../do_env_freertos_xsdkrc")
endif


.PHONY: all
all: icm_base two_tasks

.PHONY: two_tasks
two_tasks:  two_tasks.c Makefile
	$(CROSS_CC) -L ${ICM_LIB_DIR} ${CFLAGS} -c -o two_tasks.o  two_tasks.c  ${ICM_LIBS} 

.PHONY: icm_base
icm_base:
	@ if [ ! -f .icm_base ]; \
          then \
	      make -C ${ICM_ROOT_DIR} ;\
              touch .icm_base; \
          else \
              echo "ICM Base already built, Skipping";\
          fi;

.PHONY: clean
clean:
	rm -f two_tasks.o
