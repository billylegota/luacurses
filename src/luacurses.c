/**
 * @file luacurses.c
 * @author William Blount
 * @date 6/15/2018
 * @brief A simple Lua wrapper for ncurses
 */

#include <curses.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>

#define SCREEN_CONTEXT_TYPE_METATABLE_NAME "luacurses.ScreenContext"
#define MAX_READ_LENGTH 1024
#define MAX_LINE_LENGTH 1024
#define NUM_COLORS (sizeof(colors) / (sizeof(colors[0])))

typedef struct {
    WINDOW *window;
    bool    isColor;
    bool    isActive;
} screen_ctx_t;

typedef struct {
    const char *name;
    const short id;
} color_t;

static const color_t colors[] = {
    {"BLACK",     COLOR_BLACK},
    {"RED",       COLOR_RED},
    {"GREEN",     COLOR_GREEN},
    {"YELLOW",    COLOR_YELLOW},
    {"BLUE",      COLOR_BLUE},
    {"MAGENTA",   COLOR_MAGENTA},
    {"CYAN",      COLOR_CYAN},
    {"WHITE",     COLOR_WHITE}
};

/**
 * Returns the ID of a color pair given the foreground and background colors. If
 * the color pair has not already been initialized, initializes it. Color pair
 * IDs are guaranteed to be unique between different fg, bg pairs.
 * @param fg the foreground color. Must be one of the curses COLOR_* values.
 * @param bg the background color. Must be one of the curses COLOR_* values.
 * @return the ID of the color pair.
 */
static short colors_to_pair(short fg, short bg) {
    return (short) (fg * NUM_COLORS + bg);
}

int l_init(lua_State *L) {
    screen_ctx_t *ctx = lua_newuserdata(L, sizeof(*ctx));
    luaL_getmetatable(L, SCREEN_CONTEXT_TYPE_METATABLE_NAME);
    lua_setmetatable(L, -2);

    ctx->window = initscr();
    ctx->isColor = has_colors();
    ctx->isActive = true;

    if (ctx->isColor) {
        start_color();

        // The number of colors is always representable as a short so this narrowing conversion is safe.
        for (short fg = 0; fg < (short) NUM_COLORS; fg++) {
            for (short bg = 0; bg < (short) NUM_COLORS; bg++) {
                short id = colors_to_pair(fg, bg);
                init_pair(id, fg, bg);
            }
        }
    }

    return 1;
}

int l_read(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);
    size_t n = luaL_checkinteger(L, 2);
    bool blocking = lua_toboolean(L, 3);

    if (n > MAX_READ_LENGTH) {
        return luaL_error(L, "Cannot read more than %d characters", MAX_READ_LENGTH);
    }

    nodelay(ctx->window, !blocking);

    char *buf = malloc(n + 1); // +1 for the null terminator
    if (!buf) {
        return luaL_error(L, "Memory allocation error");
    }

    buf[n] = '\0'; // Handles the case where we run out of buffer space before input.
    for (size_t i = 0; i < n; i++) {
        int ch = wgetch(ctx->window);
        // TODO: Implement some more robust logic here to handle special keys etc.
        if (ch < 0) {
            buf[i] = '\0';
            break;
        }
        buf[i] = (char)ch;
    }

    lua_pushstring(L, buf);
    free(buf);

    return 1;
}

int l_readline(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);
    bool blocking = lua_toboolean(L, 2);

    nodelay(ctx->window, !blocking);
    char *buf = (char *)calloc(MAX_LINE_LENGTH, sizeof(*buf));
    if (!buf) {
        return luaL_error(L, "Memory allocation error");
    }

    wgetstr(ctx->window, buf);

    lua_pushstring(L, buf);
    free(buf);

    return 1;
}

int l_iscolor(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    lua_pushboolean(L, ctx->isColor);

    return 1;
}

int l_isactive(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    lua_pushboolean(L, ctx->isActive);

    return 1;
}

int l_getsize(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    int x;
    int y;
    getmaxyx(ctx->window, y, x);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

int l_getcursor(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    int x;
    int y;
    getyx(ctx->window, y, x);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

int l_setcursor(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);
    lua_Integer x = luaL_checkinteger(L, 2);
    lua_Integer y = luaL_checkinteger(L, 3);

    if (x < 0 || x >= COLS) {
        return luaL_error(L, "Column out of range. Expected value in [0, %d) but got %d", COLS, x);
    }

    if (y < 0 || y >= LINES) {
        return luaL_error(L, "Row out of range. Expected value in [0, %d) but got %d", LINES, y);
    }

    // Narrowing conversion is safe as we already checked that `x` and `y` fit in an int.
    wmove(ctx->window, (int) y, (int) x);

    return 0;
}

int l_write(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    const char *str = luaL_checkstring(L, 2);

    lua_Integer fg = luaL_checkinteger(L, 3);
    lua_Integer bg = luaL_checkinteger(L, 4);

    if (fg < 0 || fg >= NUM_COLORS) {
        return luaL_error(L, "Invalid fg color: %d", fg);
    }

    if (bg < 0 || bg >= NUM_COLORS) {
        return luaL_error(L, "Invalid bg color: %d", bg);
    }

    // Narrowing conversion is safe as we already checked that `fg` and `bg` fit in a short.
    short id = colors_to_pair((short)fg, (short)bg);

    wattron(ctx->window, COLOR_PAIR(id));
    waddstr(ctx->window, str);
    wattroff(ctx->window, COLOR_PAIR(id));

    return 0;
}

int l_clear(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    wclear(ctx->window);

    return 0;
}

int l_refresh(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    wrefresh(ctx->window);

    return 0;
}

int l_destroy(lua_State *L) {
    screen_ctx_t *ctx = luaL_checkudata(L, 1, SCREEN_CONTEXT_TYPE_METATABLE_NAME);

    endwin();

    ctx->isActive = false;

    return 0;
}

// TODO: Add methods that allow configuring echo and other such properties of
// the screen.
static const struct luaL_Reg libcurses[] = {
    {"init",         l_init},
    {"read",         l_read},
    {"readline",     l_readline},
    {"iscolor",      l_iscolor},
    {"isactive",     l_isactive},
    {"getsize",      l_getsize},
    {"getcursor",    l_getcursor},
    {"setcursor",    l_setcursor},
    {"write",        l_write},
    {"clear",        l_clear},
    {"refresh",      l_refresh},
    {"destroy",      l_destroy},
    {NULL, NULL}
};

__attribute__((used)) int luaopen_libcurses(lua_State *L) {
    // Load the functions.
    luaL_newlib(L, libcurses);

    // Register the metatable for the screen context type with a unique name.
    luaL_newmetatable(L, SCREEN_CONTEXT_TYPE_METATABLE_NAME);
    lua_pushvalue(L, -1);  // Duplicate the metatable
    lua_setfield(L, -2, "__index");  // metatable.__index = metatable

    // Load the color constants.
    lua_pushstring(L, "colors");
    lua_newtable(L);
    for (size_t i = 0; i < NUM_COLORS; i++) {
        lua_pushstring(L, colors[i].name);
        lua_pushinteger(L, colors[i].id);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);

    return 1;
}
