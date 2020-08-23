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

#include "atomicmassloader.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace Molsketch {

  const QString &WIKIDATA_URL{"https://query.wikidata.org/sparql"};

  AtomicMassLoader::AtomicMassLoader(QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this))
  {}

  AtomicMassLoader::~AtomicMassLoader()
  {
    qDebug() << "destroying mass loader";
  }

  QMap<QString, qreal> AtomicMassLoader::getAtomicMasses() const {
    return atomicMasses;
  }

  QMap<std::pair<QString, int>, qreal> AtomicMassLoader::getIsotopeMasses() const {
    return isotopeMasses;
  }

  void AtomicMassLoader::updateElementMasses()
  {
    qDebug() << "querying atomic masses";
    QUrl url{WIKIDATA_URL};
    url.setQuery(QUrlQuery{{"query",
                            "SELECT ?element_symbol ?mass WHERE {"
                            "  ?element wdt:P31 wd:Q11344."
                            "  ?element wdt:P246 ?element_symbol."
                            "  ?element wdt:P2067 ?mass."
                            "}"}});
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    auto reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &AtomicMassLoader::elementMassQueryFinished);
    connect(this, &AtomicMassLoader::abortedElementUpdate, reply, &QNetworkReply::abort);
  }

  void AtomicMassLoader::updateIsotopeMasses() {
    qDebug() << "querying isotope masses";
    QUrl url{WIKIDATA_URL};
    url.setQuery(QUrlQuery{{"query",
                           QUrl::toPercentEncoding(
                            "SELECT ?element_symbol (?neutron_number + ?atomic_number AS ?nucleons) ?mass WHERE {"
                            "  ?element wdt:P31 wd:Q11344."
                            "  ?element wdt:P246 ?element_symbol."
                            "  ?isotope wdt:P279 ?element."
                            "  ?isotope wdt:P1148 ?neutron_number."
                            "  ?isotope wdt:P1086 ?atomic_number."
                            "  ?isotope wdt:P2067 ?mass."
                            "}")}});
    QNetworkRequest request{url};
    request.setRawHeader("Accept", "application/json");
    auto reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &AtomicMassLoader::isotopeMassQueryFinished);
    connect(this, &AtomicMassLoader::abortedIsotopeUpdate, reply, &QNetworkReply::abort);
  }

  void AtomicMassLoader::abortElementUpdate() {
    emit abortedElementUpdate();
  }

  void AtomicMassLoader::abortIsotopeUpdate() {
    emit abortIsotopeUpdate();
  }

  void AtomicMassLoader::elementMassQueryFinished() {
    // TODO consolidate with wikiquerywidget
    qDebug() << "received reply from wikidata for element masses";
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    if (!reply->isFinished()) reply->abort();
    reply->deleteLater();
    qDebug("Network error code: %d", reply->error());

    if (reply->error() != QNetworkReply::NoError) {
      QMessageBox::critical(0, "Network error",
                            "Error occurred while trying to obtain data from Wikidata.\n"
                                             "Error code: " + QString::number(reply->error())
                            + " (check QNetworkReply)");
      return;
    }

    auto response = QJsonDocument::fromJson(reply->readAll());
    auto results = response.toVariant().toMap()["results"].toMap()["bindings"].toList();
    atomicMasses.clear();
    for (auto result : results) {
      auto elementSymbol = result.toMap()["element_symbol"].toMap()["value"].toString();
      auto atomicMass = result.toMap()["mass"].toMap()["value"].toDouble();
      atomicMasses[elementSymbol] = atomicMass;
    }
    emit elementMassesUpdated();
  }

  void AtomicMassLoader::isotopeMassQueryFinished() {
    qDebug() << "received reply from wikidata for isotope masses";
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    if (!reply->isFinished()) reply->abort();
    reply->deleteLater();
    qDebug("Network error code: %d", reply->error());

    if (reply->error() != QNetworkReply::NoError) {
      QMessageBox::critical(0, "Network error",
                            "Error occurred while trying to obtain data from Wikidata.\n"
                                             "Error code: " + QString::number(reply->error())
                            + " (check QNetworkReply)");
      return;
    }

    auto response = QJsonDocument::fromJson(reply->readAll());
    auto results = response.toVariant().toMap()["results"].toMap()["bindings"].toList();
    isotopeMasses.clear();
    for (auto result : results) {
      auto elementSymbol = result.toMap()["element_symbol"].toMap()["value"].toString();
      auto nucleonNumber = result.toMap()["nucleons"].toMap()["value"].toInt();
      auto mass = result.toMap()["mass"].toMap()["value"].toDouble();
      isotopeMasses[std::make_pair(elementSymbol, nucleonNumber)] = mass;
    }
    qDebug() << "finished updating isotope masses";
    emit isotopeMassesUpdated();
  }

} // namespace Molsketch
