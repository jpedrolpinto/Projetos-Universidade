
CC := gcc

EXEC := programa-principal
FLAGS := -Wall -Wextra -Wdouble-promotion -Werror=pedantic -Werror=vla -pedantic-errors -Wfatal-errors

DEBUG ?= 0
GRAPH ?= 0
ifeq ($(DEBUG), 1)
	FLAGS += -O0 -g
else ifeq ($(GRAPH), 1)
	FLAGS += -O3 -pg
else
	FLAGS += -O3
endif

LIBS := -lm `pkg-config --libs glib-2.0`
PKG_CONFIG := `pkg-config --cflags glib-2.0`
INC := -I includes/
SRC := $(shell find src/ -name "*.c")
OBJ := $(SRC:src/%.c=build/%.o)

BUILD_DIR := $(shell mkdir -p build/modes build/io build/types build/catalogs build/utilities)
INCLUDE_FILES := $(shell find includes/ -name "*.h")

DATASET_PATH ?= "dataset/data"
QUERIES_PATH ?= "dataset/inputs.txt"

.SUFFIXES:

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJ)
	@$(CC) $(FLAGS) $^ ${PKG_CONFIG} $(LIBS) -o $@ ; echo "[Compiling] $@"

build/%.o: src/%.c
	@$(CC) $(FLAGS) -c $< ${PKG_CONFIG} $(LIBS) $(INC) -o $@ ; echo "[Linking] $@"

.PHONY: leaks
leaks: $(EXEC)
	@if [ "$(DATASET_PATH)" == 0 ] || [ "$(QUERIES_PATH)" == 0 ]; then \
	  echo -e "\033[0;31mYou should pass DATASET_PATH and QUERIES_PATH to the command\033[0m"; \
	else \
	 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(EXEC) $(DATASET_PATH) $(QUERIES_PATH); \
	fi

.PHONY: clean
clean:
	@rm -f programa-principal ; echo "[Cleaning] programa-principal"
	@rm -rf build/ ; echo "[Cleaning] build/"
	@rm -rf Resultados/ ; echo "[Cleaning] Resultados/"

.PHONY: format
format:
	@clang-format -i --verbose $(SRC) $(INCLUDE_FILES) $(SRC_TEST)

.DELETE_ON_ERROR: