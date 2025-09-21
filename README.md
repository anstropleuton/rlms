# RLMS

RLMS is a Minesweeper implementation using raylib, combining original minesweeper style from Windows 95/98 with modern Minesweeper features.

## Minesweeper Features

- **Core**: Core minesweeper game.
- **First click**: Mines are placed only after first click.
- **Logically solvable**: Board requires no guess to solve.
- **Flag**: Mark cell as mine (reduces total mines count and prevents accidental reveal).
- **Question-mark**: Mark cell as maybe mine (only prevents accidental reveal).
- [**Speed reveal**](#speed-reveal).
- [**Speed flag**](#speed-flag).

### Speed Reveal

Trying to reveal a revealed cell with the number of neighboring mines equal
the number of hidden, unmarked adjacent cell, will reveal all those adjacent
cells.

### Speed Flag

Trying to flag a revealed cell with the number of neighboring mines equal
the number of hidden, unmarked adjacent cell, will flag all those adjacent
cells.

### First Click

Mines are generated only after the first click.

To make sure a good portion of the board is revealed on the first click,
the game does not place mines even at the neighboring cells of the first
clicked cell.

## License

This project is released under the Public Domain or licensed under the terms of MIT license.

Fonts used in this project (Bungee and Noto Color Emoji) are licensed under the terms of SIL Open Font License 1.1.
