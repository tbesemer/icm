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

ifeq ($(TARGET_ARCH),X86_LINUX)
ICM_PORT_DIR := ${ICM_ROOT_DIR}/port/linux
ICM_PORT_CMD := all
else
$(error "TARGET_ARCH not set to X86_LINUX, source ../do_env_posix_linuxrc")
endif

.PHONY: icm_base all
all: two_tasks

.PHONY: two_tasks
two_tasks: 
	$(CROSS_CC) -L ${ICM_LIB_DIR} ${CFLAGS} -o two_tasks  two_tasks.c  ${ICM_LIBS} -lpthread

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
	rm -f two_tasks
