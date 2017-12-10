#include "Informations.h"

#include <string>
#include <utility>

#include "render/stone.h"

namespace render
{

namespace
{

constexpr std::pair<float, float> frameSize()
{
   return std::make_pair(180.f, 95.f);
}

constexpr float margin = 10.f;
constexpr float fontSize = 20.f;

void drawFrame(const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto size = frameSize();

   nvgBeginPath(vg);
   nvgRoundedRect(vg, 0, 0, size.first, size.second, size.first * .05f);
   nvgFillColor(vg, frameColor());
   nvgFill(vg);
}

void drawStone(Player player, DrawContext context)
{
   auto vg = &context.vgContext();
   context.setSquareSize(40); // context is passed-by-copy in order to modify it...

   constexpr auto tx = 70.f;
   const auto ty = context.stoneRadiusF() + margin * .5f;

   nvgTranslate(vg, tx, ty);
   Stone stone(0, 0, player);
   stone.draw(context);
   nvgTranslate(vg, -tx, -ty);
}

void drawNextPlayer(Player player, const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto tx = margin;
   constexpr auto ty = margin;

   nvgFontSize(vg, fontSize);
   nvgFontFace(vg, "bold");
   nvgFillColor(vg, textColor());
   nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
   nvgText(vg, tx, ty, "Next", NULL);

   drawStone(player, context);
}

float drawScoreFrame(const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto size = frameSize();
   constexpr auto frameWidth = size.first - margin * 2.f;
   constexpr auto frameHeight = size.second * .55f - margin * .8f;

   nvgBeginPath(vg);
   nvgRoundedRect(vg, 0.f, 0.f, frameWidth, frameHeight, size.first * .04f);
   nvgFillColor(vg, scoreFrameColor());
   nvgFill(vg);

   return frameWidth;
}

void drawScoreText(int scoreB, int scoreW, float frameWidth, const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto ty = margin * .3f;

   nvgFontSize(vg, fontSize);
   nvgFontFace(vg, "bold");
   nvgFillColor(vg, textColor());
   nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);

   nvgText(vg, frameWidth * .25f, ty, "Black", NULL);
   nvgText(vg, frameWidth * .75f, ty, "White", NULL);

   nvgText(vg, frameWidth * .25f, ty + fontSize, std::to_string(scoreB).data(), NULL);
   nvgText(vg, frameWidth * .75f, ty + fontSize, std::to_string(scoreW).data(), NULL);
}

void drawScore(int scoreB, int scoreW, const DrawContext& context)
{
   auto vg = &context.vgContext();
   const auto size = frameSize();
   const auto tx = margin;
   const auto ty = size.second * .45f;

   nvgTranslate(vg, tx, ty);
   auto frameWidth = drawScoreFrame(context);
   drawScoreText(scoreB, scoreW, frameWidth, context);
   nvgTranslate(vg, -tx, -ty);
}

void drawMessageStringFrame(const std::string& message, float parentW, float parentH,
   const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto size = frameSize();
   const auto frameWidth = parentW - margin * 2.f;
   const auto frameHeight = parentH - fontSize - margin * 2.f;

   nvgBeginPath(vg);
   nvgRoundedRect(vg, 0.f, 0.f, frameWidth, frameHeight, size.first * .04f);
   nvgFillColor(vg, scoreFrameColor());
   nvgFill(vg);

   nvgFontSize(vg, 18.0f);
   nvgFontFace(vg, "sans");
   nvgFillColor(vg, textColor());
   nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

   nvgTextBox(vg, margin, margin*.3f, frameWidth - 2.f*margin, message.data(), nullptr);
}

void drawMessageFrame(const std::string& message, const DrawContext& context)
{
   auto vg = &context.vgContext();
   constexpr auto size = frameSize();
   constexpr auto frameWidth = size.first * 1.5f;
   constexpr auto frameHeight = size.second * 1.8f;

   constexpr auto frameX = 0.f;
   constexpr auto frameY = size.second + margin;

   nvgBeginPath(vg);
   nvgRoundedRect(vg, frameX, frameY, frameWidth, frameHeight, size.first * .05f);
   nvgFillColor(vg, frameColor());
   nvgFill(vg);

   nvgFontSize(vg, fontSize);
   nvgFontFace(vg, "bold");
   nvgFillColor(vg, textColor());
   nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

   nvgText(vg, frameX + margin, frameY + margin*.5f, "Message", nullptr);

   constexpr auto tx = frameX + margin;
   constexpr auto ty = frameY + fontSize + margin;

   nvgTranslate(vg, tx, ty);
   drawMessageStringFrame(message, frameWidth, frameHeight, context);
   nvgTranslate(vg, -tx, -ty);
}

} // namespace

void Informations::draw(const DrawContext& context) const
{
   drawFrame(context);
   drawNextPlayer(_player, context);
   drawScore(_blackScore, _whiteScore, context);

   drawMessageFrame(_message, context);
}

} // namespace render