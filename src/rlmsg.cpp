/// @file
/// @copyright (c) 2025 Anstro Pleuton
/// This project is relesed under the Public Domain or licensed under the terms of MIT license.

#include <algorithm>
#include <cmath>

#include "rlmsg.hpp"

bool    rlmsg::isDarkTheme;
Font    rlmsg::font24;
Font    rlmsg::font32;
Font    rlmsg::font48;
Texture rlmsg::face;
Texture rlmsg::faceClicking;
Texture rlmsg::faceLost;
Texture rlmsg::faceWon;
Texture rlmsg::bomb;
Texture rlmsg::flag;
Texture rlmsg::crossMark;

Color rlmsg::ColorFromHSLA(float hue, float saturation, float lightness, float alpha) {
    hue        = std::fmod(std::fmod(hue, 360.0f) + 360.0f, 360.0f);
    saturation = std::clamp(saturation, 0.0f, 1.0f);
    lightness  = std::clamp(lightness, 0.0f, 1.0f);
    alpha      = std::clamp(alpha, 0.0f, 1.0f);

    float c = (1.0f - std::fabs(2.0f * lightness - 1.0f)) * saturation;
    float x = c * (1.0f - std::fabs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
    float m = lightness - c / 2.0f;
    float r = 0, g = 0, b = 0;

    if (hue < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (hue < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (hue < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (hue < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (hue < 300) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }

    Color color;
    color.r = (r + m) * 255;
    color.g = (g + m) * 255;
    color.b = (b + m) * 255;
    color.a = alpha * 255;
    return color;
}

void rlmsg::DrawTextCentered(Font font, const char *text, Rectangle bounds, float fontSize, float spacing, Color tint) {
    Vector2 m = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text, {bounds.x + std::floor((bounds.width - m.x) / 2.0f), bounds.y + std::floor((bounds.height - m.y) / 2.0f)}, fontSize, spacing, tint);
}

void rlmsg::DrawTextureDest(Texture texture, Rectangle dest, Color tint) {
    DrawTexturePro(texture, {0.0f, 0.0f, (float)texture.width, (float)texture.height}, dest, {}, 0.0f, tint);
}

Color rlmsg::GetMineNumberColor(int number) {
    switch (number) {
    case 1: return isDarkTheme ? color1Dark : color1Light;
    case 2: return isDarkTheme ? color2Dark : color2Light;
    case 3: return isDarkTheme ? color3Dark : color3Light;
    case 4: return isDarkTheme ? color4Dark : color4Light;
    case 5: return isDarkTheme ? color5Dark : color5Light;
    case 6: return isDarkTheme ? color6Dark : color6Light;
    case 7: return isDarkTheme ? color7Dark : color7Light;
    case 8: return isDarkTheme ? color8Dark : color8Light;
    }
    return BLANK;
}

void rlmsg::LoadResources() {
    font24         = LoadFontEx("res/Bungee.ttf", 24, nullptr, 0);
    font32         = LoadFontEx("res/Bungee.ttf", 32, nullptr, 0);
    font48         = LoadFontEx("res/Bungee.ttf", 48, nullptr, 0);
    face         = LoadTexture("res/face.png");
    faceClicking = LoadTexture("res/face_clicking.png");
    faceLost     = LoadTexture("res/face_lost.png");
    faceWon      = LoadTexture("res/face_won.png");
    bomb         = LoadTexture("res/bomb.png");
    flag         = LoadTexture("res/flag.png");
    crossMark    = LoadTexture("res/cross_mark.png");
}

void rlmsg::UnloadResources() {
    UnloadFont(font24);
    UnloadFont(font32);
    UnloadFont(font48);
    UnloadTexture(face);
    UnloadTexture(faceClicking);
    UnloadTexture(faceLost);
    UnloadTexture(faceWon);
    UnloadTexture(bomb);
    UnloadTexture(flag);
    UnloadTexture(crossMark);
}

void rlmsg::DrawBeveledRectangle(Rectangle rec, float thickness) {
    DrawBeveledRectanglePro(rec, thickness, isDarkTheme ? tileGlareDark : tileGlareLight, isDarkTheme ? tileDark : tileLight, isDarkTheme ? tileShadeDark : tileShadeLight);
}

void rlmsg::DrawBeveledRectangleInv(Rectangle rec, float thickness) {
    DrawBeveledRectanglePro(rec, thickness, isDarkTheme ? tileShadeDark : tileShadeLight, isDarkTheme ? tileDark : tileLight, isDarkTheme ? tileGlareDark : tileGlareLight);
}

void rlmsg::DrawBeveledRectanglePro(Rectangle rec, float thickness, Color glare, Color mid, Color shade) {
    DrawRectangleRec({rec.x, rec.y, rec.width - thickness, rec.height - thickness}, glare);
    DrawTriangle({rec.x + rec.width, rec.y}, {rec.x + rec.width - thickness, rec.y}, {rec.x + rec.width - thickness, rec.y + thickness}, glare);
    DrawTriangle({rec.x + rec.width - thickness, rec.y + thickness}, {rec.x + rec.width, rec.y + thickness}, {rec.x + rec.width, rec.y}, shade);
    DrawRectangleRec({rec.x + thickness, rec.y + thickness, rec.width - thickness, rec.height - thickness}, shade);
    DrawTriangle({rec.x, rec.y + rec.height}, {rec.x + thickness, rec.y + rec.height}, {rec.x + thickness, rec.y + rec.height - thickness}, shade);
    DrawTriangle({rec.x + thickness, rec.y + rec.height - thickness}, {rec.x, rec.y + rec.height - thickness}, {rec.x, rec.y + rec.height}, glare);
    DrawRectangleRec({rec.x + thickness, rec.y + thickness, rec.width - thickness * 2.0f, rec.height - thickness * 2.0f}, mid);
}

void rlmsg::DrawLEDText(const std::string &text, Vector2 position, float fontSize) {
    float width = MeasureTextEx(font48, "M", fontSize, 0.0f).x;
    for (std::size_t i = 0; i < text.size(); i++) {
        const char      c      = text[i];
        const Rectangle bounds = {position.x + i * width, position.y, width, fontSize};
        DrawRectangleRec(bounds, isDarkTheme ? ledBgDark : ledBgLight);
        DrawTextCentered(font48, TextFormat("%c", c), bounds, fontSize, 0.0f, isDarkTheme ? ledDark : ledLight);
    }
}

Vector2 rlmsg::MeasureLEDText(const std::string &text, float fontSize) {
    return {MeasureTextEx(font48, "M", fontSize, 0.0f).x * text.size(), fontSize};
}
