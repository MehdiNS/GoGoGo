#pragma once

#include "render/utilities.h"
#include "render/DrawableItem.h"
#include "render/DrawContext.h"

namespace render {

class Stone : public DrawableItem
{
public:
   Stone(int posX, int posY, Player player) : _posX(posX), _posY(posY), _player(player) {}

   virtual void draw(const DrawContext& context) const override;

private:
   int _posX;
   int _posY;
   Player _player;
};

} // namespace render
