/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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
#ifndef WIKIQUERYWIDGET_H
#define WIKIQUERYWIDGET_H

#include <QDockWidget>

class QNetworkAccessManager;
class QNetworkReply;
class OBabelIfaceLoader;

namespace Ui {
class WikiQueryWidget;
}

class WikiQueryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit WikiQueryWidget(OBabelIfaceLoader *loader, const QString &queryUrl, QWidget *parent = 0);
    ~WikiQueryWidget();

public slots:
  void setQueryUrl(const QString &);
private slots:
  void on_searchButton_clicked();
  void on_queryInput_textChanged(const QString &newText);
  void processMoleculeQuery(QNetworkReply* reply);

private:
    Ui::WikiQueryWidget *ui;
    QNetworkAccessManager *manager;
    OBabelIfaceLoader *obloader;
    QString queryUrl;
    void startMoleculeQuery(const QString &queryString);
};

#endif // WIKIQUERYWIDGET_H
