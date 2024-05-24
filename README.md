# luacurses

A simple Lua wrapper for curses. Attempts to expose an API which is more Lua-esque than just the base curses one.

## Getting Started

These instructions will help you get luacurses up and running for development and testing purposes.

### Prerequisites

In order to build luacurses, you will need the following:
- C compiler
- Make or CMake
- Lua 5.4 and the associated headers
- Curses and the associated headers (Ncurses is recommended)

### Building

1. Clone or download the repository:
    ```shell
    $ git clone https://github.com/billylegota/luacurses.git
    $ cd luacurses
    ```

2. Build using Make:
    ```shell
    $ make
    ```

3. Alternatively, you can build using CMake:
    ```shell
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    ```

### Installing

Copy `luacurses.so` and `curses.lua` to the same directory as your Lua executable or to your Lua libraries directory.

## Features

luacurses exposes a small subset of the curses API, focusing on essential and useful methods. The available methods are:

### Screen Class

- `Screen:init()` - Initializes the screen.
- `Screen:read(n, blocking)` - Reads `n` characters. If `blocking` is set to `false`, it can return less than `n` characters or `nil`.
- `Screen:readline(blocking)` - Reads a line. If `blocking` is set to `false`, it can return `nil`.
- `Screen:iscolor()` - Returns `true` if the screen supports color; `false` otherwise.
- `Screen:getsize()` - Returns the size of the screen as a table in the format `{0=x, 1=y}`.
- `Screen:getcursor()` - Returns the position of the cursor as a table in the format `{0=x, 1=y}`.
- `Screen:setcursor(x, y)` - Sets the cursor position to `{x, y}`.
- `Screen:write(string, fg, bg)` - Writes a string to the screen using `fg` as the foreground color and `bg` as the background color.
- `Screen:clear()` - Clears the screen.
- `Screen:refresh()` - Updates the screen. Must be called to show any changes made to the screen, including moving the cursor.
- `Screen:destroy()` - Closes the screen and returns the terminal to its original state.

## Built With

- [Lua 5.4](https://www.lua.org/) - Lua language.
- [Ncurses](https://www.gnu.org/software/ncurses/) - The flavor of curses used in testing this wrapper (although any curses library should work).

## Contributing

To contribute, fork the repository, make your changes, and create a pull request.

## License

This project is licensed under the MIT License - see [LICENSE.md](LICENSE.md) for details.
