CC=gcc
CFLAGS=--std=c99 -Wall -Iinclude

BUILD_DIR=build
SRC_DIR=src
TEST_DIR=tests
INC_DIR=include

TARGET=$(BUILD_DIR)/main
TEST_TARGET=$(BUILD_DIR)/template_tests



HEADERS := $(INC_DIR)/cnf.h $(INC_DIR)/template_test.h $(INC_DIR)/parse.h $(INC_DIR)/add_conditions.h
OBJECTS := main.o add_conditions.o cnf.o parse.o
LIB_OBJECTS := add_conditions.o cnf.o parse.o
TEST_OBJECTS := template_test.o $(LIB_OBJECTS)

OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJECTS))
TEST_OBJECTS := $(addprefix $(BUILD_DIR)/, $(TEST_OBJECTS))

default: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/template_test.o: $(TEST_DIR)/template_test.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -Wall -o $@

test:
	@python3 ../tests/run_tests.py

.PHONY: template_test
template_test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)
