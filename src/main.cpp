/// @file
/// @copyright (c) 2025 Anstro Pleuton
/// This project is released under the Public Domain or licensed under the terms of MIT license.

#include "raylib.h"
#include "raymath.h"
#include "rlms.hpp"
#include "rlmsg.hpp"

using namespace rlms;
using namespace rlmsg;

/// Shrink a rectangle from all sides.
Rectangle ShrinkRec(Rectangle rec, float shrink) {
    return {rec.x + shrink, rec.y + shrink, rec.width - shrink * 2.0f, rec.height - shrink * 2.0f};
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(512, 512, "RLMS - raylib minesweeper");
    SetTargetFPS(60);

    LoadResources();

    const float bevelThick     = 5.0f;
    const float cellBevelThick = 2.0f;
    const float panelHeight    = 63.0f - bevelThick;
    const float statusHeight   = 63.0f - bevelThick;

    // This config is used to display config on screen and allow users to edit it.
    // The actual current board config is in ms.cfg.
    config cfg = {
        .width  = 10,
        .height = 10,
        .mines  = 10,
    };

    minesweeper ms;

    float time = 0.0f;
    ms.cfg     = cfg;
    ms.cfg.randomize_seed();
    ms.reset();

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (IsKeyPressed(KEY_SPACE)) isDarkTheme = !isDarkTheme;

        // Shorthands
        const Rectangle screen    = {0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()};
        const Vector2   mouse     = GetMousePosition();
        const bool      leftHeld  = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        const bool      rightHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        const bool      held      = leftHeld || rightHeld;
        const bool      leftRel   = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        const bool      rightRel  = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);

        // Main box
        const Rectangle screenBox  = screen;
        const Rectangle screenArea = ShrinkRec(screenBox, bevelThick);
        DrawBeveledRectangle(screenBox, bevelThick);

        // Top panel
        const Rectangle panelBox  = {bevelThick, bevelThick, screenBox.width - bevelThick * 2.0f, panelHeight};
        const Rectangle panelArea = ShrinkRec(panelBox, bevelThick);
        DrawBeveledRectangleInv(panelBox, bevelThick);

        // Score display
        const int     score         = ms.cells_flagged();
        const char   *scoreText     = TextFormat("%03d", score);
        const Vector2 scorePosition = {panelArea.x, panelArea.y};
        DrawLEDText(scoreText, scorePosition, 48.0f);

        // Time display
        if (ms.state == game_state::playing) time += GetFrameTime();
        const int     timer         = time;
        const char   *timerText     = TextFormat("%03d", timer);
        const Vector2 timerPosition = {panelArea.x + panelArea.width - MeasureLEDText(timerText, 48.0f).x, panelArea.y};
        DrawLEDText(timerText, timerPosition, 48.0f);

        // Smiley - The personal judger
        const Rectangle smileyBox  = {(GetScreenWidth() - 48.0f) / 2.0f, panelArea.y, 48.0f, 48.0f};
        const Rectangle smileyArea = ShrinkRec(smileyBox, bevelThick);

        const Texture texture =
            ms.state == game_state::lost
                ? faceLost
            : ms.state == game_state::won
                ? faceWon
            : held
                ? faceClicking
                : face;

        if (CheckCollisionPointRec(mouse, smileyBox) && leftRel) {
            time   = 0.0f;
            ms.cfg = cfg;
            ms.cfg.randomize_seed();
            ms.reset();
        }

        const bool gameOver = ms.state == game_state::won || ms.state == game_state::lost;

        if (CheckCollisionPointRec(mouse, smileyBox) && leftHeld)
            DrawBeveledRectangleInv(smileyBox, bevelThick);
        else
            DrawBeveledRectangle(smileyBox, bevelThick);
        DrawTextureDest(texture, smileyArea);

        // Bottom panel
        const Rectangle statusBox  = {bevelThick, screenBox.height - statusHeight - bevelThick, screenBox.width - bevelThick * 2.0f, statusHeight};
        const Rectangle statusArea = ShrinkRec(statusBox, bevelThick);
        DrawBeveledRectangleInv(statusBox, bevelThick);

        // Show editable config
        const Rectangle widthBox = {statusArea.x + 0.0f * statusArea.width / 3.0f, statusArea.y, statusArea.width / 3.0f, statusArea.height};
        if (CheckCollisionPointRec(mouse, widthBox)) cfg.width += GetMouseWheelMove();
        DrawTextCentered(font24, TextFormat("Width: %d", cfg.width), widthBox, 24.0f, 1.0f, isDarkTheme ? textDark : textLight);
        const Rectangle heightBox = {statusArea.x + 1.0f * statusArea.width / 3.0f, statusArea.y, statusArea.width / 3.0f, statusArea.height};
        if (CheckCollisionPointRec(mouse, heightBox)) cfg.height += GetMouseWheelMove();
        DrawTextCentered(font24, TextFormat("Height: %d", cfg.height), heightBox, 24.0f, 1.0f, isDarkTheme ? textDark : textLight);
        const Rectangle minesBox = {statusArea.x + 2.0f * statusArea.width / 3.0f, statusArea.y, statusArea.width / 3.0f, statusArea.height};
        if (CheckCollisionPointRec(mouse, minesBox)) cfg.mines += GetMouseWheelMove();
        DrawTextCentered(font24, TextFormat("Mines: %d", cfg.mines), minesBox, 24.0f, 1.0f, isDarkTheme ? textDark : textLight);

        // Board box
        const Rectangle boardBox  = {screenArea.x, screenArea.y + panelBox.height, screenArea.width, screenArea.height - panelBox.height - statusBox.height};
        const Rectangle boardArea = ShrinkRec(boardBox, bevelThick);
        DrawBeveledRectangleInv(boardBox, bevelThick);

        // Render grid
        const Vector2 cellSizeV = {boardArea.width / ms.cfg.width, boardArea.height / ms.cfg.height};
        const float   cellSize  = std::min(cellSizeV.x, cellSizeV.y);
        const Vector2 padding   = cellSizeV.x > cellSizeV.y ? Vector2{(cellSizeV.x - cellSizeV.y) * ms.cfg.width / 2.0f, 0.0f} : Vector2{0.0f, (cellSizeV.y - cellSizeV.x) * ms.cfg.height / 2.0f};
        const int     mCellX    = std::floor((mouse.x - boardArea.x - padding.x) / cellSize);
        const int     mCellY    = std::floor((mouse.y - boardArea.y - padding.y) / cellSize);

        for (int x = 0; x < ms.cfg.width; x++) {
            for (int y = 0; y < ms.cfg.height; y++) {
                const Rectangle cellBox    = {boardArea.x + padding.x + cellSize * x, boardArea.y + padding.y + cellSize * y, cellSize, cellSize};
                const Rectangle cellArea   = ShrinkRec(cellBox, cellBevelThick);
                const Rectangle cellArea24 = {cellArea.x + (cellArea.width - 32.0f) / 2.0f, cellArea.y + (cellArea.height - 32.0f) / 2.0f, 32.0f, 32.0f};
                const cell      cell       = ms.at(x, y);
                const bool      colliding  = mCellX == x && mCellY == y;

                // Click actions
                if (colliding && leftRel) {
                    ms.primary_click(x, y);
                }

                if (colliding && rightRel) {
                    ms.secondary_click(x, y);
                }

                bool collidingNeighbors = false;
                for (auto [nx, ny] : ms.neighbors(x, y)) {
                    if (mCellX == nx && mCellY == ny && ms.at(nx, ny).state == cell_state::revealed) {
                        collidingNeighbors = true;
                        break;
                    }
                }

                // Render cell
                if (cell.state != cell_state::revealed) {
                    if (!gameOver && held && cell.state == cell_state::hidden && (collidingNeighbors || colliding))
                        DrawBeveledRectangleInv(cellBox, cellBevelThick);
                    else
                        DrawBeveledRectangle(cellBox, cellBevelThick);

                    if (cell.state == cell_state::flagged) {
                        DrawTextureDest(flag, cellArea24);
                        if (gameOver && !cell.is_mine) {
                            DrawTextureDest(crossMark, cellArea24);
                        }
                    }
                    if (cell.state == cell_state::qmarked) {
                        DrawTextCentered(font32, "?", cellArea, 32.0f, 0.0f, isDarkTheme ? textDark : textLight);
                    }

                    if (gameOver && cell.state == cell_state::hidden && cell.is_mine) {
                        DrawTextureDest(bomb, cellArea24);
                    }
                } else {
                    if (cell.n_mines != 0) {
                        DrawTextCentered(font32, TextFormat("%d", cell.n_mines), cellArea, 32.0f, 0.0f, GetMineNumberColor(cell.n_mines));
                    }
                    if (cell.is_mine) {
                        DrawRectangleRec(cellArea, incorrect);
                        DrawTextureDest(bomb, cellArea24);
                    }
                }
            }
        }

        EndDrawing();
    }

    UnloadResources();

    CloseWindow();
}
