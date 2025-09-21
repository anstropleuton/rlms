/// @file
/// @copyright (c) 2025 Anstro Pleuton
/// This project is relesed under the Public Domain or licensed under the terms of MIT license.

#pragma once

#include <string>

#include "raylib.h"

namespace rlmsg {

/// Get color from HSLA values.
Color ColorFromHSLA(float hue, float saturation, float lightness, float alpha = 1.0f);

/// Draw text centered in the bounds.
void DrawTextCentered(Font font, const char *text, Rectangle bounds, float fontSize, float spacing, Color tint);

/// Draw texture with only destination rectangle.
void DrawTextureDest(Texture texture, Rectangle dest, Color tint = WHITE);

// Tile colors
// Glare is the top/left color
// Shade is the bottom/right color

inline const Color tileGlareLight = ColorFromHSLA(0.0f, 0.0f, 1.0f);
inline const Color tileLight      = ColorFromHSLA(0.0f, 0.0f, 0.75f);
inline const Color tileShadeLight = ColorFromHSLA(0.0f, 0.0f, 0.5f);

inline const Color tileGlareDark = ColorFromHSLA(0.0f, 0.0f, 0.5f);
inline const Color tileDark      = ColorFromHSLA(0.0f, 0.0f, 0.25f);
inline const Color tileShadeDark = ColorFromHSLA(0.0f, 0.0f, 0.0f);

// Tile number colors

inline const Color color1Light = ColorFromHSLA(240.0f, 1.0f, 0.5f);
inline const Color color2Light = ColorFromHSLA(120.0f, 1.0f, 0.5f);
inline const Color color3Light = ColorFromHSLA(000.0f, 1.0f, 0.5f);
inline const Color color4Light = ColorFromHSLA(240.0f, 1.0f, 0.25f);
inline const Color color5Light = ColorFromHSLA(120.0f, 1.0f, 0.25f);
inline const Color color6Light = ColorFromHSLA(000.0f, 1.0f, 0.25f);
inline const Color color7Light = ColorFromHSLA(0.0f, 0.0f, 0.25f);
inline const Color color8Light = ColorFromHSLA(0.0f, 0.0f, 0.375f);

inline const Color color1Dark = ColorFromHSLA(240.0f, 1.0f, 0.5f);
inline const Color color2Dark = ColorFromHSLA(120.0f, 1.0f, 0.5f);
inline const Color color3Dark = ColorFromHSLA(000.0f, 1.0f, 0.5f);
inline const Color color4Dark = ColorFromHSLA(240.0f, 1.0f, 0.75f);
inline const Color color5Dark = ColorFromHSLA(120.0f, 1.0f, 0.75f);
inline const Color color6Dark = ColorFromHSLA(000.0f, 1.0f, 0.75f);
inline const Color color7Dark = ColorFromHSLA(0.0f, 0.0f, 0.875f);
inline const Color color8Dark = ColorFromHSLA(0.0f, 0.0f, 0.75f);

Color GetMineNumberColor(int number);

// Regular text colors

inline const Color textLight = ColorFromHSLA(0.0f, 1.0f, 0.0f);
inline const Color textDark  = ColorFromHSLA(0.0f, 1.0f, 1.0f);

// LED-display style text colors

inline const Color ledLight   = ColorFromHSLA(0.0f, 1.0f, 0.5f);
inline const Color ledBgLight = ColorFromHSLA(0.0f, 1.0f, 0.25f);
inline const Color ledDark    = ColorFromHSLA(0.0f, 1.0f, 0.5f);
inline const Color ledBgDark  = ColorFromHSLA(0.0f, 1.0f, 0.25f);

// Incorrect cell color
inline const Color incorrect = ColorFromHSLA(0.0f, 1.0f, 0.5f);

extern bool isDarkTheme; ///< Whether the GUI is dark themed.

extern Font font24; ///< Font (size 24) for all UI text (including cell numbers, led text, etc.).
extern Font font32; ///< Font (size 32) for all UI text (including cell numbers, led text, etc.).
extern Font font48; ///< Font (size 48) for all UI text (including cell numbers, led text, etc.).

// Face emojis

extern Texture face;
extern Texture faceClicking;
extern Texture faceLost;
extern Texture faceWon;
extern Texture bomb;
extern Texture flag;
extern Texture crossMark;

void LoadResources();

void UnloadResources();

/// Draw beveled rectangle using scheme.
void DrawBeveledRectangle(Rectangle rec, float thickness);

/// Draw beveled rectangle with swapped glare and shade bevel color.
void DrawBeveledRectangleInv(Rectangle rec, float thickness);

/// Draw beveled rectangle with customizable colors.
void DrawBeveledRectanglePro(Rectangle rec, float thickness, Color glare, Color mid, Color shade);

void DrawLEDText(const std::string &text, Vector2 position, float fontSize);

Vector2 MeasureLEDText(const std::string &text, float fontSize);

} // namespace rlmsg
