include projects/version.mk

LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
LIBMCU_MODULES := cli
include $(LIBMCU_ROOT)/projects/modules.mk

APP_SRCS = \
	$(BASEDIR)/src/main.c \
	$(BASEDIR)/src/cli/cli_commands.c \
	$(BASEDIR)/src/cli/cmd_exit.c \
	$(BASEDIR)/src/cli/cmd_help.c \
	$(BASEDIR)/src/cli/cmd_info.c \
	$(BASEDIR)/src/cli/cmd_memdump.c \
	$(BASEDIR)/src/cli/cmd_reboot.c \
	$(LIBMCU_MODULES_SRCS) \

APP_INCS = \
	$(BASEDIR)/include \
	$(LIBMCU_MODULES_INCS) \

APP_DEFS = \
	$(DEVICE) \
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION)
