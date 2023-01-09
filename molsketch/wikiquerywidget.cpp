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
#include "wikiquerywidget.h"
#include "ui_wikiquerywidget.h"
#include "molecule.h"
#include "obabelifaceloader.h"
#include "constants.h"

#include <QJsonDocument>
#include <QLibrary>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <librarymodel.h>
#include <moleculemodelitem.h>
#include <QDebug>
#include <QFileDialog>
#include <QMimeData>

#ifdef Q_OS_WINDOWS
#define OBABELOSSUFFIX ".dll"
#else
#define OBABELOSSUFFIX
#endif

using namespace Molsketch;

class ScalingLibraryModel : public LibraryModel {
public:
  explicit ScalingLibraryModel(QObject *parent = nullptr) : LibraryModel(parent) {}
  QMimeData *mimeData(const QModelIndexList &indexes) const override {
    auto data = LibraryModel::mimeData(indexes);
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    qreal scalingFactor = 1.;
    out << scalingFactor;
    data->setData(bondLengthMimeType, ba);
    return data;
  }
};

WikiQueryWidget::WikiQueryWidget(OBabelIfaceLoader *loader, const QString &queryUrl, QWidget *parent) :
  QDockWidget(parent),
  ui(new Ui::WikiQueryWidget),
  manager(new QNetworkAccessManager(this)),
  obloader(loader),
  queryUrl(queryUrl)
{
  setObjectName("wikidata-query-widget");
  ui->setupUi(this);
  ui->moleculeListView->setModel(new ScalingLibraryModel(this));
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processMoleculeQuery(QNetworkReply*)));
  connect(loader, SIGNAL(inchiAvailable(bool)), ui->dockWidgetContents, SLOT(setEnabled(bool)));
  ui->progressWidget->hide();
  ui->queryInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[^\"]*"),
                                                               ui->queryInput));
  // TODO validator for input: do not allow quotation marks. Check input constraints
}

WikiQueryWidget::~WikiQueryWidget()
{
  delete ui;
}

void WikiQueryWidget::setQueryUrl(const QString &newQueryUrl) {
  queryUrl = newQueryUrl;
}

void WikiQueryWidget::on_searchButton_clicked() {
  startMoleculeQuery(ui->queryInput->text());
}

void WikiQueryWidget::on_queryInput_textChanged(const QString &newText) {
  ui->searchButton->setDisabled(newText.length() < 5);
}

class InChIItem : public MoleculeModelItem {
  QString inchi;
  QString name;
  OBabelIfaceLoader *obloader;
  bool performScaling() const override {
    return true;
  }
public:
  InChIItem(QString name, QString inchi, OBabelIfaceLoader* obloader) : inchi(inchi), name(name), obloader(obloader) {}
  Molecule* produceMolecule() const override {
    qDebug() << "generating from InChI" << inchi;
    Molecule* m = obloader->convertInChI(inchi);
    if (m) m->setName(name);
    return m;
  }
};

void WikiQueryWidget::processMoleculeQuery(QNetworkReply *reply) {
  ui->progressWidget->hide();
  if (!reply->isFinished()) reply->abort();
  if (!reply) return;

  qDebug("Network error code: %d", reply->error());
  if (reply->error() != QNetworkReply::NoError) {
    QMessageBox::critical(0, "Network error",
                          "Error occurred while trying to obtain data from Wikidata.\n"
                                           "Error code: " + QString::number(reply->error())
                          + " (check QNetworkReply)");
    return;
  }

  QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

  QVariantList results = response.toVariant().toMap()["results"].toMap()["bindings"].toList();

  QList<QPair<QString, QString>> namesAndInchiStrings;
  foreach (QVariant result, results) {
    QString label = result.toMap()["label"].toMap()["value"].toString();
    QString smiles = result.toMap()["smiles"].toMap()["value"].toString();
    QString inchi = result.toMap()["inchi"].toMap()["value"].toString();
    QString isomer = result.toMap()["isomer"].toMap()["value"].toString();
    qInfo() << "obtained molecule from wikidata:" << label << smiles << inchi << isomer;
    namesAndInchiStrings << qMakePair(label, inchi);
  }
  std::sort(namesAndInchiStrings.begin(), namesAndInchiStrings.end());
  QList<MoleculeModelItem*> items;
  for (auto item : namesAndInchiStrings)
    items << new InChIItem(item.first, item.second, obloader);
  ((LibraryModel*) ui->moleculeListView->model())->setMolecules(items);
  reply->deleteLater();
}

void WikiQueryWidget::startMoleculeQuery(const QString& queryString) {
  QUrlQuery query;
  query.setQueryItems(QList<QPair<QString, QString> >() <<
                      qMakePair(QString("query"),
                                "SELECT (SAMPLE(?label) AS ?label)"
                                " (SAMPLE(?smiles) AS ?smiles)"
                                " (SAMPLE(?inchi) AS ?inchi)"
                                " (SAMPLE(?isomer) AS ?isomer) WHERE"
                                " { ?qnumber wdt:P31 wd:Q11173"
                                " . ?qnumber rdfs:label ?label"
                                " . ?qnumber wdt:P233 ?smiles"
                                " . ?qnumber wdt:P234 ?inchi"
                                " . ?qnumber wdt:P2017 ?isomer"
                                " . SERVICE wikibase:label { bd:serviceParam wikibase:language \"en\". } "
                                "FILTER(REGEX(?label, \"" + queryString + "\", \"i\")) "
                                "} GROUP BY ?qnumber"
//                                " LIMIT 100 ORDER BY DESC(?label)"
                                // TODO allow for use of smiles ("isomer" column)
                                // TODO take language from locale
                                ));
  QUrl url(queryUrl);
  url.setQuery(query);
  QNetworkRequest request(url);
  request.setRawHeader("Accept", "application/json");
  QNetworkReply *reply = manager->get(request);
  connect(ui->stopButton, SIGNAL(clicked(bool)), reply, SLOT(abort()));
  ui->progressWidget->show();
}
