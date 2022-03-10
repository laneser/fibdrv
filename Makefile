CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv

obj-m := $(TARGET_MODULE).o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) client client_statistic
	$(MAKE) -C $(KDIR) M=$(PWD) modules

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null

client: client.c
	$(CC) -o $@ $^

client_statistic: client_statistic.c
	$(CC) -o $@ $^ -lm

PRINTF = env printf
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
pass = $(PRINTF) "$(PASS_COLOR)$1 Passed [-]$(NO_COLOR)\n"

CHECK_METHODS=0 1 2

check: all
	$(MAKE) unload
	$(MAKE) load
	for i in $(CHECK_METHODS); do \
	    sudo ./client $$i > out_$$i ; \
	done
	$(MAKE) unload
	for i in $(CHECK_METHODS); do \
	    diff -u out_$$i scripts/expected.txt && $(call pass,Method_$$i) ; \
	    scripts/verify.py out_$$i; \
	done

plot: all
	sh do_measurement.sh