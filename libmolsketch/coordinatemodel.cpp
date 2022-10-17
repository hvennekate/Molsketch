/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "coordinatemodel.h"

namespace Molsketch {

  struct CoordinateModel::privateData
  {
    QPolygonF coordinates;
    bool rowPermissible(const int& value) { return (0 <= value && value < coordinates.size()); }
    bool rowPermissible(const QModelIndex& index) { return rowPermissible(index.row()); }
    bool rowPermissibleWithEnd(const int& value) { return (0 <= value && value <= coordinates.size()); }
    bool rowPermissibleWithEnd(const QModelIndex& index) { return rowPermissibleWithEnd(index.row());}
    bool columnPermissible(const int& value) { return (0 <= value && value < 2);}
    bool columnPermissible(const QModelIndex& index) { return columnPermissible(index.column()); }
    bool indexPermissible(const QModelIndex& index) { return rowPermissible(index) && columnPermissible(index); }
  };

  CoordinateModel::CoordinateModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new privateData)
  {
  }

  CoordinateModel::~CoordinateModel()
  {
    delete d;
  }

  int CoordinateModel::rowCount(const QModelIndex &parent) const
  {
    Q_UNUSED(parent)
    return d->coordinates.size();
  }

  int CoordinateModel::columnCount(const QModelIndex &parent) const
  {
    Q_UNUSED(parent)
    return 2;
  }

  QVariant CoordinateModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (Qt::Vertical == orientation) return QVariant();
    if (Qt::DisplayRole != role) return QVariant();
    if (0 == section) return "x";
    if (1 == section) return "y";
    return QVariant();
  }

  QVariant CoordinateModel::data(const QModelIndex &index, int role) const
  {
    if (Qt::DisplayRole != role && Qt::EditRole != role) return QVariant();
    if (!d->indexPermissible(index)) return QVariant();
    if (index.column() == 0) return d->coordinates.at(index.row()).x();
    return d->coordinates.at(index.row()).y();
  }

  bool CoordinateModel::setData(const QModelIndex &index, const QVariant &value, int role)
  {
    if (Qt::EditRole != role) return false;
    if (!d->indexPermissible(index)) return false;
    if (!value.canConvert<double>()) return false;

    if (index.column() == 0)
      d->coordinates[index.row()].setX(value.toDouble());
    else
      d->coordinates[index.row()].setY(value.toDouble());
    emit dataChanged(index, index);
    return true;
  }

  Qt::ItemFlags CoordinateModel::flags(const QModelIndex &index) const
  {
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }

  bool CoordinateModel::insertRows(int row, int count, const QModelIndex &parent)
  {
    Q_UNUSED(parent)
    if (!d->rowPermissibleWithEnd(row)) return false;
    beginInsertRows(parent, row, row+count-1);
    // get default value
    QPointF defaultValue;
    if (row< d->coordinates.size()) defaultValue = d->coordinates.at(row);
    else if (!d->coordinates.empty()) defaultValue = d->coordinates.last();
    d->coordinates.insert(row, count, defaultValue);
    endInsertRows();
    return true;
  }

  bool CoordinateModel::removeRows(int row, int count, const QModelIndex &parent)
  {
    Q_UNUSED(parent)
    if (!d->rowPermissible(row)) return false;
    if (!d->rowPermissible(row+count-1)) return false;
    beginRemoveRows(parent, row, row+count-1);
    d->coordinates.remove(row, count);
    endRemoveRows();
    return true;
  }

  QPolygonF CoordinateModel::getCoordinates() const
  {
    return d->coordinates;
  }

  void CoordinateModel::setCoordinates(const QPolygonF &polygon)
  {
    beginResetModel();
    d->coordinates = polygon;
    endResetModel();
  }

} // namespace Molsketch
