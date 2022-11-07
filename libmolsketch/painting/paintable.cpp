#include "paintable.h"

namespace Molsketch {

  QPointF Paintable::getAnchorPoint(const Anchor &anchor) const {
    return Linkable::getAnchorPoint(boundingRect(), anchor);
  }

} // namespace Molsketch
