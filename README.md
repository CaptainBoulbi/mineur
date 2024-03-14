# MINEUR

Minesweeper game.

Oh no, my os doesn't comme with minesweeper preinstalled, let's make it myself.

# Features

- classic minesweeper
- different difficulty (beginner, intermediate, expert)
- screenshot
- timer / best time

# Keybind

- q (for azerty) : close app
- a (for qwerty) : close app
- escape         : close app
- r : reset grid
- b : switch to beginner difficulty
- i : switch to intermediate difficulty
- e : switch to expert difficulty
- s : take a screenshot

all of these action can be done with the menu bar

# building

You need to install [raylib v5](https://github.com/raysan5/raylib/releases/tag/5.0), but any version should work.

```console
$ make
```

or to build in release mode :

```console
$ make release
```

to run :

```console
$ ./mineur
```

or

```console
$ make run
```
