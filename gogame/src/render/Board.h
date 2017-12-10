#pragma once

#include "render/DrawableItem.h"
#include "render/DrawContext.h"
#include "render/utilities.h"

namespace render {

class Board : public DrawableItem
{
public:
   Board(int width, int height) : _width(width), _height(height) {}

   virtual void draw(const DrawContext& context) const override;

   void setPickedPosition(int i, int j);
   void setPickColor(PickColor color) { _pickColor = color; }

   int maxStoneCount() const { return _width * _height; }
   int boardWidthInPx(const DrawContext& context) const;

private:
   int _width;
   int _height;

   int _pickI = -1;
   int _pickJ = -1;

   PickColor _pickColor = PickColor::Red;
};

} // namespace render
