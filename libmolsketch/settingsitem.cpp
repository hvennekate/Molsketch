/***************************************************************************
 *   Copyright (C) 2017 Hendrik Vennekate                                  *
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
#include "settingsfacade.h"
#include "settingsitem.h"
#include "stringify.h"
#include <QColor>
#include <QFont>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

const QString XML_VALUE_ATTRIBUTE("value");

namespace Molsketch {

  struct SettingsItemPrivate {
    SettingsFacade *facade;
    QString key;
    bool locked;
    QVariant defaultValue;
  };

  SettingsItem::SettingsItem(const QString& key, SettingsFacade *facade, QObject *parent, const QVariant &defaultValue)
    : QObject(parent),
      d_ptr(new SettingsItemPrivate)
  {
    Q_D(SettingsItem);
    d->facade = facade;
    d->key = key;
    d->locked = false;
    d->defaultValue = defaultValue;
  }

  SettingsItem::~SettingsItem() {}

  QString SettingsItem::xmlName() const {
    Q_D(const SettingsItem);
    return d->key;
  }

  void SettingsItem::readAttributes(const QXmlStreamAttributes &attributes) {
    set(attributes.value(XML_VALUE_ATTRIBUTE).toString());
  }

  QXmlStreamAttributes SettingsItem::xmlAttributes() const {
    QXmlStreamAttributes result;
    result.append(XML_VALUE_ATTRIBUTE, serialize());
    return result;
  }

  DoubleSettingsItem::DoubleSettingsItem(const QString& key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString DoubleSettingsItem::serialize() const {
    return QString::number(get());
  }

  QVariant DoubleSettingsItem::getVariant() const {
    return QVariant(get());
  }

  qreal DoubleSettingsItem::get() const {
    qreal value = d_ptr->facade->value(d_ptr->key).toDouble();
    return value;
  }

  void DoubleSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void DoubleSettingsItem::set(const QString &value) {
    set(value.toDouble());
  }

  void DoubleSettingsItem::set(const qreal &value) {
    set(QVariant(value));
  }

  BoolSettingsItem::BoolSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString BoolSettingsItem::serialize() const {
    return get() ? "true" : "false";
  }

  QVariant BoolSettingsItem::getVariant() const {
    return QVariant(get());
  }

  bool BoolSettingsItem::get() const {
    bool value = d_ptr->facade->value(d_ptr->key).toBool();
    return value;
  }

  void BoolSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void BoolSettingsItem::set(const QString &value) {
    set(value.toLower() == "true");
  }

  void BoolSettingsItem::set(const bool &value) {
    set(QVariant(value));
  }

  ColorSettingsItem::ColorSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString ColorSettingsItem::serialize() const {
    return stringify(get());
  }

  QVariant ColorSettingsItem::getVariant() const {
    return QVariant(get());
  }

  QColor ColorSettingsItem::get() const {
    QColor value = d_ptr->facade->value(d_ptr->key).value<QColor>();
    return value;
  }

  void ColorSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void ColorSettingsItem::set(const QString &value) {
    set(makeFromString<QColor>(value));
  }

  void ColorSettingsItem::set(const QColor &value) {
    set(QVariant(value));
  }

  FontSettingsItem::FontSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString FontSettingsItem::serialize() const {
    return stringify(get());
  }

  QVariant FontSettingsItem::getVariant() const {
    return QVariant(get());
  }

  QFont FontSettingsItem::get() const {
    QFont value = d_ptr->facade->value(d_ptr->key).value<QFont>();
    return value;
  }

  void FontSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void FontSettingsItem::set(const QString &value) {
    set(makeFromString<QFont>(value));
  }

  void FontSettingsItem::set(const QFont &value) {
    set(QVariant(value));
  }

  StringListSettingsItem::StringListSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString StringListSettingsItem::serialize() const {
    return stringify(get());
  }

  QVariant StringListSettingsItem::getVariant() const {
    return d_ptr->facade->value(d_ptr->key); // TODO fix in others as well
  }

  QStringList StringListSettingsItem::get() const {
    return getVariant().toStringList();
  }

  void StringListSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void StringListSettingsItem::set(const QString &value) {
    auto sl = makeFromString<QStringList>(value);
    qDebug() << "making string list: " << sl;
    set(makeFromString<QStringList>(value));
  }

  void StringListSettingsItem::set(const QStringList &value) {
    set(QVariant(value));
  }

  SettingsItemUndoCommand::SettingsItemUndoCommand(SettingsItem *item, const QVariant &newValue, const QString &text, QUndoStack *stack)
    : Commands::Command<SettingsItem, SettingsItemUndoCommand, Commands::SettingsItemId>(item, text),
      stack(stack),
      newValue(newValue){}

  SettingsItemUndoCommand* SettingsItemUndoCommand::forCurrentValue(SettingsItem *item, const QString &text, QUndoStack *stack) {
    return new SettingsItemUndoCommand(item, item->getVariant(), text, stack);
  }

  void SettingsItemUndoCommand::redo() {
    QVariant currentValue = this->getItem()->getVariant();
    this->getItem()->set(newValue);
    newValue = currentValue;
  }

  QUndoStack *SettingsItemUndoCommand::getStack() {
    return stack;
  }

  StringSettingsItem::StringSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent, const QVariant &defaultValue)
    : SettingsItem(key, facade, parent, defaultValue) {}

  QString StringSettingsItem::serialize() const {
    return get();
  }

  QVariant StringSettingsItem::getVariant() const {
    return d_ptr->facade->value(d_ptr->key, d_ptr->defaultValue); // TODO extend others with default value as well
  }

  QString StringSettingsItem::get() const {
    return getVariant().toString();
  }

  void StringSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void StringSettingsItem::set(const QString &value) {
    set(QVariant(value));
  }

#ifdef QT_DEBUG
  QDebug operator<<(QDebug debug, const SettingsItem &setting) {
    debug.nospace() << "Settings item ("
                    << "key: " << setting.d_ptr->key
                    << ", facade: " << setting.d_ptr->facade
                    << ", locked: " << setting.d_ptr->locked
                    << ")";
    return debug;
  }

  QDebug operator<<(QDebug debug, const SettingsItem *setting) {
    debug.nospace() << "SettingsItem (" << (void*) setting << ", key: " << setting->d_ptr->key << ")";
    return debug;
  }

#endif

  StringDoubleMapSettingsItem::StringDoubleMapSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString StringDoubleMapSettingsItem::serialize() const {
    auto jsonMap = QJsonObject();
    auto map = get();
    for (auto it = map.cbegin(); it != map.cend(); ++it)
      jsonMap.insert(it.key(), {it.value()});
    auto jsonDocument = QJsonDocument();
    jsonDocument.setObject(jsonMap);
    return jsonDocument.toJson(QJsonDocument::Compact);
  }

  QVariant StringDoubleMapSettingsItem::getVariant() const {
    return d_ptr->facade->value(d_ptr->key);
  }

  QMap<QString, qreal> StringDoubleMapSettingsItem::get() const {
    auto variantMap = getVariant().toMap();
    QMap<QString, qreal> result;
    for (auto it = variantMap.cbegin(); it != variantMap.cend(); ++it)
      result[it.key()] = it.value().toDouble();
    return result;
  }

  void StringDoubleMapSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void StringDoubleMapSettingsItem::set(const QString &data) {
    auto jsonObject = QJsonDocument::fromJson(data.toUtf8()).object();
    QMap<QString, qreal> newValue;
    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
      newValue[it.key()] = it.value().toDouble();
    set(newValue);
  }

  void StringDoubleMapSettingsItem::set(const QMap<QString, qreal> &value) {
    QVariantMap variantMap;
    for (auto it = value.cbegin(); it != value.cend(); ++it)
      variantMap[it.key()] = it.value();
    set(variantMap);
  }

  const char *STRING_KEY = "keyString";
  const char *INT_KEY = "keyInt";
  const char *VALUE = "value";

  StringIntDoubleMapSettingsItem::StringIntDoubleMapSettingsItem(const QString &key, SettingsFacade *facade, QObject *parent)
    : SettingsItem(key, facade, parent) {}

  QString StringIntDoubleMapSettingsItem::serialize() const {
    QJsonArray jsonList;
    auto map = get();
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
      QJsonObject entry;
      entry.insert(STRING_KEY, it.key().first);
      entry.insert(INT_KEY, it.key().second);
      entry.insert(VALUE, it.value());
      jsonList << entry;
    }
    auto jsonDocument = QJsonDocument();
    jsonDocument.setArray(jsonList);
    return jsonDocument.toJson(QJsonDocument::Compact);
  }

  QVariant StringIntDoubleMapSettingsItem::getVariant() const {
    return d_ptr->facade->value(d_ptr->key);
  }

  QMap<std::pair<QString, int>, qreal> StringIntDoubleMapSettingsItem::get() const {
    QMap<std::pair<QString, int>, qreal> result;
    auto variantMap = getVariant().toMap();
    for (auto it = variantMap.cbegin(); it != variantMap.cend(); ++it) {
      auto string = it.key();
      auto innerMap = it.value().toMap();
      for (auto inner = innerMap.cbegin(); inner != innerMap.cend(); ++inner)
        result[std::make_pair(string, inner.key().toInt())] = inner.value().toDouble();
    }
    return result;
  }

  void StringIntDoubleMapSettingsItem::set(const QVariant &value) {
    if (d_ptr->locked) return;
    d_ptr->locked = true;
    qInfo() << "Setting" << d_ptr->key << "to new value" << value;
    d_ptr->facade->setValue(d_ptr->key, value);
    emit updated(get());
    d_ptr->locked = false;
  }

  void StringIntDoubleMapSettingsItem::set(const QString &data) {
    auto jsonList = QJsonDocument::fromJson(data.toUtf8()).array();
    QMap<std::pair<QString, int>, qreal> newValue;
    for (auto value : jsonList) {
      auto jsonObject = value.toObject();
      newValue[std::make_pair(jsonObject[STRING_KEY].toString(), jsonObject[INT_KEY].toInt())]
          = jsonObject[VALUE].toDouble();
    }
    set(newValue);
  }

  void StringIntDoubleMapSettingsItem::set(const QMap<std::pair<QString, int>, qreal> &value) {
    QVariantMap variantMap;
    for (auto it = value.cbegin(); it != value.cend(); ++it) {
      auto string = it.key().first;
      auto integer = it.key().second;
      auto oldMap = variantMap[string].toMap();
      oldMap[QString::number(integer)] = it.value();
      variantMap[string] = oldMap;
    }
    set(variantMap);
  }

} // namespace Molsketch
