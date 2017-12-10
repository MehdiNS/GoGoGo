#pragma once

#include <vector>

#include "render/DrawContext.h"
#include "render/Board.h"
#include "render/Stone.h"
#include "render/utilities.h"
#include "render/Informations.h"

namespace render {

struct GoModel
{
   GoModel(DrawContext& context) : context(context), board(context.boardWidth(), context.boardHeight())
   { 
      stones.reserve(board.maxStoneCount());
   }

   DrawContext& context;

   Board board;
   std::vector<Stone> stones = {};
   Informations infos = {};
};

void draw(const GoModel& model);

} // namespace render
