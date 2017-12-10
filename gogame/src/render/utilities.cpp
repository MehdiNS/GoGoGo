#include "utilities.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <nanovg.h>

namespace render {

namespace {
   constexpr float boardOriginFactor = 1.f;
}

float boardOrigin(const DrawContext& context)
{
   return boardOriginFactor * context.squareSizeF();
}

int squareSizeFromCanvasSize(int winWidth, int winHeight, const DrawContext& context)
{
   const auto squareWidth = float(winWidth) /
      std::ceil(float(context.boardWidth() - 1) + boardOriginFactor + context.marginFactor());

   const auto squareHeight = float(winHeight) /
      std::ceil(float(context.boardHeight() - 1) + boardOriginFactor + context.marginFactor());

   return int(std::floor((std::min)(squareWidth, squareHeight)));
}

std::pair<int, int> squarePosFromScreenPos(int windowX, int windowY, const DrawContext& context)
{
   const auto deltaOrigin = int(context.squareSizeF() * boardOriginFactor - context.stoneRadiusF());
   windowX -= deltaOrigin;
   windowY -= deltaOrigin;

   auto x = windowX / context.squareSizeF();
   auto y = windowY / context.squareSizeF();

   auto i = int(std::floor(x));
   auto j = int(std::floor(y));

   if (x - std::floor(x) > 2.f*context.stoneFactor()) i = -1;
   if (y - std::floor(y) > 2.f*context.stoneFactor()) j = -1;

   if (i >= context.boardWidth()) i = -1;
   if (j >= context.boardHeight()) j = -1;

   return std::make_pair(i, j);
}

} // namespace render