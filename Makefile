# Check arguments
ifeq ($(HW),LAUNCHPAD)
TARGET_NAME=launchpad
else ifeq ($(HW),JR)
TARGET_NAME=jr
else ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),cppcheck)
else ifeq ($(MAKECMDGOALS),format)
else
$(error "Must pass HW=LAUNCHPAD or HW=JR")
endif

# Directories
TOOLS_DIR = ${TOOLS_PATH}
MSPGCC_ROOT_DIR = $(TOOLS_DIR)/msp430-gcc
MSPGCC_BIN_DIR = $(MSPGCC_ROOT_DIR)/bin
MSPGCC_INCLUDE_DIR = $(MSPGCC_ROOT_DIR)/include
BUILD_DIR = build/$(TARGET_NAME)
OBJ_DIR = $(BUILD_DIR)/obj
TI_CCS_DIR = $(TOOLS_DIR)/ccs2040/ccs
DEBUG_BIN_DIR = $(TI_CCS_DIR)/ccs_base/DebugServer/bin
DEBUG_DRIVERS_DIR = $(TI_CCS_DIR)/ccs_base/DebugServer/drivers

LIB_DIRS = $(MSPGCC_INCLUDE_DIR)
INCLUDE_DIRS = $(MSPGCC_INCLUDE_DIR) \
				./src \
				./external/ \
				./external/printf


# Toolchain
CC = $(MSPGCC_BIN_DIR)/msp430-elf-gcc
RM = rm
DEBUG = LD_LIBRARY_PATH=$(DEBUG_DRIVERS_DIR) $(DEBUG_BIN_DIR)/mspdebug
CPPCHECK = cppcheck
FORMAT = clang-format

# Files
TARGET = $(BUILD_DIR)/$(TARGET_NAME)_exe

SOURCES_W_HEADERS = 	\
	src/drivers/io.c	\
	src/drivers/mcu_init.c \
	src/drivers/led.c	\
	src/common/assert_handler.c	
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

# Defines
HW_DEFINE = $(addprefix -D,$(HW))
DEFINES = $(HW_DEFINE)

# Static Analysis
## Don't check the msp430 helper headers
CPPCHECK_INCLUDES = ./src
CPPCHECK_IGNORE = external/printf
CPPCHECK_FLAGS = \
	--quiet --enable=all --error-exitcode=1		\
	--inline-suppr	--suppress=unusedFunction	\
	--suppress=missingIncludeSystem		\
	--suppress=unmatchedSuppression		\
	$(addprefix -I,$(CPPCHECK_INCLUDES))	\
	$(addprefix -i,$(CPPCHECK_IGNORE))

# Flags
MCU = msp430g2553
WFLAGS = -Wall -Wextra -Werror -Wshadow
CFLAGS = -mmcu=$(MCU) $(WFLAGS) $(addprefix -I , $(INCLUDE_DIRS)) $(DEFINES) -Og -g
LDFLAGS = -mmcu=$(MCU) $(DEFINES) $(addprefix -L , $(LIB_DIRS))

# Build
## Linking
$(TARGET): $(OBJECTS) $(HEADERS)
	echo $(OBJECTS)
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
	@$(RM) -r $(BUILD_DIR)

flash:
	echo $(TARGET)
	@$(DEBUG) tilib "prog $(TARGET)"

cppcheck:
	@$(CPPCHECK) $(CPPCHECK_FLAGS) $(SOURCES)

format:
	@$(FORMAT) -i $(SOURCES) $(HEADERS)
