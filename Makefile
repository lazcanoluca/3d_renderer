CC = gcc
WARN_FLAGS = -Wall -Wextra
CFLAGS = $(WARN_FLAGS) -std=c99 -I./lib

DEBUG_FLAGS = -ggdb -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG -Werror

LDFLAGS = -L$(BUILD_DIR) -larray -lSDL2 -lm

SRC = $(wildcard ./src/*.c)
BUILD_DIR = build

TARGET_DEBUG   = $(BUILD_DIR)/renderer_debug
TARGET_RELEASE = $(BUILD_DIR)/renderer_release

LIB_OBJECT     = $(BUILD_DIR)/array.o
LIBRARY        = $(BUILD_DIR)/libarray.a

# Ensure build directory always exists
$(shell mkdir -p $(BUILD_DIR))

# Full debug cycle: build + run
debug: build-debug run-debug

# Full release cycle: build + run
release: build-release run-release

build-debug: CFLAGS += $(DEBUG_FLAGS)
build-debug: $(TARGET_DEBUG)

build-release: CFLAGS += $(RELEASE_FLAGS)
build-release: $(TARGET_RELEASE)

$(TARGET_DEBUG): $(LIBRARY) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

$(TARGET_RELEASE): $(LIBRARY) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

$(LIB_OBJECT): ./lib/array.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(LIB_OBJECT)
	ar rcs $@ $^

run-debug:
	./$(TARGET_DEBUG)

run-release:
	./$(TARGET_RELEASE)

clean:
	rm -rf $(BUILD_DIR)
