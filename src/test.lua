-------------------------------------------
--! @file test.lua
--! @author William Blount
--! @date 6/19/2018
--! @brief simple test script for luacurses
-------------------------------------------

local curses = require("curses")
local colors = curses.colors

local function do_test()
    local screen = curses.Screen()
    screen:setcursor(1, 1)
    screen:write("Hello, World!", colors.RED, colors.BLACK)
    screen:setcursor(2, 2)
    screen:write("Name: ", colors.RED, colors.BLACK)
    screen:refresh()
    local name = screen:readline()
    screen:setcursor(3, 3)
    screen:write(name, colors.RED, colors.BLACK)
    screen:refresh()
    screen:destroy()
end

do_test()
