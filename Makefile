CC := gcc
CF := clang-format-6.0
BIN  := bin
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)/%.o,$(SRCS))
TARGET := $(BIN)/isync_ble
COMMON_HDR := isync.h epoll_util.h
CFLAGS := -Wall -g
LDFLAGS := -lbluetooth -lpthread

all: $(BIN) $(TARGET)

$(BIN):
	@$(CF) -i $(COMMON_HDR)
	@mkdir -p $(BIN) 2>/dev/null

$(BIN)/%.o: %.c $(COMMON_HDR)
	@$(CF) -i $<
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(COMMON_HDR)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@rm -rf ./$(BIN)
