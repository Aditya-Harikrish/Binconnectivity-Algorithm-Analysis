# -*- Makefile -*-
SRC_DIR = src
HEADERS_DIR = $(SRC_DIR)

CC = clang++
CFLAGS = -I$(HEADERS_DIR) -g -std=c++17 # -fsanitize=thread 

HEADERS_PATHLESS = mm.hpp
HEADERS = $(patsubst %, $(HEADERS_DIR)/%, $(HEADERS_PATHLESS))

EXECUTABLES = tarjan

# all: $(SRC_DIR)/%.cpp $(HEADERS_DIR)/%.h $(HEADERS_DIR)/%.hpp 
# 	$(CC) $(CFLAGS) $^ -o $@

tarjan: $(SRC_DIR)/Tarjan.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(SRC_DIR)/Tarjan.cpp -o $@

debug: CFLAGS += -DDEBUG=1 -Wall -Wextra -O2 -Wswitch-default -Wconversion -Wundef -fsanitize=address -fsanitize=undefined -fstack-protector # -Werror
debug: tarjan

dev: CFLAGS += -Wall -Wextra -O2 -Wswitch-default -Wconversion -Wundef -fsanitize=address -fsanitize=undefined -fstack-protector
dev: tarjan

clean:
	rm -f tarjan