/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QFont>
#include <QRegularExpression>
#include <QSettings>
#include "scenesettings.h"
#include "settingsfacade.h"
#include "settingsitem.h"

Q_DECLARE_METATYPE(Molsketch::SceneSettings::MouseWheelMode)

namespace Molsketch {

  static const char ATOM_FONT[] = "atom-font";
  static const char ARROW_WIDTH[] = "arrow-width";
  static const char AUTO_ADD_HYDROGEN[] = "auto-add-hydrogen";
  static const char BOND_ANGLE[] = "bond-angle";
  static const char BOND_LENGTH[] = "bond-length";
  static const char BOND_WIDTH[] = "bond-width";
  static const char CARBON_VISIBLE[] = "carbon-visible";
  static const char CHARGE_VISIBLE[] = "charge-visible";
  static const char ELECTRON_SYSTEMS_VISIBLE[] = "electron-systems-visible";
  static const char GRID_COLOR[] = "grid-color";
  static const char GRID_LINEWIDTH[] = "grid-linewidth";
  static const char HORIZONTAL_GRID_SPACING[] = "horizontal-grid-spacing";
  static const char LONE_PAIR_LENGTH[] = "lone-pair-length";
  static const char LONE_PAIR_LINE_WIDTH[] = "lone-pair-line-width";
  static const char MOUSE_CYCLE_MODE[] = "mouse-cycle-mode";
  static const char RADICAL_DIAMETER[] = "radical-diameter";
  static const char VERTICAL_GRID_SPACING[] = "vertical-grid-spacing";
  static const char DEFAULT_COLOR[] = "default-color";
  static const char FRAME_LINE_WIDTH[] = "frame-line-width";
  static const char HYDROGEN_VISIBLE[] = "hydrogen-visible";
  static const char LONE_PAIRS_VISIBLE[] = "lone-pairs-visible";

  class SceneSettingsPrivate {
    Q_DISABLE_COPY(SceneSettingsPrivate)
  public:
    SettingsFacade *settingsFacade;
    SceneSettings *parent;
    DoubleSettingsItem *bondAngle,
    *gridLineWidth,
    *verticalGridSpacing,
    *horizontalGridSpacing,
    *radicalDiameter,
    *lonePairLength,
    *lonePairLineWidth,
    *bondWidth,
    *bondLength,
    *arrowWidth,
    *frameLineWidth;

    BoolSettingsItem *carbonVisible,
    *electronSystemsVisible,
    *chargeVisible,
    *autoAddHydrogen,
    *hydrogenVisible,
    *lonePairsVisible;

    ColorSettingsItem *gridColor,
    *defaultColor;
    FontSettingsItem *atomFont;

    QMap<QString, SettingsItem*> settingsItems;

    template<typename ITEM, typename VALUE>
    ITEM* initializeSetting(const QString& key, const VALUE& defaultValue) {
      if (!settingsFacade->value(key).isValid())
        settingsFacade->setValue(key, defaultValue);
      ITEM* result = new ITEM(key, settingsFacade, parent);
      settingsItems[key] = result;
      return result;
    }

    DoubleSettingsItem* initializeDoubleSetting(const QString& key, const qreal& defaultValue) {
      return initializeSetting<DoubleSettingsItem, qreal>(key, defaultValue);
    }

    BoolSettingsItem* initializeBoolSetting(const QString& key, const bool& defaultValue) {
      return initializeSetting<BoolSettingsItem, bool>(key, defaultValue);
    }

    SceneSettingsPrivate(SettingsFacade *facade, SceneSettings* parent)
      : settingsFacade(facade),
        parent(parent)
    {
      atomFont = initializeSetting<FontSettingsItem, QFont>(ATOM_FONT, QFont());
      arrowWidth = initializeDoubleSetting(ARROW_WIDTH, 1.5);
      autoAddHydrogen = initializeBoolSetting(AUTO_ADD_HYDROGEN, true);
      bondAngle = initializeDoubleSetting(BOND_ANGLE, 30);
      bondLength = initializeDoubleSetting(BOND_LENGTH, 40);
      bondWidth = initializeDoubleSetting(BOND_WIDTH, 2);
      carbonVisible = initializeBoolSetting(CARBON_VISIBLE, false);
      chargeVisible = initializeBoolSetting(CHARGE_VISIBLE, true);
      defaultColor = initializeSetting<ColorSettingsItem, QColor>(DEFAULT_COLOR, QColor(Qt::black));
      electronSystemsVisible = initializeBoolSetting(ELECTRON_SYSTEMS_VISIBLE, false);
      frameLineWidth = initializeDoubleSetting(FRAME_LINE_WIDTH, 1.5);
      gridLineWidth = initializeDoubleSetting(GRID_LINEWIDTH, 0);
      gridColor = initializeSetting<ColorSettingsItem, QColor>(GRID_COLOR, QColor(Qt::gray));
      horizontalGridSpacing = initializeDoubleSetting(HORIZONTAL_GRID_SPACING, 10);
      hydrogenVisible = initializeBoolSetting(HYDROGEN_VISIBLE, true);
      lonePairLength = initializeDoubleSetting(LONE_PAIR_LENGTH, 7);
      lonePairLineWidth = initializeDoubleSetting(LONE_PAIR_LINE_WIDTH, 1);
      lonePairsVisible = initializeBoolSetting(LONE_PAIRS_VISIBLE, false);
      radicalDiameter = initializeDoubleSetting(RADICAL_DIAMETER, 3);
      verticalGridSpacing = initializeDoubleSetting(VERTICAL_GRID_SPACING, 10);
    }
  };

  SceneSettings::SceneSettings(SettingsFacade* facade, QObject *parent)
    : QObject(parent),
      d_ptr(new SceneSettingsPrivate(facade, this))
  {
    Q_D(SceneSettings);
    d->settingsFacade->setParent(this);
  }

  SceneSettings::~SceneSettings() {}

  QString dashifyCamelCaseAttributeName(const QStringRef &originalName) {
    return originalName.toString().replace(QRegularExpression("([A-Z])"), "-\\1").toLower().remove("-molscene-");
  }

  void SceneSettings::setFromAttributes(const QXmlStreamAttributes &attributes) {
    Q_D(SceneSettings);
    for (auto attribute : attributes) {
      QString name(dashifyCamelCaseAttributeName(attribute.name()));
      if (d->settingsItems.contains(name))
        d->settingsItems[name]->set(attribute.value().toString());
    }
  }

  QString SceneSettings::xmlName() const {
    return xmlClassName();
  }

  QString SceneSettings::xmlClassName() { return "settings"; }

#define PROPERTY_DEF(TYPE, NAME) \
  const TYPE* SceneSettings::NAME() const { return d_ptr->NAME; } \
  TYPE* SceneSettings::NAME() { return d_ptr->NAME; }

  PROPERTY_DEF(FontSettingsItem, atomFont)
  PROPERTY_DEF(ColorSettingsItem, gridColor)
  PROPERTY_DEF(ColorSettingsItem, defaultColor)

#define PROPERTY(NAME, TYPE, CONFIGSTRING) \
  void SceneSettings::set##NAME(const TYPE& value) { settingsFacade().setValue(CONFIGSTRING, value); \
  emit settingsChanged(); } \
  TYPE SceneSettings::get##NAME() const { return settingsFacade().value(CONFIGSTRING).value<TYPE>(); }

  PROPERTY(MouseWheelMode, SceneSettings::MouseWheelMode, MOUSE_CYCLE_MODE)

#define BOOL_PROPERTY_DEF(NAME) \
  const BoolSettingsItem* SceneSettings::NAME() const { return d_ptr->NAME; } \
  BoolSettingsItem* SceneSettings::NAME() { return d_ptr->NAME; }

  BOOL_PROPERTY_DEF(carbonVisible)
  BOOL_PROPERTY_DEF(electronSystemsVisible)
  BOOL_PROPERTY_DEF(chargeVisible)
  BOOL_PROPERTY_DEF(autoAddHydrogen)
  BOOL_PROPERTY_DEF(hydrogenVisible)
  BOOL_PROPERTY_DEF(lonePairsVisible)

#define REAL_PROPERTY_DEF(NAME) \
  const DoubleSettingsItem* SceneSettings::NAME() const { return d_ptr->NAME; } \
  DoubleSettingsItem* SceneSettings::NAME() { return d_ptr->NAME; }

  REAL_PROPERTY_DEF(bondAngle)
  REAL_PROPERTY_DEF(gridLineWidth)
  REAL_PROPERTY_DEF(verticalGridSpacing)
  REAL_PROPERTY_DEF(horizontalGridSpacing)
  REAL_PROPERTY_DEF(radicalDiameter)
  REAL_PROPERTY_DEF(lonePairLength)
  REAL_PROPERTY_DEF(lonePairLineWidth)
  REAL_PROPERTY_DEF(bondWidth)
  REAL_PROPERTY_DEF(bondLength)
  REAL_PROPERTY_DEF(arrowWidth)
  REAL_PROPERTY_DEF(frameLineWidth)

  SettingsFacade &SceneSettings::settingsFacade() {
    Q_D(SceneSettings);
    return *(d->settingsFacade);
  }

  QList<const XmlObjectInterface *> SceneSettings::children() const {
    Q_D(const SceneSettings);
    QList<const XmlObjectInterface *> result;
    for (auto settingsItem : d->settingsItems.values())
      result << dynamic_cast<const XmlObjectInterface *>(settingsItem);
    return result;
  }

  XmlObjectInterface *SceneSettings::produceChild(const QString &name, const QString &type) {
    Q_UNUSED(type)
    Q_D(SceneSettings);
    if (d->settingsItems.contains(name))
      return d->settingsItems[name];
    return nullptr;
  }

  const SettingsFacade &SceneSettings::settingsFacade() const
  {
    Q_D(const SceneSettings);
    return *(d->settingsFacade);
  }
} // namespace Molsketch
