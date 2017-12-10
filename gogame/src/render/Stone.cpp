#include "stone.h"

#include <nanovg.h>

#include "utilities.h"

namespace render {

namespace
{
   void drawStone(int i, int j, Player player, const DrawContext& context)
   {
      auto vg = &context.vgContext();
      const auto squareSize = context.squareSize();

      const auto x = float(squareSize * i);
      const auto y = float(squareSize * j);

      const auto r = context.stoneRadiusF();

      const float dx = .1f;
      const float dy = .15f;
      const auto shadowColor = nvgLinearGradient(vg, x - r*(1.f-dx), y - r*(1.f-dy), 
         x+r*(1.f+dx), y+r*(1.f+dy), nvgRGBA(32, 32, 32, 255), nvgRGBA(32, 32, 32, 32));
      nvgBeginPath(vg);
      nvgCircle(vg, x+r*dx, y+r*dy, r);
      nvgFillPaint(vg, shadowColor);
      nvgFill(vg);

      nvgBeginPath(vg);
      nvgCircle(vg, x, y, r);
      nvgFillColor(vg, stoneColor(player));
      nvgFill(vg);

      const unsigned char c = (player == Player::White) ? 0 : 192;
      const auto inGradientColor = nvgLinearGradient(vg, x - r*(1.f - dx), y - r*(1.f - dy),
         x + r*(1.f + dx), y + r*(1.f + dy), nvgRGBA(c, c, c, 60), nvgRGBA(c, c, c, 0));
      nvgBeginPath(vg);
      nvgCircle(vg, x, y, r);
      nvgFillPaint(vg, inGradientColor);
      nvgFill(vg);


      const unsigned char c2 = (player == Player::White) ? 255 : 128;
      const auto specularColor = nvgRadialGradient(vg, x - r*0.5f, y - r*0.35f, r*0.02f, r*0.9f,
         nvgRGBA(c2, c2, c2, 227), nvgRGBA(c2, c2, c2, 0));

      nvgBeginPath(vg);
      nvgCircle(vg, x, y, r);
      nvgFillPaint(vg, specularColor);
      nvgFill(vg);
   }
}


void Stone::draw(const DrawContext& context) const
{
   drawStone(_posX, _posY, _player, context);   
}

} // namespace render