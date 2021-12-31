/***************************************************************************
 *   Copyright (C) 2021 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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
#ifndef PATHINPUT_H
#define PATHINPUT_H

#include <QWidget>

namespace Ui {
  class PathInput;
}

class PathInput : public QWidget
{
  Q_OBJECT
public:
  explicit PathInput(QWidget *parent = nullptr);
  ~PathInput();
  QString value() const;
  QString label() const;
  static PathInput *fileInput(const QString &extension, QWidget *parent = nullptr);
  static PathInput *folderInput(QWidget *parent = nullptr);
protected:
  virtual QString getValueFromDialog() const = 0;
private:
  Ui::PathInput *ui;

signals:
  void pathStringChanged(const QString&);
public slots:
  void setValue(const QString &);
  void setLabel(const QString &);
private slots:
  void on_dialogButton_clicked();
};

class FilePathInput : public PathInput {
  QString extension;
  QString getValueFromDialog() const override;
public:
  FilePathInput(const QString &extension, QWidget *parent);
  void setExtensionFilter(const QString &extension);
};

class FolderPathInput : public PathInput {
  QString getValueFromDialog() const override;
public:
  FolderPathInput(QWidget *parent);
};

#endif // PATHINPUT_H
