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

#include "moleculemodelitem.h"
#include "molecule.h"
#include "molscene.h"
#include "scenesettings.h"
#include "settingsitem.h"
#include <QIcon>
#include <QLibrary>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QtMath>

#ifdef Q_OS_WINDOWS
#define OBABELOSSUFFIX ".dll"
#else
#define OBABELOSSUFFIX
#endif

namespace Molsketch {

  class MoleculeModelItemPrivate {
    Q_DISABLE_COPY(MoleculeModelItemPrivate)
  public:
    MoleculeModelItemPrivate()
      : molecule(nullptr)
    {}
    Molecule *molecule;
    QIcon icon;
    ~MoleculeModelItemPrivate() {
      delete molecule;
    }
    void ensureMolecule(MoleculeModelItem* item) {
      if (molecule) return;
      molecule = item->produceMolecule();
      qDebug() << "produced molecule: " << (void*) molecule << "from item" << (void*) item;
    }
  };

  MoleculeModelItem::MoleculeModelItem()
    : d_ptr(new MoleculeModelItemPrivate)
  {}

  bool MoleculeModelItem::performScaling() const {
    return false;
  }

  MoleculeModelItem::~MoleculeModelItem() {}

  QIcon MoleculeModelItem::icon() {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    if (!d->molecule) return QIcon();
    if (d->icon.isNull()) d->icon = QIcon(renderMolecule(*(d->molecule)));
    return d->icon;
  }

  QString MoleculeModelItem::name() {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    if (!d->molecule) return QString();
    return d->molecule->getName();
  }

  const Molecule *MoleculeModelItem::getMolecule() {
    Q_D(MoleculeModelItem);
    d->ensureMolecule(this);
    return d->molecule;
  }

  MoleculeModelItem *MoleculeModelItem::fromXml(QByteArray xml) {
    class XmlItem : public MoleculeModelItem {
        Molecule* molecule;
    public:
        XmlItem(const QByteArray& input)
          : molecule (new Molecule())
        {
          QXmlStreamReader in(input);
          in >> *molecule;
        }
        Molecule *produceMolecule() const override {
          return molecule;
        }
      };
    return new XmlItem(xml);
  }

  MoleculeModelItem *MoleculeModelItem::fromMolecule(Molecule *molecule) {
    class MoleculeItem : public MoleculeModelItem {
      Molecule *molecule;
    public:
      MoleculeItem(Molecule* molecule)
        : molecule(molecule)
      {}
      Molecule *produceMolecule() const override {
        return molecule;
      }
    };
    return new MoleculeItem(molecule);
  }

  QPixmap MoleculeModelItem::renderMolecule(const Molecule &input) {
    Molecule *molecule = new Molecule(input);
    MolScene renderScene;
    if (performScaling()) molecule->scale(renderScene.settings()->bondLength()->get());
    qDebug() << "rendering molecule" << &input;
    if (molecule->atoms().size() > 20)
      renderScene.setRenderMode(MolScene::RenderColoredCircles);
    renderScene.addItem(molecule);
    renderScene.settings()->chargeVisible()->set(true);
    renderScene.setSceneRect(molecule->boundingRect());
    QPixmap pixmap(qCeil(renderScene.width()), qCeil(renderScene.height()));
    if (pixmap.isNull()) return pixmap;

    pixmap.fill();
    // Creating and setting the painter
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    renderScene.render(&painter);
    qDebug() << "rendered molecule" << &input;
    return pixmap;
  }
} // namespace Molsketch
