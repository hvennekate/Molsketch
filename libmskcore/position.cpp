#include "position.h"


Molsketch::Core::Position::Position(const double &x, const double &y)
    : x(x), y(y)
{}

double Molsketch::Core::Position::getX() const {
    return x;
}

double Molsketch::Core::Position::getY() const {
    return y;
}

Molsketch::Core::BoundingRect Molsketch::Core::Position::boundingRect(const Coordinates &coords)
{
    double left = 0, right = 0, top = 0, bottom = 0;
    for (auto coord : coords) {
        left = std::min(left, coord.x);
        right = std::max(right, coord.x);
        top = std::min(top, coord.y);
        bottom = std::max(bottom, coord.y);
    }

    return std::make_pair(Position(left, top), Position(right, bottom));
}

Molsketch::Core::Position Molsketch::Core::Position::center(const Coordinates &coords)
{
    auto bounds = boundingRect(coords);
    return Position((bounds.first.x + bounds.second.x)/2., (bounds.first.y + bounds.second.y)/2.);
}

Molsketch::Core::Position Molsketch::Core::Position::operator +(const Position &other) const
{
    return Position(x + other.x, y + other.y);
}

Molsketch::Core::Position Molsketch::Core::Position::operator -(const Position &other) const
{
  return Position(x - other.x, y - other.y);
}

Molsketch::Core::Position Molsketch::Core::Position::operator *(const double &factor) const
{
    return Position(x * factor, y * factor);
}

Molsketch::Core::Position &Molsketch::Core::Position::operator +=(const Position &other)
{
  x += other.getX();
  y += other.getY();
  return *this;
}

bool Molsketch::Core::Position::operator ==(const Position &other) const
{
  return x == other.x && y == other.y;
}

Molsketch::Core::Position Molsketch::Core::operator*(const double &factor, const Position &pos)
{
    return pos * factor;
}
