LUA_INCLUDE = /usr/include/lua5.3

SRC_DIR = src
OBJ_DIR = obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -I$(LUA_INCLUDE)

CFLAGS += -fPIC -Wall -Wextra
CFLAGS += -ggdb -O0

LFLAGS += -llua5.3 -lcurses

all: dirs obj/libcurses.so obj/libcurses.a

dirs:
	mkdir -p obj/

obj/libcurses.so: $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -o $@ $^ $(LFLAGS)

obj/libcurses.a: $(OBJ)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OBJ_DIR)/*

.PHONY: all clean dirs
