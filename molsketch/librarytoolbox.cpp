/***************************************************************************
 *   Copyright (C) 2018 Hendrik Vennekate                                  *
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
#include "librarytoolbox.h"
#include <QToolBox>
#include <QPushButton>
#include <QToolBox>
#include <QVBoxLayout>
#include "helpforemptytoolbox.h"
#include "librarylistwidget.h"

struct LibraryToolBoxPrivate {
  QToolBox *toolBox;
  QPushButton *refreshLibraries;
  void createLibraryWidgetLayout(QWidget *parent) {
    QLayout* layout = new QVBoxLayout(parent);
    layout->addWidget(refreshLibraries = new QPushButton(QIcon::fromTheme("view-refresh", QIcon(":icons/view-refresh.svg")), "", parent));
    layout->addWidget(toolBox = new QToolBox(parent));
    layout->addWidget(new HelpForEmptyToolBox(QObject::tr("Define libraries using <b>File > Preferences... > Libraries</b>."), toolBox, parent));
    layout->setContentsMargins(0, 0, 0, 0);
  }
};

LibraryToolBox::LibraryToolBox(const QStringList &folders, QWidget *parent)
  :	QDockWidget(tr("Molecule Libraries"), parent),
    d_ptr(new LibraryToolBoxPrivate)
{
  Q_D(LibraryToolBox);
  setObjectName("toolbox-dockwidget");
  setMinimumWidth(270);

  QWidget* content = new QWidget(this);
  d->createLibraryWidgetLayout(content);
  setWidget(content);

  rebuildLibraries(folders);
#ifdef __ANDROID__
  hide();
#endif
}

void LibraryToolBox::rebuildLibraries(const QStringList &folders)
{
  Q_D(LibraryToolBox);
  for(auto library : d->toolBox->findChildren<LibraryListWidget*>())
    delete library;

  for(auto folder : folders) {
    LibraryListWidget* library = new LibraryListWidget(folder, d->toolBox);
    d->toolBox->addItem(library, library->title());
    connect(d->refreshLibraries, SIGNAL(clicked()), library, SLOT(refreshItems()));
  }
}

LibraryToolBox::~LibraryToolBox() {
}
