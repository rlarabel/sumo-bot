# Check arguments for main program
ifeq ($(HW),LAUNCHPAD)
TARGET_HW=launchpad
else ifeq ($(HW),JR)
TARGET_HW=jr
else ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),cppcheck)
else ifeq ($(MAKECMDGOALS),format)
else
$(error "Must pass HW=LAUNCHPAD or HW=JR")
endif
TARGET_NAME=$(TARGET_HW)

# Check arguments for test program
ifneq ($(TEST),) # TEST argument
ifeq ($(findstring test_,$(TEST)),)
$(error "TEST=$(TEST) is invalid (test function must start with test_)")
else
TARGET_NAME=$(TEST)
endif
endif

# Directories
TOOLS_DIR = ${TOOLS_PATH}
MSPGCC_ROOT_DIR = $(TOOLS_DIR)/msp430-gcc
MSPGCC_BIN_DIR = $(MSPGCC_ROOT_DIR)/bin
MSPGCC_INCLUDE_DIR = $(MSPGCC_ROOT_DIR)/include
BUILD_DIR = build/
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
SIZE = $(MSPGCC_BIN_DIR)/msp430-elf-size
READELF = $(MSPGCC_BIN_DIR)/msp430-elf-readelf

# Files
TARGET = $(BUILD_DIR)/bin/$(TARGET_HW)/$(TARGET_NAME)

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

ifndef TEST
SOURCES = src/main.c \
	$(SOURCES_W_HEADERS)
else
SOURCES = src/test/test.c \
	$(SOURCES_W_HEADERS)
# Delete object file to force rebuild when changing test
$(shell rm -f $(BUILD_DIR)/obj/src/test/test.o)
endif

HEADERS = \
	$(SOURCES_W_HEADERS:.c=.h) \
	src/common/defines.h
OBJECT_NAMES = $(SOURCES:.c=.o)
OBJECTS = $(patsubst %, $(OBJ_DIR)/%,$(OBJECT_NAMES))

# Defines
HW_DEFINE = $(addprefix -D,$(HW))
TEST_DEFINE = $(addprefix -DTEST=, $(TEST))
DEFINES = \
	$(HW_DEFINE) \
	$(TEST_DEFINE)

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
CFLAGS = -mmcu=$(MCU) $(WFLAGS) $(addprefix -I , $(INCLUDE_DIRS)) $(DEFINES) -Og -g -fshort-enums
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
.PHONY: all clean flash cppcheck format size

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

size: $(TARGET)
	@$(SIZE) $(TARGET)

symbols: $(TARGET)
	# List symbols table sorted by size
	@$(READELF) -s $(TARGET) | sort -n -k3