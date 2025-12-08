# Directories
TOOLS_DIR = ${TOOLS_PATH}
MSPGCC_ROOT_DIR = $(TOOLS_DIR)/msp430-gcc
MSPGCC_BIN_DIR = $(MSPGCC_ROOT_DIR)/bin
MSPGCC_INCLUDE_DIR = $(MSPGCC_ROOT_DIR)/include

INCLUDE_DIRS = $(MSPGCC_INCLUDE_DIR)
LIB_DIRS = $(MSPGCC_INCLUDE_DIR)
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
TI_CCS_DIR = $(TOOLS_DIR)/ccs2040/ccs
DEBUG_BIN_DIR = $(TI_CCS_DIR)/ccs_base/DebugServer/bin
DEBUG_DRIVERS_DIR = $(TI_CCS_DIR)/ccs_base/DebugServer/drivers

# Toolchain
CC = $(MSPGCC_BIN_DIR)/msp430-elf-gcc
RM = rm
DEBUG = LD_LIBRARY_PATH=$(DEBUG_DRIVERS_DIR) $(DEBUG_BIN_DIR)/mspdebug
CPPCHECK = cppcheck
FORMAT = clang-format

# Files
TARGET = $(BIN_DIR)/jr

SOURCES_W_HEADERS = 	\
	src/drivers/io.c
#	src/drivers/uart.c	\
	src/drivers/i2c.c	\
	src/app/drive.c		\
	src/app/enemy.c		\
	src/app/line.c

SOURCES = src/main.c \
	$(SOURCES_W_HEADERS)

HEADERS = \
	$(SOURCES_W_HEADERS:.c=.h) \
	src/common/defines.h
OBJECT_NAMES = $(SOURCES:.c=.o)
OBJECTS = $(patsubst %, $(OBJ_DIR)/%,$(OBJECT_NAMES))

# Flags
MCU = msp430g2553
WFLAGS = -Wall -Wextra -Werror -Wshadow
CFLAGS = -mmcu=$(MCU) $(WFLAGS) $(addprefix -I , $(INCLUDE_DIRS)) -Og -g
LDFLAGS = -mmcu=$(MCU) $(addprefix -L , $(LIB_DIRS))

# Build
## Linking
$(TARGET): $(OBJECTS) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

## Compiling
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

# Phonies
.PHONY: all clean flash cppcheck format

all: $(TARGET)

clean:
	$(RM) -r $(BUILD_DIR)

flash:
	$(DEBUG) tilib "prog $(TARGET)"

cppcheck:
	@$(CPPCHECK)	--quiet 	\
	--enable=all 			\
	--error-exitcode=1		\
	--inline-suppr			\
	-i external/printf		\
	--suppress=unusedFunction			\
	--suppress=missingIncludeSystem		\
	--suppress=unmatchedSuppression		\
	$(SOURCES)				\

format:
	@$(FORMAT) -i $(SOURCES) $(HEADERS)
