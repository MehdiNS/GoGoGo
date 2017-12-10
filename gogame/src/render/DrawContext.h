#pragma once

#include <cmath>

#include <nanovg.h>

namespace render {

class DrawContext
{
public:
   DrawContext(NVGcontext& context, int boardWidth, int boardHeight)
      : _vgContext(context), _boardWidth(boardWidth), _boardHeight(boardHeight) {}

   NVGcontext& vgContext() const { return _vgContext; }

   int squareSize() const { return _squareSize; }
   float squareSizeF() const { return float(squareSize()); }
   void setSquareSize(int value) { _squareSize = value; }

   int margin() const { return int(std::ceil(squareSizeF() * marginFactor())); };
   static constexpr float marginFactor() { return .5f; }

   float stoneRadiusF() const { return float(squareSize()) * stoneFactor(); };
   static constexpr float stoneFactor() { return .36f; }

   int boardWidth() const { return _boardWidth; }
   int boardHeight() const { return _boardHeight; }

private:
   NVGcontext& _vgContext;

   int _squareSize = 50;
   int _boardWidth;
   int _boardHeight;
};

} // namespace render
