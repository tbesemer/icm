FREE_RTOS_ROOT := $(shell pwd)
FREE_RTOS_KERNEL := $(shell cd ../FreeRTOS-Sim/Source; pwd)

CFLAGS := -DDEBUG=1 -g -UUSE_STDIO -D__GCC_POSIX__=1 -pthread -DMAX_NUMBER_OF_TASKS=300 -I ${FREE_RTOS_KERNEL}/include -I ${FREE_RTOS_KERNEL}/portable/GCC/POSIX -I./ -W -Wall -Werror -Wextra -Wformat -Wmissing-braces -Wno-cast-align -Wparentheses -Wshadow -Wno-sign-compare -Wswitch -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wmissing-prototypes

OBJ_LIST := obj/croutine.o obj/event_groups.o obj/list.o obj/queue.o obj/tasks.o obj/timers.o obj/heap_3.o obj/port.o

.PHONY: all
all: free_rtos_objs freertos_test
	gcc -o freertos_test obj/freertos_test.o obj/freertos_hooks.o obj/freertosx86.o -lrt -lpthread

.PHONY: freertos_test
freertos_test:
	gcc -c ${CFLAGS} freertos_test.c -o obj/freertos_test.o
	gcc -c ${CFLAGS} freertos_hooks.c -o obj/freertos_hooks.o

.PHONY: free_rtos_objs
free_rtos_objs:	${OBJ_LIST}
	ld -r -o obj/freertosx86.o ${OBJ_LIST}

obj/croutine.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/croutine.c -o obj/croutine.o

obj/list.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/list.c -o obj/list.o

obj/event_groups.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/event_groups.c -o obj/event_groups.o

obj/queue.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/queue.c -o obj/queue.o

obj/tasks.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/tasks.c -o obj/tasks.o

obj/timers.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/timers.c -o obj/timers.o

obj/heap_3.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/portable/MemMang/heap_3.c -o obj/heap_3.o

obj/port.o:
	gcc -c ${CFLAGS} ${FREE_RTOS_KERNEL}/portable/GCC/POSIX/port.c -o obj/port.o

.PHONY: clean
clean:
	rm -f obj/* freertos_test
