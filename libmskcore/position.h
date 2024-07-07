#ifndef POSITION_H
#define POSITION_H

#include <vector>
#include <utility>

namespace Molsketch {
namespace Core {

class Position;
typedef std::vector<Position> Coordinates;
typedef std::pair<Position, Position> BoundingRect;

class Position
{
    double x, y;
public:
    Position(const double &x = 0, const double &y = 0);
    double getX() const;
    double getY() const;
    static BoundingRect boundingRect(const Coordinates &coords);
    static Position center(const Coordinates &coords);
    Position operator +(const Position &other) const;
    Position operator -(const Position &other) const;
    Position operator *(const double &factor) const;
    Position& operator +=(const Position &other);
};

Position operator*(const double &factor, const Position &pos);

} // namespace Core
} // namespace Molsketch

#endif // POSITION_H
