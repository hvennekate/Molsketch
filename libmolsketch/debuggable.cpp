#include "debuggable.h"

#include <QDebug>

namespace Molsketch {


QDebug operator<<(QDebug debug, const Debuggable *debuggable) {
    if (!debuggable) return debug << "null";
    debuggable->printDebug(debug.nospace());
    return debug.resetFormat();
}

QDebug Debuggable::printDebug(QDebug debug) const {
    return this->debug(debug); // TODO check if necessary or if "friend" declaration is enough
}

} // namespace Molsketch
