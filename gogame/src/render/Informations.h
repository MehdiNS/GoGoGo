#pragma once

#include <string>

#include <render/DrawableItem.h>
#include <render/utilities.h>

namespace render
{

class Informations : public DrawableItem
{
public:
   Informations() {}

   void setScore(int blackScore, int whiteScore) { _whiteScore = whiteScore; _blackScore = blackScore; }
   void setPlayer(Player player) { _player = player; }
   void setMessage(const std::string& message) { _message = message; }

   virtual void draw(const DrawContext& context) const override;

private:
   int _whiteScore = 0;
   int _blackScore = 0;
   Player _player = Player::Black;
   std::string _message = "";
};

} // namespace render
