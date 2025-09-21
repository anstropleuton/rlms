/// @file
/// @copyright (c) 2025 Anstro Pleuton
/// This project is released under the Public Domain or licensed under the terms of MIT license.

#include <algorithm>
#include <queue>
#include <stdexcept>

#include "rlms.hpp"

rlms::cell &rlms::minesweeper::at(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        throw std::invalid_argument("x and y must be in 0..width and 0..height respectively.");
    }
    return board[x][y];
}

const rlms::cell &rlms::minesweeper::at(int x, int y) const {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        throw std::invalid_argument("x and y must be in 0..width and 0..height respectively.");
    }
    return board[x][y];
}

void rlms::minesweeper::ensure_size() {
    if (board.size() != cfg.width) {
        board.resize(cfg.width);
    }

    for (auto &linear : board) {
        if (linear.size() != cfg.height) {
            linear.resize(cfg.height);
        }
    }
}

void rlms::minesweeper::initialize_board() {
    state = game_state::first_click;
    ensure_size();
}

void rlms::minesweeper::reset() {
    board = {};
    initialize_board();
}

std::vector<std::pair<int, int>> rlms::minesweeper::neighbors(int x, int y) const {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return {};
    }

    std::vector<std::pair<int, int>> neighbors;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = dx + x;
            int ny = dy + y;

            if (nx < 0 || nx >= cfg.width || ny < 0 || ny >= cfg.height) {
                continue;
            }

            neighbors.emplace_back(nx, ny);
        }
    }

    return neighbors;
}

void rlms::minesweeper::generate_mines(int x, int y) {
    if (!cfg.validate()) {
        return;
    }

    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    // Forbidden cells
    std::vector<std::pair<int, int>> forb = {
        {x, y}};
    for (auto [nx, ny] : neighbors(x, y)) {
        forb.emplace_back(nx, ny);
    }

    std::mt19937 gen(cfg.seed);

    unsolvable = true;
    for (int i = 0; i < cfg.attempts; i++) {
        // Reset board completely
        for (int x = 0; x < cfg.width; x++) {
            for (int y = 0; y < cfg.height; y++) {
                at(x, y) = cell();
            }
        }

        // Create list of allowed positions
        std::vector<std::pair<int, int>> allowed;
        allowed.reserve(cfg.width * cfg.height);
        for (int x = 0; x < cfg.width; ++x) {
            for (int y = 0; y < cfg.height; ++y) {
                if (std::find(forb.begin(), forb.end(), std::make_pair(x, y)) == forb.end()) {
                    allowed.emplace_back(x, y);
                }
            }
        }

        // Make sure we have enough space
        if (allowed.size() < cfg.mines) {
            throw std::runtime_error("Not enough free cells to place mines.");
        }

        std::shuffle(allowed.begin(), allowed.end(), gen);

        // Place mines
        for (int j = 0; j < cfg.mines; ++j) {
            auto [mx, my]      = allowed[j];
            at(mx, my).is_mine = true;
        }

        // Compute neighbor mines count
        for (int x = 0; x < cfg.width; x++) {
            for (int y = 0; y < cfg.height; y++) {
                int count = 0;

                for (auto [nx, ny] : neighbors(x, y)) {
                    if (at(nx, ny).is_mine) {
                        count++;
                    }
                }

                at(x, y).n_mines = count;
            }
        }

        // Check logically solvability
        if (logically_solvable(x, y)) {
            unsolvable = false;
            break;
        }
    }
}

bool rlms::minesweeper::check_won() const {
    for (int x = 0; x < cfg.width; x++) {
        for (int y = 0; y < cfg.height; y++) {
            if (!at(x, y).is_mine && at(x, y).state != cell_state::revealed) {
                return false;
            }
        }
    }

    return true;
}

int rlms::minesweeper::cells_flagged() const {
    int flagged_count = 0;
    for (int x = 0; x < cfg.width; x++) {
        for (int y = 0; y < cfg.height; y++) {
            if (at(x, y).state == cell_state::flagged) {
                flagged_count++;
            }
        }
    }

    return flagged_count;
}

void rlms::minesweeper::reveal(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    if (at(x, y).is_mine) {
        at(x, y).state = cell_state::revealed;
        state          = game_state::lost;
        return;
    }

    // Does not use recursion
    std::queue<std::pair<int, int>> queue;
    queue.emplace(x, y);

    while (!queue.empty()) {
        auto [cx, cy] = queue.front();
        queue.pop();

        // Cell already revealed, or is flagged/question-marked
        if (at(cx, cy).state != cell_state::hidden) {
            continue;
        }

        at(cx, cy).state = cell_state::revealed;

        // Stop expanding if cell has neighboring mines
        if (at(cx, cy).n_mines != 0) {
            continue;
        }

        // Add hidden neighbors to queue
        for (auto [nx, ny] : neighbors(cx, cy)) {
            if (at(nx, ny).state == cell_state::hidden) {
                queue.emplace(nx, ny);
            }
        }
    }
}

void rlms::minesweeper::speed_reveal(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    // Coords of neighboring cells
    std::vector<std::pair<int, int>> marked;
    std::vector<std::pair<int, int>> hidden;

    for (auto [nx, ny] : neighbors(x, y)) {
        if (at(nx, ny).state == cell_state::flagged ||
            at(nx, ny).state == cell_state::qmarked) {
            marked.emplace_back(nx, ny);
        }
        if (at(nx, ny).state == cell_state::hidden) {
            hidden.emplace_back(nx, ny);
        }
    }

    if (marked.size() == at(x, y).n_mines) {
        for (auto [rx, ry] : hidden) {
            reveal(rx, ry);
        }
    }
}

void rlms::minesweeper::toggle(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    if (at(x, y).state == cell_state::hidden) {
        at(x, y).state = cell_state::flagged;
    } else if (at(x, y).state == cell_state::flagged) {
        at(x, y).state = cell_state::qmarked;
    } else if (at(x, y).state == cell_state::qmarked) {
        at(x, y).state = cell_state::hidden;
    }
}

void rlms::minesweeper::speed_flag(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    // Coords of neighboring cells
    std::vector<std::pair<int, int>> hidden;

    for (auto [nx, ny] : neighbors(x, y)) {
        if (at(nx, ny).state != cell_state::revealed) {
            hidden.emplace_back(nx, ny);
        }
    }

    if (hidden.size() == at(x, y).n_mines) {
        for (auto [rx, ry] : hidden) {
            at(rx, ry).state = cell_state::flagged;
        }
    }
}

void rlms::minesweeper::primary_click(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    if (state == game_state::first_click) {
        generate_mines(x, y);
        reveal(x, y);
        state = game_state::playing;

        if (check_won()) {
            state = game_state::won;
        }

        return;
    }

    if (state != game_state::playing) {
        return;
    }

    if (at(x, y).state == cell_state::flagged ||
        at(x, y).state == cell_state::qmarked) {
        return;
    }

    if (at(x, y).is_mine) {
        at(x, y).state = cell_state::revealed;
        state          = game_state::lost;
        return;
    }

    if (at(x, y).state != cell_state::revealed) {
        reveal(x, y);
    } else if (at(x, y).n_mines > 0) {
        speed_reveal(x, y);
    }

    if (check_won()) {
        state = game_state::won;
    }
}

void rlms::minesweeper::secondary_click(int x, int y) {
    if (x < 0 || x >= cfg.width || y < 0 || y >= cfg.height) {
        return;
    }

    if (state != game_state::playing) {
        return;
    }

    if (at(x, y).state != cell_state::revealed) {
        toggle(x, y);
    } else {
        speed_flag(x, y);
    }

    if (check_won()) {
        state = game_state::won;
    }
}

bool rlms::minesweeper::logically_solvable(int x, int y) {
    // Initial reveal
    reveal(x, y);

    // Deduction loop
    // Beware, never nesters
    bool progress = true;
    while (progress) {
        progress = false;

        for (int x = 0; x < cfg.width; x++) {
            for (int y = 0; y < cfg.height; y++) {
                if (at(x, y).state != cell_state::hidden) {
                    continue;
                }

                // Coords of neighboring cells
                std::vector<std::pair<int, int>> flagged;
                std::vector<std::pair<int, int>> hidden;

                for (auto [nx, ny] : neighbors(x, y)) {
                    if (at(nx, ny).state == cell_state::flagged) {
                        flagged.emplace_back(nx, ny);
                    }
                    if (at(nx, ny).state == cell_state::hidden) {
                        hidden.emplace_back(nx, ny);
                    }
                }

                // Rule 1: If the number of neighboring flagged cells equals the
                // number of neighboring mine cells, then all hidden cells are
                // safe to be revealed
                if (flagged.size() == at(x, y).n_mines) {
                    for (auto [hx, hy] : hidden) {
                        reveal(hx, hy);
                        progress = true;
                    }
                }

                // Rule 2: If the number of neighboring flagged cells plus the
                // number of neighboring hidden cells equals the number of
                // neighboring cells that are mine, then all hidden cells are
                // mines.
                if (flagged.size() + hidden.size() == at(x, y).n_mines) {
                    for (auto [hx, hy] : hidden) {
                        at(hx, hy).state = cell_state::flagged;
                        progress         = true;
                    }
                }
            }
        }
    }

    // The board is logically solvable if the algorithm won the game
    bool solved = check_won();

    // Reset the board's cell state.
    for (int x = 0; x < cfg.width; x++) {
        for (int y = 0; y < cfg.height; y++) {
            at(x, y).state = cell_state::hidden;
        }
    }

    return solved;
}