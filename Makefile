CC=gcc
CF=clang-format-6.0
BIN=bin
TARGETS=$(BIN)/isync_advertise $(BIN)/isync_scan
COMMON_HDR=isync.h
LDFLAGS=-lbluetooth

all: $(BIN) $(TARGETS)

$(BIN):
	@mkdir -p $(BIN) 2>/dev/null

$(BIN)/isync_advertise: isync_advertise.c $(COMMON_HDR)
	@$(CF) -i $<
	$(CC) $< -o $@ $(LDFLAGS)

$(BIN)/isync_scan: isync_scan.c $(COMMON_HDR)
	@$(CF) -i $<
	$(CC) $< -o $@ $(LDFLAGS)

clean:
	@rm -rf ./$(BIN)
