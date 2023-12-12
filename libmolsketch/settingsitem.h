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
#ifndef MOLSKETCH_SETTINGSITEM_H
#define MOLSKETCH_SETTINGSITEM_H
#include <QObject>

#include "commands.h"

class QWidget;

namespace Molsketch {

  class SettingsFacade;
  class SettingsItemPrivate;

  class SettingsItem : public QObject, public abstractXmlObject
  {
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsItem)
  public:
    explicit SettingsItem(const QString &key, SettingsFacade* facade, QObject *parent = 0, const QVariant &defaultValue = QVariant());
    ~SettingsItem();
    virtual QString serialize() const = 0; // TODO check visibility
    virtual QVariant getVariant() const = 0;
    virtual void set(const QVariant&) = 0;
    virtual void set(const QString&) = 0;
    QString xmlName() const override;
  protected:
    QScopedPointer<SettingsItemPrivate> d_ptr;
    void readAttributes(const QXmlStreamAttributes &attributes) override;
    QXmlStreamAttributes xmlAttributes() const override;
#ifdef QT_DEBUG
    friend QDebug operator<<(QDebug debug, const SettingsItem* setting);
    friend QDebug operator<<(QDebug debug, const SettingsItem& setting);
#endif
  };

#ifdef QT_DEBUG
  QDebug operator<<(QDebug debug, const SettingsItem* setting);
  QDebug operator<<(QDebug debug, const SettingsItem& setting);
#endif

  class DoubleSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    DoubleSettingsItem(const QString &key, SettingsFacade* facade, QObject *parent = 0);
    QString serialize() const override;
    QVariant getVariant() const override;
    qreal get() const;
    void set(const QVariant &) override;
    void set(const QString &) override;
  public slots:
    void set(const qreal&);
  signals:
    void updated(const qreal&);
  };

  class BoolSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    BoolSettingsItem(const QString &key, SettingsFacade* facade, QObject *parent = 0);
    QString serialize() const override;
    QVariant getVariant() const override;
    bool get() const;
    void set(const QVariant &) override;
    void set(const QString &) override;
  public slots:
    void set(const bool&);
  signals:
    void updated(const bool&);
  };

  class ColorSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    ColorSettingsItem(const QString& key, SettingsFacade *facade, QObject *parent = 0);
    QString serialize() const override;
    QVariant getVariant() const override;
    QColor get() const;
    void set(const QVariant &) override;
    void set(const QString &) override;
  public slots:
    void set(const QColor&);
  signals:
    void updated(const QColor&);
  };

  class FontSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    FontSettingsItem(const QString& key, SettingsFacade *facade, QObject *parent = 0);
    QString serialize() const override;
    QVariant getVariant() const override;
    QFont get() const;
    void set(const QVariant &) override;
    void set(const QString &) override;
  public slots:
    void set(const QFont&);
  signals:
    void updated(const QFont&);
  };

  class StringListSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    StringListSettingsItem(const QString& key, SettingsFacade *facade, QObject *parent = 0);
    QString serialize() const override;
    QVariant getVariant() const override;
    QStringList get() const;
    void set(const QVariant&) override;
    void set(const QString&) override;
  public slots:
    void set(const QStringList&);
  signals:
    void updated(const QStringList&);
  };

  class StringSettingsItem : public SettingsItem {
    Q_OBJECT
  public:
    StringSettingsItem(const QString& key, SettingsFacade *facade, QObject *parent = 0, const QVariant &defaultValue = QVariant());
    QString serialize() const override;
    QVariant getVariant() const override;
    QString get() const;
    void set(const QVariant&) override;
  public slots:
    void set(const QString&) override;
  signals:
    void updated(const QString&);
  };

  class SettingsItemUndoCommand : public Commands::Command<SettingsItem, SettingsItemUndoCommand, Commands::SettingsItemId> {
    QUndoStack *stack;
    QVariant newValue;
  public:
    SettingsItemUndoCommand(SettingsItem *item, const QVariant &newValue, const QString &text = QString(), QUndoStack *stack = 0);
    static SettingsItemUndoCommand *forCurrentValue(SettingsItem *item, const QString &text = QString(), QUndoStack *stack = 0);
    void redo() override;
  protected:
    QUndoStack *getStack() override;
  };
} // namespace Molsketch

#endif // MOLSKETCH_SETTINGSITEM_H
