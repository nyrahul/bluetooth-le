ifeq ($(PLATFORM),android)
CC := gcc
else
CC := gcc
CF := clang-format-6.0
endif

BIN  := bin
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)/%.o,$(SRCS))
BT_SRCS := $(wildcard bt/*.c)
OBJS += $(patsubst %.c,$(BIN)/%.o,$(BT_SRCS))
TARGET := $(BIN)/libhidevice.so
COMMON_HDR := isync.h epoll_util.h isync_appexp.h isync_pal.h
CFLAGS := -Wall -g -DUSE_RFCOMM=7 -fPIC -I.
LDFLAGS := -fPIC
#LDFLAGS := -lbluetooth -lpthread

all: $(BIN) $(TARGET)

$(BIN):
	@$(CF) -i $(COMMON_HDR)
	@mkdir -p $(BIN)/bt 2>/dev/null

$(BIN)/%.o: %.c $(COMMON_HDR)
	@$(CF) -i $<
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(COMMON_HDR)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)

clean:
	@rm -rf ./$(BIN)
