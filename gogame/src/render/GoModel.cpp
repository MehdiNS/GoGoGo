#include "GoModel.h"

namespace render {

void draw(const GoModel& model)
{
   const auto& context = model.context;
   auto vg = &context.vgContext();
   nvgResetTransform(vg);

   auto origin = boardOrigin(context);
   nvgTranslate(vg, origin, origin);

   model.board.draw(context);

   for (auto&& stone : model.stones)
      stone.draw(context);

   nvgTranslate(vg, -origin, -origin);

   const auto tx = float(origin + model.board.boardWidthInPx(context) + 10);
   const auto ty = float(origin - context.margin());
   nvgTranslate(&context.vgContext(), tx, ty);
   model.infos.draw(context);
   nvgTranslate(&context.vgContext(), -tx, -ty);
}

} // namespace render
