#ifndef MOLSKETCH_DEBUGGABLE_H
#define MOLSKETCH_DEBUGGABLE_H

class QDebug;

namespace Molsketch {

  class Debuggable
  {
  protected:
    virtual QDebug debug(QDebug debug) const = 0;
  public:
    QDebug printDebug(QDebug debug) const;
  };

  QDebug operator<<(QDebug debug, const Debuggable *debuggable);

} // namespace Molsketch

#endif // MOLSKETCH_DEBUGGABLE_H
