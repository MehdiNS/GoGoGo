#pragma once

#include "render/DrawContext.h"

namespace render {

class DrawableItem
{
public:
   virtual ~DrawableItem() {}

   virtual void draw(const DrawContext& context) const = 0;
};

} // namespace render
