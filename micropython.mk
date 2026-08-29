# micropython.mk
# Makefile definition for backward compatibility with standard Make builds
USERMOD_DIR := $(USERMOD_DIR)
SRC_USERMOD += $(USERMOD_DIR)/esp32_twt.c
CFLAGS_USERMOD += -I$(USERMOD_DIR)
