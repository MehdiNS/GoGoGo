#include "Board.h"

#include <cassert>

#include <nanovg.h>

#include "utilities.h"

namespace render {

namespace
{
   constexpr float originX = 0.f;
   constexpr float originY = 0.f;

   void drawBoard(int width, int height, const DrawContext& context)
   {
      auto vg = &context.vgContext();
      const auto squareSize = context.squareSize();
      const auto margin = context.margin();

      // first point is (0,0), so width - 1, height - 1
      auto hSize = float((width - 1) * squareSize + 2 * margin);
      auto vSize = float((height - 1) * squareSize + 2 * margin);

      nvgBeginPath(vg);
      nvgRect(vg, float(originX - margin), float(originY - margin), hSize, vSize);
      nvgFillColor(vg, boardColor());
      nvgFill(vg);
   }

   void drawLines(int width, int height, const DrawContext& context)
   {
      auto vg = &context.vgContext();
      const auto squareSize = context.squareSize();

      const auto lineColor = nvgRGBA(27, 27, 27, 255);
      const auto lineWidth = 2.f;

      nvgBeginPath(vg);

      // first point is (0,0), so width - 1
      const float ySize = float((height - 1) * squareSize) - originY + lineWidth;
      for (int i = 0; i < width; ++i)
      {
         const float x = float(i* squareSize);

         nvgRect(vg, x, originY, lineWidth, ySize);
      }

      // see ySize comment
      const float xSize = float((width - 1) * squareSize) - originX;
      for (int j = 0; j < height; ++j)
      {
         const auto y = float(j * squareSize);
         nvgRect(vg, originX, y, xSize, lineWidth);
      }

      nvgFillColor(vg, lineColor);
      nvgFill(vg);
   }

   void drawPick(int i, int j, PickColor color, const DrawContext& context)
   {
      if (i < 0 || j < 0)
         return;

      auto vg = &context.vgContext();
      const auto squareSize = context.squareSize();
      const auto stoneRadius = context.stoneRadiusF();
      const auto lineColor = pickColor(color);

      auto x = float(i * squareSize) - stoneRadius;
      auto y = float(j * squareSize) - stoneRadius;
      auto size = 2.f * stoneRadius;

      nvgBeginPath(vg);
      nvgRoundedRect(vg, x, y, size, size, stoneRadius*.2f);
      nvgFillColor(vg, lineColor);
      nvgFill(vg);
   }
}


void Board::draw(const DrawContext& context) const
{
   drawBoard(_width, _height, context);
   drawLines(_width, _height, context);

   drawPick(_pickI, _pickJ, _pickColor, context);
}

void Board::setPickedPosition(int i, int j)
{
   assert(i < _width && j < _height);

   _pickI = i;
   _pickJ = j;
}

int Board::boardWidthInPx(const DrawContext& context) const
{
   const auto squareSize = context.squareSize();
   const auto margin = context.margin();

   // first point is (0,0), so width - 1, height - 1
   auto hSize = (_width - 1) * squareSize + 2 * margin;

   return hSize;
}

} // namespace render