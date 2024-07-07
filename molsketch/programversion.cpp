/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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

#include "programversion.h"

struct ProgramVersion::ProgramVersionPrivate {
  unsigned int version, subversion, patch, build;
};

ProgramVersion::ProgramVersion(const QString &version)
  : d(new ProgramVersionPrivate) {
  d->version = version.section('.', 0,0).toInt();
  d->subversion = version.section('.', 1,1).toInt();
  d->patch = version.section('.', 2,2).toInt();
  d->build = version.section('.', 3,3).toInt();
}

ProgramVersion::ProgramVersion(int version, int subversion, int patch, int build)
  : d(new ProgramVersionPrivate) {
  d->version = version;
  d->subversion = subversion;
  d->patch = patch;
  d->build = build;
}

ProgramVersion::~ProgramVersion() {
  delete d;
}

unsigned int ProgramVersion::patch() const {
  return d->patch;
}

unsigned int ProgramVersion::build() const {
  return d->build;
}

bool ProgramVersion::operator ==(const ProgramVersion &other) const {
  return d->version == other.d->version
      && d->subversion == other.d->subversion
      && d->patch == other.d->patch
      && d->build == other.d->build;
}

bool ProgramVersion::operator !=(const ProgramVersion &other) const {
  return ! ((*this) == other);
}

bool ProgramVersion::operator <(const ProgramVersion &other) const {
  return d->version < other.d->version
      || (d->version == other.d->version && (
        d->subversion < other.d->subversion
        || (d->subversion == other.d->subversion && (
          d->patch < other.d->patch
          || (d->patch == other.d->patch && d->build < other.d->build)))));
}

bool ProgramVersion::operator >(const ProgramVersion &other) const {
  return !((*this) <= other);
}

bool ProgramVersion::operator <=(const ProgramVersion &other) const {
  return (*this) == other || (*this) < other;
}

bool ProgramVersion::operator >=(const ProgramVersion &other) const {
  return !((*this) < other);
}

QString ProgramVersion::toString() const
{
  return QString("%1.%2.%3.%4").arg(d->version).arg(d->subversion).arg(d->patch).arg(d->build);
}

unsigned int ProgramVersion::subversion() const {
  return d->subversion;
}

unsigned int ProgramVersion::version() const {
  return d->version;
}
