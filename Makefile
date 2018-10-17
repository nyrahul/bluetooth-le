ifeq ($(PLATFORM),)
PLATFORM := linux
endif

ifeq ($(PLATFORM),android)
CC := aarch64-linux-android-gcc
else
CC := gcc
endif

BIN  := bin/$(PLATFORM)
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)/%.o,$(SRCS))

ifeq ($(PLATFORM),linux)
BT_SRCS := $(wildcard bt/*.c)
OBJS += $(patsubst %.c,$(BIN)/%.o,$(BT_SRCS))
endif

TARGET := $(BIN)/libhidevice_$(PLATFORM).so
COMMON_HDR := isync.h epoll_util.h isync_appexp.h isync_pal.h log.h
CFLAGS := -Wall -g -DUSE_RFCOMM=7 -fPIC -I.
LDFLAGS := -fPIC

ifeq ($(PLATFORM),android)
CFLAGS += -fPIE
endif

all: $(BIN) $(TARGET)

$(BIN):
	@mkdir -p $(BIN)/bt 2>/dev/null

$(BIN)/%.o: %.c $(COMMON_HDR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(COMMON_HDR)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)

clean:
	@rm -rf ./$(BIN)
