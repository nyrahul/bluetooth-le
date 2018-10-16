CC := gcc
BIN  := .
SRCS := $(wildcard *.c)
TARGET := $(BIN)/app
CFLAGS := -I../ -Wall -g -fPIC
LDFLAGS := -lpthread -L../bin/linux -lhidevice_linux -lbluetooth

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	@rm -rf $(TARGET)
