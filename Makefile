BIN_NAME   := tlint
CC         := gcc
CC_FLAGS   := -Wall -Wextra -Wundef -Wconversion -Ofast
SRC_DIR    := src
OBJ_DIR    := build
BUILD_DIR  := ./build
TST_DIR    := ./tests
TST_FILE   := $(BUILD_DIR)/files.txt
SRC_EXT    := .c
SRC_FILES  := $(wildcard $(SRC_DIR)/*$(SRC_EXT))
OBJ_FILES  := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:$(SRC_EXT)=.o)))

ifndef VERBOSE
.SILENT:
endif

all: $(BIN_NAME)

$(BIN_NAME): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CC_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%$(SRC_EXT)
	$(CC) $(CXX_FLAGS) -c -o $@ $<


clean:
	rm -rf $(BUILD_DIR)/*
	rm -f ./$(BIN_NAME)

test:
	# First and most importantly: scan the source of the tool itself, just to make a point
	@echo "Tool source code:   `./$(BIN_NAME) $(BUILD_DIR)/own_src | wc -l` defects detected."
	# Then generate a list of test-files for the regression suite
	find $(TST_DIR) -name "*.[ch]" > $(TST_FILE)
	# - For each test-file, each line with an error contains the line 'HIT' - we count how many.
	#   That number is compared to the number of lines (warnings) output by the tool.
	@echo "Regression suite:   `./$(BIN_NAME) $(TST_FILE) | wc -l` / `grep -Rn HIT $(TST_DIR) --include=*.[ch] | wc -l` defects detected."
	find $(SRC_DIR) -name "*.[ch]" > $(BUILD_DIR)/own_src.txt



