----------------------------------------------
--! @file curses.lua
--! @author William Blount
--! @date 6/18/2018
--! @brief object oriented wrapper for curses.
----------------------------------------------

c_curses = require("c_curses")

local Screen = {}
Screen.__index = Screen

setmetatable(Screen, {
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:init(...)
        return self
    end,
})

function Screen:init()
    self.ctx = c_curses.screen_init();
end

function Screen:read(n, blocking)
    return c_curses.screen_read(self.ctx, n, blocking)
end

function Screen:readline(blocking)
    return c_curses.screen_readline(self.ctx, blocking)
end

function Screen:iscolor()
    return c_curses.screen_iscolor(self.ctx)
end

function Screen:getsize()
    return c_curses.screen_getsize(self.ctx)
end

function Screen:getcursor()
    return c_curses.screen_getcursor(self.ctx)
end

function Screen:setcursor(x, y)
    c_curses.screen_setcursor(self.ctx, x, y)
end

function Screen:write(char, fg, bg)
    c_curses.screen_write(self.ctx, fg, bg)
end

function Screen:clear()
    c_curses.screen_clear(self.ctx)
end

function Screen:refresh()
    c_curses.screen_refresh(self.ctx)
end

function Screen:destroy()
    c_curses.screen_destroy()
end

----------------------------------------------

local colors = {
    BLACK =     0,
    RED =       1,
    GREEN =     2,
    YELLOW =    3,
    BLUE =      4,
    MAGENTA =   5,
    CYAN =      6,
    WHITE =     7
}

----------------------------------------------

local curses = {
    Screen = Screen,
    colors = colors
}