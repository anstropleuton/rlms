/// @file
/// @copyright (c) 2025 Anstro Pleuton
/// This project is released under the Public Domain or licensed under the terms of MIT license.

#pragma once

#include <random>
#include <vector>

namespace rlms {

/// Minesweeper board configuration.
struct config {
    int width    = 8;   ///< Board width (number of columns).
    int height   = 8;   ///< Board height (number of rows).
    int mines    = 10;  ///< Number of mines to generate on the board.
    int seed     = -1;  ///< RNG seed. Use -1 to randomize seed.
    int attempts = 100; ///< Max generation attempts for logically solvable board.

    void randomize_seed() {
        std::random_device rd;
        seed = rd();
    }

    /// Validate the board configuration.
    bool validate() {
        if (width < 1 || height < 1 || attempts < 1) {
            return false;
        }

        if (mines > width * height - 9) {
            return false;
        }

        return true;
    }
};

/// Minesweeper cell state.
/// @note Flagged cells can still be hidden. Instead of checking if a cell is
///       hidden, check if the cell is not revealed.
enum class cell_state {
    revealed,
    hidden,
    flagged,
    qmarked
};

/// Minesweeper cell.
struct cell {
    bool is_mine = false; ///< True if the cell is a mine.
    int  n_mines = 0;     ///< Number of neighboring mines.

    cell_state state = cell_state::hidden; ///< State of the cell.
};

/// Minesweeper game state.
enum class game_state {
    first_click, ///< First click required.
    playing,     ///< Game is running.
    won,         ///< Player flagged all correct mines.
    lost         ///< Player revealed a cell with a mine.
};

/// The Minesweeper.
/// @note The member functions will ignore provided invalid coordinates.
struct minesweeper {
    config     cfg;                ///< Minesweeper board configuration.
    game_state state;              ///< Minesweeper game state.
    bool       unsolvable = false; ///< Whether the board is logically unsolvable.

    /// Minesweeper board, the grid of cells.
    /// @note It is neither column-major nor row-major. The access pattern is
    ///       board[x][y], where x in [0, width), y in [0, height).
    std::vector<std::vector<cell>> board;

    /// Get reference for cell at x, y.
    cell &at(int x, int y);

    /// Get constant reference for cell at x, y.
    const cell &at(int x, int y) const;

    /// Ensures that the board is properly resized.
    void ensure_size();

    /// Initialize (resize) the board.
    void initialize_board();

    /// Reset everything and reinitialize the board. Useful after changing the
    /// configuration.
    void reset();

    /// Obtain the neighboring cells of the given cell coordinates.
    std::vector<std::pair<int, int>> neighbors(int x, int y) const;

    /// Generate mines in the board in a logically solvable manner by excluding
    /// the specified coordinates and its neighbors.
    void generate_mines(int x, int y);

    /// Check if all the non-mine cells are revealed.
    bool check_won() const;

    /// Number of cells flagged.
    int cells_flagged() const;

    /// Reveal the cell and non-0 mines neighbors.
    void reveal(int x, int y);

    /// Perform speed reveal on the revealed cell.
    void speed_reveal(int x, int y);

    /// Toggle the cell state (hidden -> flagged -> qmarked -> hidden).
    void toggle(int x, int y);

    /// Speed flag neighbor cells.
    void speed_flag(int x, int y);

    /// Primary click (usually left click) on the board. This will reveal or
    /// performs speed reveal on the cell.
    void primary_click(int x, int y);

    /// Secondary click (usually right click) on the board. This will flag or
    /// performs speed flag on the cell.
    void secondary_click(int x, int y);

    /// Try to solve the board logically from the first click coords.
    /// @note Do not call it during gameplay, as it mutates state and destroys
    ///       it later. This is only used when generating the board.
    bool logically_solvable(int x, int y);
};

}; // namespace rlms
