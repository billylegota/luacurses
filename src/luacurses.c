/**
 * @file luacurses.c
 * @author William Blount
 * @date 6/15/2018
 * @brief a simple lua wrapper for ncurses
 */

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <assert.h>
#include <curses.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define NUM_COLORS 8

typedef struct {
    WINDOW *window;
    bool    isColor;
    bool    isActive;
} _screen_ctx_t;

typedef struct {
    const char *name;
    const short id;
} _color_t;

static const _color_t colors[] = {
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
 * the colorpair has not already been initialized, initializes it. Color pair
 * IDs are guaranteed to be unique between different fg, bg pairs.
 * @param fg the foreground color. Must be one of the curses COLOR_* values.
 * @param bg the background color. Must be one of the curses COLOR_* values.
 * @return the ID of the color pair.
 */
static short colors_to_pair(short fg, short bg) {
    // Calculate the ID from the fg and bg.
    short id = fg * NUM_COLORS + bg;

    // Initialize the color pair with the given fg and bg.
    init_pair(id, fg, bg);

    // Return the ID.
    return id;
}

int l_init(lua_State *L) {
    _screen_ctx_t *ctx = lua_newuserdata(L, sizeof(*ctx));
    ctx->window = initscr();
    ctx->isColor = has_colors();
    ctx->isActive = true;

    if(ctx->isColor) {
        start_color();
    }

    return 1;
}

int l_read(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);
    int n = luaL_checkinteger(L, 2);
    bool blocking = lua_toboolean(L, 3);

    nodelay(ctx->window, !blocking);

    char buf[n];
    for(int i = 0; i < n; i++) {
        int ch = wgetch(ctx->window);
        if(ch < 0) {
            buf[n] = 0;
            break;
        }
        buf[n] = (char) ch;
    }
    lua_pushstring(L, buf);

    return 1;
}

int l_readline(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);
    bool blocking = lua_toboolean(L, 2);

    nodelay(ctx->window, !blocking);
    char *buf = (char *) calloc(MAX_LINE_LENGTH, sizeof(*buf));
    wgetstr(ctx->window, buf);

    lua_pushstring(L, buf);

    return 1;
}

int l_iscolor(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    lua_pushboolean(L, ctx->isColor);

    return 1;
}

int l_isactive(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    lua_pushboolean(L, ctx->isActive);

    return 1;
}

int l_getsize(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    int x;
    int y;
    getmaxyx(ctx->window, y, x);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

int l_getcursor(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    int x;
    int y;
    getyx(ctx->window, y, x);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

int l_setcursor(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    wmove(ctx->window, y, x);

    return 0;
}

int l_write(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    const char *str = luaL_checkstring(L, 2);

    short fg = (short) luaL_checkinteger(L, 3);
    short bg = (short) luaL_checkinteger(L, 4);

    short id = colors_to_pair(fg, bg);

    wattron(ctx->window, COLOR_PAIR(id));
    waddstr(ctx->window, str);
    wattroff(ctx->window, COLOR_PAIR(id));

    return 0;
}

int l_clear(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    wclear(ctx->window);

    return 0;
}

int l_refresh(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

    wrefresh(ctx->window);

    return 0;
}

int l_destroy(lua_State *L) {
    assert(lua_isuserdata(L, 1));
    _screen_ctx_t *ctx = lua_touserdata(L, 1);

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
    {"getsize",      l_getsize},
    {"getcursor",    l_getcursor},
    {"setcursor",    l_setcursor},
    {"write",        l_write},
    {"clear",        l_clear},
    {"refresh",      l_refresh},
    {"destroy",      l_destroy},
    {NULL, NULL}
};

int luaopen_libcurses(lua_State *L) {
    // Load the functions.
    luaL_newlib(L, libcurses);

    // Load the color constants.
    lua_pushstring(L, "colors");
    lua_newtable(L);
    for(int i = 0; i < NUM_COLORS; i++) {
        lua_pushstring(L, colors[i].name);
        lua_pushinteger(L, colors[i].id);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);

    return 1;
}
