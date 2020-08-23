/***************************************************************************
 *   Copyright (C) 2020 by Hendrik Vennekate                               *
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

#ifndef ATOMICMASSLOADER_H
#define ATOMICMASSLOADER_H

#include <QMutex>
#include <QNetworkAccessManager>
#include <QObject>

namespace Molsketch {

  class AtomicMassLoader : public QObject
  {
    Q_OBJECT
  private:
    QNetworkAccessManager *manager;
    QMap<QString, qreal> atomicMasses;
    QMap<std::pair<QString, int>, qreal> isotopeMasses;
    QMutex atomicMassesMutex;
    QMutex isotopeMassesMutex;
  public:
    explicit AtomicMassLoader(QObject *parent = nullptr);
    ~AtomicMassLoader();
    QMap<QString, qreal> getAtomicMasses() const;
    QMap<std::pair<QString, int>, qreal> getIsotopeMasses() const;
  public slots:
    void updateElementMasses();
    void updateIsotopeMasses();
    void abortElementUpdate();
    void abortIsotopeUpdate();
  private slots:
    void elementMassQueryFinished();
    void isotopeMassQueryFinished();
  signals:
    void abortedElementUpdate();
    void abortedIsotopeUpdate();
    void elementMassesUpdated();
    void isotopeMassesUpdated();
  };

} // namespace Molsketch

#endif // ATOMICMASSLOADER_H
