#pragma once

#include <utility>

#include "DrawContext.h"

namespace render {

enum class Player
{
   White, Black
};

enum class PickColor
{
   Red, Green, Grey, White, Black
};

constexpr NVGcolor color(int r, int g, int b, int alpha) 
{ 
   return NVGcolor{ float(r)/255.f, float(g)/255.f, float(b)/255.f, float(alpha)/255.f }; 
}

constexpr NVGcolor color(int r, int g, int b)
{ 
   return color(r, g, b, 255);
}

constexpr auto stoneColor(Player player)
{
   return (player == Player::White) ? color(242, 242, 242) : color(5, 5, 5);
}

constexpr auto pickColor(PickColor pickColor)
{
   switch (pickColor)
   {
   case PickColor::Red: return color(255, 0, 0, 80);
   case PickColor::Green: return color(0, 255, 0, 80);
   case PickColor::Grey: return color(100, 100, 100, 100);
   case PickColor::White: return color(255, 255, 255, 100);
   case PickColor::Black: return color(0, 0, 0, 100);
   default: return color(0, 0, 0);
   }
}

constexpr NVGcolor boardColor() { return color(192, 150, 102); };
constexpr NVGcolor frameColor() { return color(105, 105, 105); };
constexpr NVGcolor scoreFrameColor() { return color(75, 75, 75); }
constexpr NVGcolor textColor(){ return color(215, 215, 215); }

float boardOrigin(const DrawContext& context);

int squareSizeFromCanvasSize(int winWidth, int winHeight, const DrawContext& context);

std::pair<int, int> squarePosFromScreenPos(int windowX, int windowY, const DrawContext& context);

} // namespace render
