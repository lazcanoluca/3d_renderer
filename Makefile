CC = gcc
CFLAGS = -Wall -Werror -std=c99 -I./lib
DEBUG_FLAGS = -ggdb -O0
RELEASE_FLAGS = -O2 -DNDEBUG
LDFLAGS = -L$(BUILD_DIR) -larray -lSDL2 -lm
SRC = ./src/*.c
BUILD_DIR = build
TARGET_DEBUG = $(BUILD_DIR)/renderer_debug
TARGET_RELEASE = $(BUILD_DIR)/renderer_release
LIBRARY = $(BUILD_DIR)/libarray.a

# Ensure build directory exists
$(shell mkdir -p $(BUILD_DIR))

debug: CFLAGS += $(DEBUG_FLAGS)
debug: build-debug run-debug

release: CFLAGS += $(RELEASE_FLAGS)
release: build-release run-release

build-debug: $(LIBRARY)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET_DEBUG) $(LDFLAGS)

build-release: $(LIBRARY)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET_RELEASE) $(LDFLAGS)

$(LIBRARY): ./lib/array.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $(BUILD_DIR)/array.o
	ar rcs $@ $(BUILD_DIR)/array.o

run-debug:
	./$(TARGET_DEBUG)

run-release:
	./$(TARGET_RELEASE)

clean:
	rm -rf $(BUILD_DIR)


